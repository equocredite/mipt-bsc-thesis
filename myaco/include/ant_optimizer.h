#ifndef MYACO_ANT_OPTIMIZER_H
#define MYACO_ANT_OPTIMIZER_H

#include "optimizer.h"
#include "colony.h"
#include <thread>

class AntOptimizer : public IOptimizer {
public:
    AntOptimizer();

    explicit AntOptimizer(const Schedule& initial_schedule);

    void Run(Timer::fsec max_time, Timer::fsec log_frequency) override;

    void Run(Timer::fsec max_time, Timer::fsec log_frequency, Timer::fsec sync_frequency);

    Schedule& GetSchedule() override;

    static Schedule CreateInitialSchedule();

private:
    static Matrix3D<double> CalcVisibility();

    Matrix3D<double> visibility;

    void SyncColonies();

    std::vector<Colony>::iterator GetBestColony();

    std::vector<Colony> colonies_;

    double best_quality_;
};

#endif //MYACO_ANT_OPTIMIZER_H
