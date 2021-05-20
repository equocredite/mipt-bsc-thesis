#ifndef MYACO_OPTIMIZER_H
#define MYACO_OPTIMIZER_H

#include "schedule.h"
#include "problem_data.h"
#include "time_util.h"
#include <chrono>

class IOptimizer {
public:
    virtual void Run(Timer::fsec max_time, Timer::fsec log_frequency) = 0;

    virtual Schedule& GetSchedule() = 0;

    virtual ~IOptimizer() = default;

    virtual std::vector<std::pair<Timer::fsec, double>> GetHistory() const {
        return history_;
    }

protected:
    std::vector<std::pair<Timer::fsec, double>> history_;
};


#endif //MYACO_OPTIMIZER_H
