#ifndef MYACO_LATE_ACCEPTANCE_H
#define MYACO_LATE_ACCEPTANCE_H

#include "local_search.h"

// Late Acceptance Hill Climbing
class LAHillClimber : public LocalSearcher {
public:
    explicit LAHillClimber(Schedule&& initial_schedule, IPerturbator* perturbator = new SimplePerturbator());

    void Run(Timer::fsec max_time, Timer::fsec log_frequency) override;

protected:
    std::vector<double> qualities_;
    // best_schedule_ -- inherited
};

// Diversified Late Acceptance Search
class DLASearcher : public LAHillClimber {
public:
    explicit DLASearcher(Schedule&& initial_schedule, IPerturbator* perturbator = new SimplePerturbator());

    void Run(Timer::fsec max_time, Timer::fsec log_frequency) override;
};

#endif //MYACO_LATE_ACCEPTANCE_H
