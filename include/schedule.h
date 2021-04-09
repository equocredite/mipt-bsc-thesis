#ifndef MYACO_SCHEDULE_H
#define MYACO_SCHEDULE_H

#include "matrix.h"
#include "problem_data.h"

class Schedule {
public:
    struct Move {
        int64_t teacher_id;
        int64_t slot_x;
        int64_t slot_y;

        bool operator ==(const Move& other) const {
            return teacher_id == other.teacher_id &&
                   std::minmax(slot_x, slot_y) == std::minmax(other.slot_x, other.slot_y);
        }

        bool operator < (const Move& other) const {
            return std::tie(teacher_id, slot_x, slot_y) < std::tie(other.teacher_id, other.slot_y, other.slot_y);
        }
    };

    using Row = std::vector<std::optional<int64_t>>;
    using Table = std::vector<Row>;

    explicit Schedule(int64_t n = myaco::data.n_teachers, int64_t k = myaco::data.n_slots);

    explicit Schedule(const Table& table);

    explicit Schedule(Table&& table);

    Row& operator [](int64_t i);

    const Row& operator [](int64_t i) const;

    double GetQuality() const;

    double ApplyMove(Move move);

    double ApplyMoves(const std::vector<Move>& moves);

    void RollbackMove(Move move);

    void RollbackMoves(const std::vector<Move>& moves);

    double SimulateMove(Move move);

    double SimulateMoves(const std::vector<Move>& moves);

    static Schedule Create(uint32_t n, uint32_t k);

    Table::iterator begin();

    Table::iterator end();

private:
    void RecountQuality();

    std::vector<Row> table_;
    Matrix2D<int64_t> count_;
    double quality_{};
};

#endif //MYACO_SCHEDULE_H
