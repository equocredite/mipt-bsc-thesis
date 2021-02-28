#include "local_search.h"

#include <chrono>
#include <random>

using namespace myaco;

LocalSearcher::LocalSearcher(Schedule&& schedule)
        : best_schedule_(std::move(schedule))
        , rng_(std::chrono::system_clock::now().time_since_epoch().count())
        , teacher_id_gen_(0, data.n_teachers - 1)
        , slot_id_gen_(0, data.week_length - 1) {
}

LocalSearcher::Move LocalSearcher::GenerateMove(const Schedule& schedule) {
    int64_t teacher_id, slot_x, slot_y;
    do {
        teacher_id = teacher_id_gen_(rng_);
        slot_x = slot_id_gen_(rng_);
        slot_y = slot_id_gen_(rng_);
    } while (schedule[teacher_id][slot_x] == schedule[teacher_id][slot_y]);
    return {teacher_id, slot_x, slot_y};
}

void LocalSearcher::ApplyMove(Schedule& schedule, Move move) {
    std::swap(schedule[move.teacher_id][move.slot_x], schedule[move.teacher_id][move.slot_y]);
}

std::pair<Schedule, LocalSearcher::Move> LocalSearcher::GenerateNeighbor(Schedule schedule) {
    Move move = GenerateMove(schedule);
    ApplyMove(schedule, move);
    return {schedule, move};
}

Schedule LocalSearcher::GetSchedule() {
    return best_schedule_;
}

