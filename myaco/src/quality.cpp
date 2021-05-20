#include "quality.h"
#include "config.h"

using namespace myaco;

double QualityEstimator::Estimate(const Schedule& schedule) {
    return schedule.GetQuality();
    //return static_cast<double>(CountStudentAvailabilityViolations(schedule)); // / data.n_students / data.n_slots;
}

int64_t QualityEstimator::CountStudentAvailabilityViolations(const Schedule& schedule) {
    auto student_assignments = CreateMatrix2D<int64_t>(data.n_students, data.n_slots);
    for (int64_t teacher_id = 0; teacher_id < data.n_teachers; ++teacher_id) {
        for (int64_t slot_id = 0; slot_id < data.n_slots; ++slot_id) {
            if (schedule[teacher_id][slot_id].has_value()) {
                ++student_assignments[schedule[teacher_id][slot_id].value()][slot_id];
            }
        }
    }
    int64_t n_violations = 0;
    for (int64_t student_id = 0; student_id < data.n_students; ++student_id) {
        for (int64_t slot_id = 0; slot_id < data.n_slots; ++slot_id) {
            n_violations += std::max<int64_t>(0, student_assignments[student_id][slot_id] -
                                                 data.student_available[student_id][slot_id]);
        }
    }
    return n_violations;
}