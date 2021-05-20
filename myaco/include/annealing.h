#ifndef MYACO_ANNEALING_H
#define MYACO_ANNEALING_H

#include "local_search.h"

class Annealer : public LocalSearcher {
public:
    explicit Annealer(Schedule&& initial_schedule, IPerturbator* perturbator = new SimplePerturbator());

    void Run(Timer::fsec max_time, Timer::fsec log_frequency) override;

private:
    bool AcceptableMove(double delta);

    std::uniform_real_distribution<double> threshold_gen_{0, 1};

    double temperature_;
    // best_schedule_ -- inherited
};

#endif //MYACO_ANNEALING_H
