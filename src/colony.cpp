#include "colony.h"
#include <algorithm>

Colony::Colony(const ProblemData* data, Config config)
        : data_(data)
        , config_(config) {
    InitTrails();
    InitVisibilities();
}

void Colony::MakeIteration() {
    std::vector<Schedule> schedules(config_.n_ants);
    for (int64_t i = 0; i < config_.n_ants; ++i) {
        schedules[i] = RunAnt();
        ApplyLocalSearch(schedules[i]);
    }
    UpdateTrail();
    auto [iter_best_quality, iter_best_schedule] = SelectBestSchedule(schedules);
    if (iter_best_quality < best_quality_) {
        best_schedule_ = *iter_best_schedule;
        best_quality_ = iter_best_quality;
    }
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

void Colony::InitTrails() {
    trail_ = myaco::CreateMatrix3D(data_->n_teachers, data_->n_students, data_->week_length, config_.initial_trail);
}

void Colony::InitVisibilities() {
    visibility_ = myaco::CreateMatrix3D<double>(data_->n_teachers, data_->n_students, data_->week_length);
    for (int64_t i = 0; i < data_->n_teachers; ++i) {
        for (int64_t j = 0; j < data_->n_students; ++j) {
            for (int64_t k = 0; k < data_->week_length; ++k) {
                visibility_[i][j][k] = static_cast<double>(data_->available[i][k])
                                     + static_cast<double>(data_->convenient[i][k]) / 10.
                                     + static_cast<double>(data_->requirements[i][j]) / data_->week_length;
            }
        }
    }
}

Schedule Colony::RunAnt() {
    Schedule schedule = myaco::CreateSchedule(data_->n_teachers, data_->week_length);
    for (int64_t teacher_id = 0; teacher_id < data_->n_teachers; ++teacher_id) {
        for (int64_t student_id = 0; student_id < data_->n_students; ++student_id) {
            for (int64_t i = 0; i < data_->requirements[teacher_id][student_id]; ++i) {
                int64_t timeslot_id = SelectTimeslot(teacher_id, student_id);
                schedule[teacher_id][timeslot_id] = student_id;
                ApplyLocalTrailDecay(teacher_id, student_id, timeslot_id);
            }
        }
    }
    return schedule;
}

int64_t Colony::SelectTimeslot(int64_t teacher_id, int64_t student_id) {
    // TODO
    return 0;
}

void Colony::ApplyLocalTrailDecay(int64_t teacher_id, int64_t student_id, int64_t timeslot_id) {
    trail_[teacher_id][student_id][timeslot_id] = (1. - config_.local_decay_rate) * trail_[teacher_id][student_id][timeslot_id]
        - config_.local_decay_rate * config_.initial_trail;
}

void Colony::ApplyLocalSearch(Schedule& schedule) {
    // TODO
}

void Colony::UpdateTrail() {
    // TODO
}

std::pair<double, std::vector<Schedule>::const_iterator> Colony::SelectBestSchedule(const std::vector<Schedule>& schedules) {
    std::vector<double> qualities;
    for (const Schedule& schedule : schedules) {
        qualities.push_back(CalculateScheduleQuality(schedule));
    }
    auto best_id = std::min_element(qualities.begin(), qualities.end()) - qualities.begin();
    return {qualities[best_id], schedules.begin() + best_id};
}

double Colony::CalculateScheduleQuality(const Schedule& schedule) const {
    // TODO
    return 0;
}
