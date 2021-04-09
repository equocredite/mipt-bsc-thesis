#ifndef MYACO_HILL_CLIMBING_H
#define MYACO_HILL_CLIMBING_H

#include "local_search.h"

class HillClimber : public LocalSearcher {
public:
    explicit HillClimber(Schedule&& initial_schedule, IPerturbator* perturbator);

    void Run(Timer::fsec max_time, Timer::fsec log_frequency) override;
};

#endif //MYACO_HILL_CLIMBING_H
