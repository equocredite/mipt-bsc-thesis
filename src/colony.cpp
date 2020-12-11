#include "colony.h"
#include <algorithm>
#include <cmath>
#include <chrono>

Colony::Colony(const ProblemData* data, Config config)
        : data_(data)
        , config_(config)
        , visibility_(CalcVisibility()) {
    InitTrail();
    random_engine_.seed(std::chrono::system_clock::now().time_since_epoch().count());
}

void Colony::MakeIteration() {
    std::vector<Schedule> schedules(config_.n_ants);
    for (int64_t i = 0; i < config_.n_ants; ++i) {
        schedules[i] = RunAnt();
        ApplyLocalSearch(schedules[i]);
    }
    auto [iter_best_quality, iter_best_schedule] = SelectBestSchedule(schedules);
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
    trail_ = myaco::CreateMatrix3D(data_->n_teachers, data_->n_students, data_->week_length, config_.initial_trail);
}

Matrix3D<double> Colony::CalcVisibility() {
    auto visibility = myaco::CreateMatrix3D<double>(data_->n_teachers, data_->n_students, data_->week_length);
    for (int64_t teacher_id = 0; teacher_id < data_->n_teachers; ++teacher_id) {
        for (int64_t student_id = 0; student_id < data_->n_students; ++student_id) {
            for (int64_t slot_id = 0; slot_id < data_->week_length; ++slot_id) {
                visibility[teacher_id][student_id][slot_id] = static_cast<double>(data_->available[teacher_id][slot_id])
                                                            + static_cast<double>(data_->convenient[teacher_id][slot_id]) / 10.
                                                            + static_cast<double>(data_->requirements[teacher_id][student_id]) / data_->week_length;
                // visibilities do not change, so we can exponentiate it right away
                visibility[teacher_id][student_id][slot_id] = pow(visibility[teacher_id][student_id][slot_id], config_.visibility_weight);
            }
        }
    }
    return visibility;
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
    std::vector<double> partial_sum_probas(data_->week_length);
    for (int64_t slot_id = 0; slot_id < data_->week_length; ++slot_id) {
        if (slot_id > 0) {
            partial_sum_probas[slot_id] += partial_sum_probas[slot_id - 1];
        }
        partial_sum_probas[slot_id] += pow(trail_[teacher_id][student_id][slot_id], config_.trail_weight);
        // visibilities are already exponentiated
        partial_sum_probas[slot_id] += visibility_[teacher_id][student_id][slot_id];
    }
    double total_sum_probas = partial_sum_probas.back();
    std::uniform_real_distribution<double> unif(0., total_sum_probas);
    return std::lower_bound(partial_sum_probas.begin(), partial_sum_probas.end(), unif(random_engine_)) - partial_sum_probas.begin();
}

void Colony::ApplyLocalTrailDecay(int64_t teacher_id, int64_t student_id, int64_t slot_id) {
    trail_[teacher_id][student_id][slot_id] *= (1. - config_.local_decay_rate);
    trail_[teacher_id][student_id][slot_id] += config_.local_decay_rate * config_.initial_trail;
}

void Colony::ApplyLocalSearch(Schedule& schedule) {
    // TODO
}

void Colony::UpdateTrail() {
    for (int64_t teacher_id = 0; teacher_id < data_->n_teachers; ++teacher_id) {
        for (int64_t student_id = 0; student_id < data_->n_students; ++student_id) {
            for (int64_t slot_id = 0; slot_id < data_->week_length; ++slot_id) {
                trail_[teacher_id][student_id][slot_id] *= (1. - config_.evaporation_rate);
                if (best_schedule_[teacher_id][slot_id] == student_id) {
                    trail_[teacher_id][student_id][slot_id] += config_.evaporation_rate
                                                            * config_.trail_store_factor
                                                            / (1 + best_quality_);
                }
            }
        }
    }
}

std::pair<double, std::vector<Schedule>::const_iterator> Colony::SelectBestSchedule(const std::vector<Schedule>& schedules) {
    std::vector<double> qualities;
    for (const Schedule& schedule : schedules) {
        qualities.push_back(CalcScheduleQuality(schedule));
    }
    auto best_id = std::min_element(qualities.begin(), qualities.end()) - qualities.begin();
    return {qualities[best_id], schedules.begin() + best_id};
}

