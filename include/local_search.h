#ifndef MYACO_LOCAL_SEARCH_H
#define MYACO_LOCAL_SEARCH_H

#include <random>
#include "optimizer.h"

class LocalSearcher : public IOptimizer {
public:
    explicit LocalSearcher(Schedule&& schedule);

    Schedule GetSchedule() override;

protected:
    struct Move {
        int64_t teacher_id;
        int64_t slot_x;
        int64_t slot_y;

        bool operator == (const Move& other) const {
            return teacher_id == other.teacher_id && std::minmax(slot_x, slot_y) == std::minmax(other.slot_x, other.slot_y);
        }
    };

    virtual Move GenerateMove(const Schedule& schedule);

    virtual void ApplyMove(Schedule& schedule, Move move);

    virtual std::pair<Schedule, Move> GenerateNeighbor(Schedule schedule);

    std::mt19937 rng_;
    std::uniform_int_distribution<int64_t> teacher_id_gen_, slot_id_gen_;

    Schedule best_schedule_;
};


#endif //MYACO_LOCAL_SEARCH_H
