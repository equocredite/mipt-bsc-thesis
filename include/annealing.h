#ifndef MYACO_ANNEALING_H
#define MYACO_ANNEALING_H

#include "local_searcher.h"

class Annealer : public LocalSearcher {
public:
    explicit Annealer(Schedule initial_schedule);

    void Run() override;

private:
    double temperature_;

    // best_schedule_ -- inherited
    double best_quality_;
};

#endif //MYACO_ANNEALING_H
