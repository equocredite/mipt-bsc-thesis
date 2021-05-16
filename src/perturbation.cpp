#include "perturbation.h"
#include "problem_data.h"
#include <chrono>
#include <queue>

using namespace myaco;

IPerturbator::IPerturbator()
        : rng_(std::chrono::system_clock::now().time_since_epoch().count())
        , teacher_id_gen_(0, data.n_teachers - 1)
        , slot_id_gen_(0, data.n_slots - 1){

}

Schedule::Move IPerturbator::GenerateMove(const Schedule& schedule) {
    while (true) {
        int64_t teacher_id = teacher_id_gen_(rng_);
        int64_t slot_x = slot_id_gen_(rng_);
        int64_t slot_y = slot_id_gen_(rng_);
        if (slot_x != slot_y &&
            data.teacher_available[teacher_id][slot_x] &&
            data.teacher_available[teacher_id][slot_y] &&
            schedule[teacher_id][slot_x] != schedule[teacher_id][slot_y]) {
            return {teacher_id, slot_x, slot_y};
        }
    }
}

std::vector<Schedule::Move> SimplePerturbator::GenerateMoves(const Schedule& schedule) {
    return {GenerateMove(schedule)};
}

std::vector<Schedule::Move> Torquer::GenerateMoves(const Schedule& schedule) {
    auto basic_move = GenerateMove(schedule);
    int64_t slot_x = basic_move.slot_x;
    int64_t slot_y = basic_move.slot_y;

    // list of rows that have a specific student in either slot_x or slot_y
    auto adj_list = CreateMatrix3D<int64_t>(2, data.n_students, 0);
    for (int64_t i = 0; i < data.n_teachers; ++i) {
        if (schedule[i][slot_x].has_value()) {
            adj_list[0][schedule[i][slot_x].value()].push_back(i);
        }
        if (schedule[i][slot_y].has_value()) {
            adj_list[1][schedule[i][slot_y].value()].push_back(i);
        }
    }

    std::vector<Schedule::Move> moves;

    std::vector<char> visited(data.n_teachers, false);
    std::queue<int64_t> queue;
    queue.push(basic_move.teacher_id);
    visited[basic_move.teacher_id] = true;

    while (!queue.empty()) {
        int64_t teacher_id = queue.front();
        queue.pop();
        moves.push_back({teacher_id, slot_x, slot_y});

        if (schedule[teacher_id][slot_x].has_value()) {
            for (int64_t adj_teacher_id : adj_list[1][schedule[teacher_id][slot_x].value()]) {
                if (!visited[adj_teacher_id]) {
                    queue.push(adj_teacher_id);
                    visited[adj_teacher_id] = true;
                }
            }
        }

        if (schedule[teacher_id][slot_y].has_value()) {
            for (int64_t adj_teacher_id : adj_list[0][schedule[teacher_id][slot_y].value()]) {
                if (!visited[adj_teacher_id]) {
                    queue.push(adj_teacher_id);
                    visited[adj_teacher_id] = true;
                }
            }
        }
    }

    return moves;
}