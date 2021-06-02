#include "colony.h"
#include "quality.h"
#include "annealing.h"
#include "time_util.h"

#include <algorithm>
#include <cmath>
#include <chrono>

using namespace myaco;

Colony::Colony(Schedule initial_schedule, const Matrix3D<double>& visibility)
        : best_schedule_(std::move(initial_schedule))
        , visibility_(visibility)
        , rng_(std::chrono::system_clock::now().time_since_epoch().count()) {
    InitTrail();
}

void Colony::MakeIteration(Timer::fsec max_time = Timer::kInfiniteSeconds) {
    std::vector<Schedule> schedules(config.aco_n_ants);
    for (int64_t i = 0; i < config.aco_n_ants; ++i) {
        schedules[i] = RunAnt();
    }
    auto iter_best_schedule = SelectBestSchedule(schedules);
    ApplyLocalSearch(*iter_best_schedule, max_time);
    if (best_schedule_.GetQuality() > iter_best_schedule->GetQuality()) {
        best_schedule_ = std::move(*iter_best_schedule);
    }
    UpdateTrail();
}

void Colony::MakeNIterations(int64_t n) {
    while (n--) {
        MakeIteration();
    }
}

void Colony::Run(Timer::fsec max_time) {
    Timer timer(max_time);
    while (!timer.Expired()) {
        MakeIteration(timer.TimeLeft());
    }
}

Schedule& Colony::GetSchedule() {
    return best_schedule_;
}

double Colony::GetQuality() const {
    return best_schedule_.GetQuality();
}

Matrix3D<double> Colony::GetTrail() const {
    return trail_;
}

void Colony::SetTrail(Matrix3D<double> trail) {
    trail_ = std::move(trail);
}

void Colony::InitTrail() {
    trail_ = myaco::CreateMatrix3D(data.n_teachers, data.n_students, data.n_slots, config.initial_trail);
}

Schedule Colony::RunAnt() {
    Schedule::Table table = Schedule::CreateTable(data.n_teachers, data.n_slots);
    for (int64_t teacher_id = 0; teacher_id < data.n_teachers; ++teacher_id) {
        for (int64_t student_id = 0; student_id < data.n_students; ++student_id) {
            for (int64_t i = 0; i < data.requirements[teacher_id][student_id]; ++i) {
                int64_t timeslot_id = SelectTimeslot(teacher_id, student_id, table[teacher_id]);
                table[teacher_id][timeslot_id] = student_id;
                ApplyLocalTrailDecay(teacher_id, student_id, timeslot_id);
            }
        }
    }
    return Schedule(table);
}

int64_t Colony::SelectTimeslot(int64_t teacher_id, int64_t student_id, const Schedule::Row& current_schedule) {
    std::vector<double> partial_sum_probas(data.n_slots, 0);
    for (int64_t slot_id = 0; slot_id < data.n_slots; ++slot_id) {
        if (current_schedule[slot_id].has_value()) {
            continue;
        }
        partial_sum_probas[slot_id] =
                pow(trail_[teacher_id][student_id][slot_id], config.trail_weight) *
                visibility_[teacher_id][student_id][slot_id]; // visibilities are already exponentiated
        if (slot_id > 0) {
            partial_sum_probas[slot_id] += partial_sum_probas[slot_id - 1];
        }
    }
    double total_sum_probas = partial_sum_probas.back();
    std::uniform_real_distribution<double> unif(0., total_sum_probas);
    return std::lower_bound(partial_sum_probas.begin(), partial_sum_probas.end(), unif(rng_)) - partial_sum_probas.begin();
}

void Colony::ApplyLocalTrailDecay(int64_t teacher_id, int64_t student_id, int64_t slot_id) {
    trail_[teacher_id][student_id][slot_id] *= (1. - config.decay_rate);
    trail_[teacher_id][student_id][slot_id] += config.decay_rate * config.initial_trail;
}

IOptimizer* Colony::GetLocalSearcher(Schedule& schedule) {
    //return new Annealer(std::move(schedule));
    return nullptr;
}

void Colony::ApplyLocalSearch(Schedule& schedule, Timer::fsec max_time = Timer::kInfiniteSeconds) {
    IOptimizer* local_searcher = GetLocalSearcher(schedule);
    if (local_searcher != nullptr) {
        local_searcher->Run(max_time, std::chrono::seconds(1));
        schedule = std::move(local_searcher->GetSchedule());
        delete local_searcher;
    }
}

void Colony::UpdateTrail() {
    for (int64_t teacher_id = 0; teacher_id < data.n_teachers; ++teacher_id) {
        for (int64_t student_id = 0; student_id < data.n_students; ++student_id) {
            for (int64_t slot_id = 0; slot_id < data.n_slots; ++slot_id) {
                trail_[teacher_id][student_id][slot_id] *= (1. - config.decay_rate);
                if (best_schedule_[teacher_id][slot_id] == student_id) {
                    trail_[teacher_id][student_id][slot_id] += config.decay_rate * config.trail_store_factor
                                                            / (1 + best_schedule_.GetQuality());
                }
            }
        }
    }
}

std::vector<Schedule>::iterator Colony::SelectBestSchedule(std::vector<Schedule>& schedules) {
    std::vector<double> qualities;
    qualities.reserve(schedules.size());
    for (const Schedule& schedule : schedules) {
        qualities.push_back(CalcScheduleQuality(schedule));
    }
    auto best_id = std::min_element(qualities.begin(), qualities.end()) - qualities.begin();
    return schedules.begin() + best_id;
}

double Colony::CalcScheduleQuality(const Schedule& schedule) {
    return QualityEstimator::Estimate(schedule);
}

