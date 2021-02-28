#ifndef MYACO_LOCAL_SEARCHER_H
#define MYACO_LOCAL_SEARCHER_H

#include <random>
#include "optimizer.h"

class LocalSearcher : public IOptimizer {
public:
    explicit LocalSearcher(Schedule&& schedule);

    Schedule GetSchedule() override;

protected:
    virtual Schedule GenerateNeighbor(Schedule schedule);

    std::mt19937 rng_;
    std::uniform_int_distribution<int64_t> teacher_id_gen_, slot_id_gen_;

    Schedule best_schedule_;
};


#endif //MYACO_LOCAL_SEARCHER_H
