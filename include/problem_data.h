#ifndef MYACO_PROBLEM_DATA_H
#define MYACO_PROBLEM_DATA_H

#include "matrix.h"

#include <cstdint>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>

struct ProblemData {
    int64_t n_teachers{};
    int64_t n_students{};
    int64_t n_slots{};
    Matrix2D<int64_t> requirements;
    Matrix2D<int64_t> teacher_available;
    Matrix2D<int64_t> student_available;
};

class ProblemDataReader {
public:
    static ProblemData ReadFromStream(std::istream& in);
    static ProblemData ReadFromFile(const std::string& filename);

private:
    static void StartSection(std::istream& in, const std::string& section_name);
};

namespace myaco {
inline ProblemData data;
}

#endif //MYACO_PROBLEM_DATA_H
