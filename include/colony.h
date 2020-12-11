#ifndef MYACO_COLONY_H
#define MYACO_COLONY_H

#include "config.h"
#include "problem_data.h"
#include <random>

class Colony {
public:
    friend class AntOptimizer;

    Colony(const ProblemData* data, Config config);

    void MakeIteration();

    Schedule GetSchedule() const;

    double GetQuality() const;

    Matrix3D<double> GetTrail() const;

    void SetTrail(Matrix3D<double>);

private:
    Matrix3D<double> CalcVisibility();

    void InitTrail();

    Schedule RunAnt();

    int64_t SelectTimeslot(int64_t teacher_id, int64_t student_id);

    void ApplyLocalTrailDecay(int64_t teacher_id, int64_t student_id, int64_t slot_id);

    void ApplyLocalSearch(Schedule& schedule);

    void UpdateTrail();

    std::pair<double, std::vector<Schedule>::const_iterator>  SelectBestSchedule(const std::vector<Schedule>& schedules);

    size_t CountHoles(const Schedule& schedule) const;

    size_t CountInconvenientAssignments(const Schedule& schedule) const;

    double CalcTargetFunction(const Schedule& schedule) const;

    size_t CountSimultaneousAssignmentsForStudents(const Schedule& schedule) const;

    size_t CountRequirementViolations(const Schedule& schedule) const;

    double CalcConstraintViolationPenalty(const Schedule& schedule) const;

    double CalcScheduleQuality(const Schedule& schedule) const;

    const ProblemData* data_;
    const Config config_;

    Matrix3D<double> trail_;
    const Matrix3D<double> visibility_;
    std::default_random_engine random_engine_;

    Schedule best_schedule_{};
    double best_quality_{};
};

#endif //MYACO_COLONY_H
