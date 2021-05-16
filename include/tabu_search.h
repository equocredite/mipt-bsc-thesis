#ifndef MYACO_TABU_SEARCH_H
#define MYACO_TABU_SEARCH_H

#include "local_search.h"
#include <set>

class TabuSearcher : public LocalSearcher {
public:
    explicit TabuSearcher(Schedule&& initial_schedule, IPerturbator* perturbator = new SimplePerturbator());

    void Run(Timer::fsec max_time, Timer::fsec log_frequency) override;

private:
    struct TabuEntry {
        Schedule::Move move;
        mutable int64_t tenure;

        bool operator < (const TabuEntry& other) const {
            return move < other.move;
        }
    };

    bool Tabu(const std::vector<Schedule::Move>& moves) const;

    void AddTabus(const std::vector<Schedule::Move>& moves);

    std::set<TabuEntry> tabus_;
    // best_schedule_ -- inherited
};

#endif //MYACO_TABU_SEARCH_H
