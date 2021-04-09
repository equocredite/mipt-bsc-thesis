#include "perturbation.h"
#include "problem_data.h"
#include <chrono>

using namespace myaco;

IPerturbator::IPerturbator()
        : rng_(std::chrono::system_clock::now().time_since_epoch().count())
        , teacher_id_gen_(0, data.n_teachers - 1)
        , slot_id_gen_(0, data.n_slots - 1){

}

std::vector<Schedule::Move> SimplePerturbator::GenerateMoves(const Schedule& schedule) {
    while (true) {
        int64_t teacher_id = teacher_id_gen_(rng_);
        int64_t slot_x = slot_id_gen_(rng_);
        int64_t slot_y = slot_id_gen_(rng_);
        if (slot_x != slot_y &&
            data.teacher_available[teacher_id][slot_x] &&
            data.teacher_available[teacher_id][slot_y] &&
            schedule[teacher_id][slot_x] != schedule[teacher_id][slot_y]) {
            return {{teacher_id, slot_x, slot_y}};
        }
    }
}

std::vector<Schedule::Move> Torquer::GenerateMoves(const Schedule& schedule) {
    return {};
}
