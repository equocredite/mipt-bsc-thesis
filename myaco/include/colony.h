#ifndef MYACO_COLONY_H
#define MYACO_COLONY_H

#include "config.h"
#include "problem_data.h"
#include "optimizer.h"

#include <random>

class Colony {
public:
    Colony() = delete;

    explicit Colony(Schedule initial_schedule, const Matrix3D<double>& visibility);

    void MakeIteration(Timer::fsec max_time);

    void Run(Timer::fsec max_time);

    void MakeNIterations(int64_t n);

    Schedule& GetSchedule();

    double GetQuality() const;

    Matrix3D<double> GetTrail() const;

    void SetTrail(Matrix3D<double>);

private:
    void InitTrail();

    Schedule RunAnt();

    int64_t SelectTimeslot(int64_t teacher_id, int64_t student_id, const Schedule::Row& current_schedule);

    void ApplyLocalTrailDecay(int64_t teacher_id, int64_t student_id, int64_t slot_id);

    static IOptimizer* GetLocalSearcher(Schedule& schedule);

    static void ApplyLocalSearch(Schedule& schedule, Timer::fsec max_time);

    void UpdateTrail();

    static std::vector<Schedule>::iterator SelectBestSchedule(std::vector<Schedule>& schedules);

    static double CalcScheduleQuality(const Schedule& schedule);

    Matrix3D<double> trail_;
    const Matrix3D<double>& visibility_;
    std::mt19937 rng_;

    Schedule best_schedule_;
};

#endif //MYACO_COLONY_H
