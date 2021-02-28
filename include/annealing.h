#ifndef MYACO_ANNEALING_H
#define MYACO_ANNEALING_H

#include "local_search.h"

class Annealer : public LocalSearcher {
public:
    explicit Annealer(Schedule initial_schedule);

    void Run() override;

private:
    bool AcceptMove(double from, double to);

    std::uniform_real_distribution<double> threshold_gen_{0, 1};

    double temperature_;
    double best_quality_;
    // best_schedule_ -- inherited
};

#endif //MYACO_ANNEALING_H
