#ifndef MYACO_COLONY_H
#define MYACO_COLONY_H

#include "config.h"
#include "problem_data.h"

class Colony {
public:
    Colony(const ProblemData* data, Config config);

    void MakeIteration();

    Schedule GetSchedule() const;

    double GetQuality() const;

    Matrix3D<double> GetTrail() const;

    void SetTrail(Matrix3D<double>);

private:
    void InitTrails();

    void InitVisibilities();

    Schedule RunAnt();

    int64_t SelectTimeslot(int64_t teacher_id, int64_t student_id);

    void ApplyLocalTrailDecay(int64_t teacher_id, int64_t student_id, int64_t timeslot_id);

    void ApplyLocalSearch(Schedule& schedule);

    void UpdateTrail();

    std::pair<double, std::vector<Schedule>::const_iterator>  SelectBestSchedule(const std::vector<Schedule>& schedules);

    double CalculateScheduleQuality(const Schedule& schedule) const;

    const ProblemData* data_;
    Config config_;

    Matrix3D<double> trail_;
    Matrix3D<double> visibility_;

    Schedule best_schedule_{};
    double best_quality_{};
};

#endif //MYACO_COLONY_H
