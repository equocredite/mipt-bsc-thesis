#ifndef MYACO_COLONY_H
#define MYACO_COLONY_H

#include "config.h"
#include "problem_data.h"
#include "optimizer.h"

#include <random>

class Colony {
public:
    explicit Colony(Schedule initial_schedule);

    void MakeIteration();

    void MakeNIterations(int64_t n);

    Schedule GetSchedule() const;

    double GetQuality() const;

    Matrix3D<double> GetTrail() const;

    void SetTrail(Matrix3D<double>);

private:
    static Matrix3D<double> CalcVisibility();

    void InitTrail();

    Schedule RunAnt();

    int64_t SelectTimeslot(int64_t teacher_id, int64_t student_id);

    void ApplyLocalTrailDecay(int64_t teacher_id, int64_t student_id, int64_t slot_id);

    static IOptimizer* GetLocalSearcher(const Schedule& schedule);

    static void ApplyLocalSearch(Schedule& schedule);

    void UpdateTrail();

    static std::vector<Schedule>::const_iterator SelectBestSchedule(const std::vector<Schedule>& schedules);

    static double CalcScheduleQuality(const Schedule& schedule);

    Matrix3D<double> trail_;
    const Matrix3D<double> visibility_;
    std::mt19937 rng_;

    Schedule best_schedule_{};
    double best_quality_{};
};

#endif //MYACO_COLONY_H
