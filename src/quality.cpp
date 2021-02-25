#include "quality.h"
#include "config.h"

using namespace myaco;

double QualityEstimator::Estimate(const Schedule& schedule) {
    return // hard
           config.simultaneous_assignments_penalty * CountSimultaneousAssignmentsForStudents(schedule) +
           config.requirement_violation_penalty * CountUnsatisfiedRequirements(schedule) +
           // soft
           config.holes_weight * CountHoles(schedule) +
           config.inconvenience_weight * CountInconvenientAssignments(schedule);
}

int64_t QualityEstimator::CountSimultaneousAssignmentsForStudents(const Schedule& schedule) {
    int64_t n_simult_assignments = 0;
    for (int64_t teacher_id_x = 0; teacher_id_x < data.n_teachers; ++teacher_id_x) {
        for (int64_t teacher_id_y = 0; teacher_id_y < data.n_teachers; ++teacher_id_y) {
            for (int64_t slot_id = 0; slot_id < data.week_length; ++slot_id) {
                if (schedule[teacher_id_x][slot_id].has_value() &&
                    schedule[teacher_id_x][slot_id] == schedule[teacher_id_y][slot_id]) {
                    ++n_simult_assignments;
                }
            }
        }
    }
    return n_simult_assignments;
}

int64_t QualityEstimator::CountUnsatisfiedRequirements(const Schedule& schedule) {
    int64_t n_violations = 0;
    auto assignments = myaco::CreateMatrix2D<int64_t>(data.n_teachers, data.n_students);
    for (int64_t teacher_id = 0; teacher_id < data.n_teachers; ++teacher_id) {
        for (int64_t slot_id = 0; slot_id < data.week_length; ++slot_id) {
            auto assigned_class = schedule[teacher_id][slot_id];
            if (assigned_class.has_value()) {
                ++assignments[teacher_id][assigned_class.value()];
            }
        }
    }
    for (int64_t teacher_id = 0; teacher_id < data.n_teachers; ++teacher_id) {
        for (int64_t student_id = 0; student_id < data.n_students; ++student_id) {
            n_violations += std::abs(assignments[teacher_id][student_id] - data.requirements[teacher_id][student_id]);
        }
    }
    return n_violations;
}

int64_t QualityEstimator::CountHoles(const Schedule& schedule) {
    int64_t n_holes = 0;
    for (int64_t teacher_id = 0; teacher_id < data.n_teachers; ++teacher_id) {
        for (int64_t day_id = 0; day_id < data.GetNDays(); ++day_id) {
            for (int64_t hour_id = 0; hour_id < data.day_length; ++hour_id) {
                if (schedule[teacher_id][data.GetSlotId(day_id, hour_id)].has_value()) {
                    continue;
                }
                bool has_assignments_earlier = std::any_of(schedule[teacher_id].begin() + data.GetDayBegin(day_id),
                                                           schedule[teacher_id].begin() + data.GetSlotId(day_id, hour_id),
                                                           [](const auto assignment) { return assignment.has_value(); });
                bool has_assignments_later = std::any_of(schedule[teacher_id].begin() + data.GetSlotId(day_id, hour_id),
                                                         schedule[teacher_id].begin() + data.GetDayEnd(day_id),
                                                         [](const auto assignment) { return assignment.has_value(); });
                if (has_assignments_earlier && has_assignments_later) {
                    ++n_holes;
                }
            }
        }
    }
    return n_holes;
}

int64_t QualityEstimator::CountInconvenientAssignments(const Schedule& schedule) {
    int64_t n_inconvenient_assignments = 0;
    for (int64_t teacher_id = 0; teacher_id < data.n_teachers; ++teacher_id) {
        for (int64_t slot_id = 0; slot_id < data.week_length; ++slot_id) {
            if (schedule[teacher_id][slot_id].has_value() && !data.convenient[teacher_id][slot_id]) {
                ++n_inconvenient_assignments;
            }
        }
    }
    return n_inconvenient_assignments;
}
