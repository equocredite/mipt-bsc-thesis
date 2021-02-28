#include "local_searcher.h"

#include <chrono>
#include <random>

using namespace myaco;

LocalSearcher::LocalSearcher(Schedule&& schedule)
        : best_schedule_(std::move(schedule))
        , rng_(std::chrono::system_clock::now().time_since_epoch().count())
        , teacher_id_gen_(0, data.n_teachers - 1)
        , slot_id_gen_(0, data.week_length - 1) {
}

Schedule LocalSearcher::GenerateNeighbor(Schedule schedule) {
    int64_t teacher_id, slot_x, slot_y;
    do {
        teacher_id = teacher_id_gen_(rng_);
        slot_x = slot_id_gen_(rng_);
        slot_y = slot_id_gen_(rng_);
    } while (schedule[teacher_id][slot_x] == schedule[teacher_id][slot_y]);
    std::swap(schedule[teacher_id][slot_x], schedule[teacher_id][slot_y]);
    return schedule;
}

Schedule LocalSearcher::GetSchedule() {
    return best_schedule_;
}
