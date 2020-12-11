#ifndef MYACO_PROBLEM_DATA_H
#define MYACO_PROBLEM_DATA_H

#include "matrix.h"

#include <cstdint>
#include <fstream>
#include <string>
#include <vector>

struct ProblemData {
    inline int64_t GetSlotId(int64_t day_id, int64_t hour_id) const {
        return day_id * day_length + hour_id;
    }

    inline int64_t GetNDays() const {
        return week_length / day_length;
    }

    inline int64_t GetDayBegin(int64_t day_id) const {
        return day_id * day_length;
    }

    inline int64_t GetDayEnd(int64_t day_id) const {
        return (day_id + 1) * day_length;
    }

    int64_t n_teachers;
    int64_t n_students;
    int64_t week_length;
    int64_t day_length;
    Matrix2D<int64_t> requirements;
    Matrix2D<int16_t> available;
    Matrix2D<int16_t> convenient;
};

class ProblemDataReader {
public:
    static ProblemData ReadFromStream(std::istream& in);
    static ProblemData ReadFromFile(const std::string& filename);

private:
    static void StartSection(std::istream& in, const std::string& section_name);
};

#endif //MYACO_PROBLEM_DATA_H