size_t Colony::CountHoles(const Schedule& schedule) const {
    size_t n_holes = 0;
    for (int64_t teacher_id = 0; teacher_id < data_->n_teachers; ++teacher_id) {
        for (int64_t day_id = 0; day_id < data_->GetNDays(); ++day_id) {
            for (int64_t hour_id = 0; hour_id < data_->day_length; ++hour_id) {
                if (schedule[teacher_id][data_->GetSlotId(day_id, hour_id)].has_value()) {
                    continue;
                }
                bool has_assignments_earlier = std::any_of(schedule[teacher_id].begin() + data_->GetDayBegin(day_id),
                                                           schedule[teacher_id].begin() + data_->GetSlotId(day_id, hour_id),
                                                           [](const auto assignment) { return assignment.has_value(); });
                bool has_assignments_later = std::any_of(schedule[teacher_id].begin() + data_->GetSlotId(day_id, hour_id),
                                                         schedule[teacher_id].begin() + data_->GetDayEnd(day_id),
                                                         [](const auto assignment) { return assignment.has_value(); });
                if (has_assignments_earlier && has_assignments_later) {
                    ++n_holes;
                }
            }
        }
    }
    return n_holes;
}

size_t Colony::CountInconvenientAssignments(const Schedule& schedule) const {
    size_t n_inconvenient_assignments = 0;
    for (int64_t teacher_id = 0; teacher_id < data_->n_teachers; ++teacher_id) {
        for (int64_t slot_id = 0; slot_id < data_->week_length; ++slot_id) {
            if (schedule[teacher_id][slot_id].has_value() && !data_->convenient[teacher_id][slot_id]) {
                ++n_inconvenient_assignments;
            }
        }
    }
    return n_inconvenient_assignments;
}


double Colony::CalcTargetFunction(const Schedule& schedule) const {
    return config_.holes_weight * CountHoles(schedule) + config_.inconvenience_weight * CountInconvenientAssignments(schedule);
}

size_t Colony::CountSimultaneousAssignmentsForStudents(const Schedule& schedule) const {
    size_t n_simult_assignments = 0;
    for (int64_t teacher_id_x = 0; teacher_id_x < data_->n_teachers; ++teacher_id_x) {
        for (int64_t teacher_id_y = 0; teacher_id_y < data_->n_teachers; ++teacher_id_y) {
            for (int64_t slot_id = 0; slot_id < data_->week_length; ++slot_id) {
                if (schedule[teacher_id_x][slot_id].has_value() &&
                    schedule[teacher_id_x][slot_id] == schedule[teacher_id_y][slot_id]) {
                    ++n_simult_assignments;
                }
            }
        }
    }
    return n_simult_assignments;
}

size_t Colony::CountRequirementViolations(const Schedule& schedule) const {
    size_t n_violations = 0;
    auto assignments = myaco::CreateMatrix2D<int64_t>(data_->n_teachers, data_->n_students);
    for (int64_t teacher_id = 0; teacher_id < data_->n_teachers; ++teacher_id) {
        for (int64_t slot_id = 0; slot_id < data_->week_length; ++slot_id) {
            auto assigned_class = schedule[teacher_id][slot_id];
            if (assigned_class.has_value()) {
                ++assignments[teacher_id][assigned_class.value()];
            }
        }
    }
    for (int64_t teacher_id = 0; teacher_id < data_->n_teachers; ++teacher_id) {
        for (int64_t student_id = 0; student_id < data_->n_students; ++student_id) {
            if (assignments[teacher_id][student_id] != data_->requirements[teacher_id][student_id]) {
                ++n_violations;
            }
        }
    }
    return n_violations;
}

double Colony::CalcConstraintViolationPenalty(const Schedule& schedule) const {
    return config_.simultaneous_assignments_penalty * CountSimultaneousAssignmentsForStudents(schedule) +
           config_.requirement_violation_penalty    * CountRequirementViolations(schedule);
}

double Colony::CalcScheduleQuality(const Schedule& schedule) const {
    return CalcTargetFunction(schedule) + CalcConstraintViolationPenalty(schedule);
}
