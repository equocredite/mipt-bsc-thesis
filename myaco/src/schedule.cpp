#include "schedule.h"
#include "problem_data.h"

#include <utility>

using namespace myaco;

Schedule::Schedule(int64_t n, int64_t k)
        : table_(CreateMatrix2D<std::optional<int64_t>>(data.n_teachers, data.n_slots)) {
    RecountQuality();
}

Schedule::Row& Schedule::operator[](int64_t i) {
    return table_[i];
}

Schedule::Schedule(const Schedule::Table& table)
        : table_(table) {
    RecountQuality();
}

Schedule::Schedule(Schedule::Table&& table)
        : table_(std::move(table)) {
    RecountQuality();
}

const Schedule::Row& Schedule::operator[](int64_t i) const {
    return table_[i];
}

double Schedule::GetQuality() const {
    return quality_;
}

double Schedule::GetNormalizedQuality() const {
    return quality_ / data.n_students;
}

void Schedule::RecountQuality() {
    count_ = CreateMatrix2D<int64_t>(data.n_students, data.n_slots);
    for (int64_t teacher_id = 0; teacher_id < data.n_teachers; ++teacher_id) {
        for (int64_t slot_id = 0; slot_id < data.n_slots; ++slot_id) {
            if (table_[teacher_id][slot_id].has_value()) {
                ++count_[table_[teacher_id][slot_id].value()][slot_id];
            }
        }
    }
    quality_ = 0;
    for (int64_t student_id = 0; student_id < data.n_students; ++student_id) {
        for (int64_t slot_id = 0; slot_id < data.n_slots; ++slot_id) {
            quality_ += std::max<int64_t>(0LL, count_[student_id][slot_id] - data.student_available[student_id][slot_id]);
        }
    }
}

double Schedule::ApplyMove(Move move) {
    double old_quality = quality_;
    auto [teacher_id, slot_x, slot_y] = std::tie(move.teacher_id, move.slot_x, move.slot_y);

    for (auto [from, to] : {std::make_pair(slot_x, slot_y), std::make_pair(slot_y, slot_x)}) {
        if (!table_[teacher_id][from].has_value()) {
            continue;
        }
        int64_t student = table_[teacher_id][from].value();
        if (--count_[student][from] >= data.student_available[student][from]) {
            --quality_;
        }
        if (++count_[student][to] > data.student_available[student][to]) {
            ++quality_;
        }
    }

    std::swap(table_[teacher_id][slot_x], table_[teacher_id][slot_y]);
    return quality_ - old_quality;
}

double Schedule::ApplyMoves(const std::vector<Move>& moves) {
    double delta_quality = 0;
    for (auto move : moves) {
        delta_quality = ApplyMove(move);
    }
    return delta_quality;
}

void Schedule::RollbackMove(Move move) {
    ApplyMove(move);
}

void Schedule::RollbackMoves(const std::vector<Move>& moves) {
    for (auto move : moves) {
        RollbackMove(move);
    }
}

double Schedule::SimulateMove(Move move) {
    double delta_quality = ApplyMove(move);
    RollbackMove(move);
    return delta_quality;
}

double Schedule::SimulateMoves(const std::vector<Move>& moves) {
    double delta_quality = ApplyMoves(moves);
    RollbackMoves(moves);
    return delta_quality;
}

Schedule Schedule::Create(uint32_t n, uint32_t k) {
    return Schedule(n, k);
}

std::vector<Schedule::Row>::iterator Schedule::begin() {
    return table_.begin();
}

std::vector<Schedule::Row>::iterator Schedule::end() {
    return table_.end();
}



