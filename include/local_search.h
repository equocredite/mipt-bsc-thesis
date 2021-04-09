#ifndef MYACO_LOCAL_SEARCH_H
#define MYACO_LOCAL_SEARCH_H

#include <random>
#include "optimizer.h"
#include "perturbation.h"

class LocalSearcher : public IOptimizer {
public:
    LocalSearcher() = delete;

    explicit LocalSearcher(Schedule&& schedule, IPerturbator* perturbator);

    Schedule& GetSchedule() override;

protected:
    void Log(Timer& timer);

    IPerturbator* perturbator_;

    Schedule best_schedule_;

    std::mt19937 rng_;
};


#endif //MYACO_LOCAL_SEARCH_H
