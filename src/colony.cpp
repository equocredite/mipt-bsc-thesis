#include "colony.h"
#include "quality.h"

#include <algorithm>
#include <cmath>
#include <chrono>

using namespace myaco;

Colony::Colony(Schedule initial_schedule)
        : best_schedule_(std::move(initial_schedule))
        , visibility_(CalcVisibility()) {
    InitTrail();
    rng_.seed(std::chrono::system_clock::now().time_since_epoch().count());
}

void Colony::MakeIteration() {
    std::vector<Schedule> schedules(config.n_ants);
    for (int64_t i = 0; i < config.n_ants; ++i) {
        schedules[i] = RunAnt();
    }

    auto iter_best_schedule = SelectBestSchedule(schedules);
    ApplyLocalSearch(const_cast<Schedule&>(*iter_best_schedule));
    double iter_best_quality = CalcScheduleQuality(*iter_best_schedule);

    if (best_schedule_.empty() || iter_best_quality < best_quality_) {
        best_schedule_ = *iter_best_schedule;
        best_quality_ = iter_best_quality;
    }
    UpdateTrail();
}

Schedule Colony::GetSchedule() const {
    return best_schedule_;
}

double Colony::GetQuality() const {
    return best_quality_;
}

Matrix3D<double> Colony::GetTrail() const {
    return trail_;
}

void Colony::SetTrail(Matrix3D<double> trail) {
    trail_ = std::move(trail);
}

void Colony::InitTrail() {
    trail_ = myaco::CreateMatrix3D(data.n_teachers, data.n_students, data.week_length, config.initial_trail);
}

Matrix3D<double> Colony::CalcVisibility() {
    auto visibility = myaco::CreateMatrix3D<double>(data.n_teachers, data.n_students, data.week_length);
    for (int64_t teacher_id = 0; teacher_id < data.n_teachers; ++teacher_id) {
        for (int64_t student_id = 0; student_id < data.n_students; ++student_id) {
            for (int64_t slot_id = 0; slot_id < data.week_length; ++slot_id) {
                visibility[teacher_id][student_id][slot_id] = static_cast<double>(data.available[teacher_id][slot_id])
                                                            + static_cast<double>(data.convenient[teacher_id][slot_id]) / 10.
                                                            + static_cast<double>(data.requirements[teacher_id][student_id]) / data.week_length;
                // visibilities do not change, so we can exponentiate it right away
                visibility[teacher_id][student_id][slot_id] = pow(visibility[teacher_id][student_id][slot_id], config.visibility_weight);
            }
        }
    }
    return visibility;
}

Schedule Colony::RunAnt() {
    Schedule schedule = myaco::CreateSchedule(data.n_teachers, data.week_length);
    for (int64_t teacher_id = 0; teacher_id < data.n_teachers; ++teacher_id) {
        for (int64_t student_id = 0; student_id < data.n_students; ++student_id) {
            for (int64_t i = 0; i < data.requirements[teacher_id][student_id]; ++i) {
                int64_t timeslot_id = SelectTimeslot(teacher_id, student_id);
                schedule[teacher_id][timeslot_id] = student_id;
                ApplyLocalTrailDecay(teacher_id, student_id, timeslot_id);
            }
        }
    }
    return schedule;
}

int64_t Colony::SelectTimeslot(int64_t teacher_id, int64_t student_id) {
    std::vector<double> partial_sum_probas(data.week_length);
    for (int64_t slot_id = 0; slot_id < data.week_length; ++slot_id) {
        if (slot_id > 0) {
            partial_sum_probas[slot_id] += partial_sum_probas[slot_id - 1];
        }
        partial_sum_probas[slot_id] += pow(trail_[teacher_id][student_id][slot_id], config.trail_weight);
        // visibilities are already exponentiated
        partial_sum_probas[slot_id] += visibility_[teacher_id][student_id][slot_id];
    }
    double total_sum_probas = partial_sum_probas.back();
    std::uniform_real_distribution<double> unif(0., total_sum_probas);
    return std::lower_bound(partial_sum_probas.begin(), partial_sum_probas.end(), unif(rng_)) - partial_sum_probas.begin();
}

void Colony::ApplyLocalTrailDecay(int64_t teacher_id, int64_t student_id, int64_t slot_id) {
    trail_[teacher_id][student_id][slot_id] *= (1. - config.local_decay_rate);
    trail_[teacher_id][student_id][slot_id] += config.local_decay_rate * config.initial_trail;
}

IOptimizer* Colony::GetLocalSearcher(const Schedule& schedule) {
    // TODO
    return nullptr;
}

void Colony::ApplyLocalSearch(Schedule& schedule) {
    IOptimizer* local_searcher = GetLocalSearcher(schedule);
    if (local_searcher != nullptr) {
        local_searcher->Run();
        schedule = local_searcher->GetSchedule();
    }
}

void Colony::UpdateTrail() {
    for (int64_t teacher_id = 0; teacher_id < data.n_teachers; ++teacher_id) {
        for (int64_t student_id = 0; student_id < data.n_students; ++student_id) {
            for (int64_t slot_id = 0; slot_id < data.week_length; ++slot_id) {
                trail_[teacher_id][student_id][slot_id] *= (1. - config.evaporation_rate);
                if (best_schedule_[teacher_id][slot_id] == student_id) {
                    trail_[teacher_id][student_id][slot_id] += config.evaporation_rate
                                                            * config.trail_store_factor
                                                            / (1 + best_quality_);
                }
            }
        }
    }
}

std::vector<Schedule>::const_iterator Colony::SelectBestSchedule(const std::vector<Schedule>& schedules) {
    std::vector<double> qualities;
    for (const Schedule& schedule : schedules) {
        qualities.push_back(CalcScheduleQuality(schedule));
    }
    auto best_id = std::min_element(qualities.begin(), qualities.end()) - qualities.begin();
    return schedules.begin() + best_id;
}

double Colony::CalcScheduleQuality(const Schedule& schedule) {
    return QualityEstimator::Estimate(schedule);
}
