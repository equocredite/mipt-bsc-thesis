#ifndef MYACO_TABU_SEARCH_H
#define MYACO_TABU_SEARCH_H

#include "local_search.h"
#include <deque>

class TabuSearcher : public LocalSearcher {
public:
    explicit TabuSearcher(Schedule initial_schedule);

    void Run() override;

private:
    bool IsTabu(Move move) const;

    void AddTabu(Move move);

    std::deque<Move> tabus;
    double best_quality_;
};

#endif //MYACO_TABU_SEARCH_H
