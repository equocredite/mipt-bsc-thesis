#ifndef MYACO_PROBLEM_DATA_H
#define MYACO_PROBLEM_DATA_H

#include "matrix.h"

#include <cstdint>
#include <fstream>
#include <string>
#include <vector>

struct ProblemData {
    int64_t n_teachers;
    int64_t n_students;
    int64_t week_length;
    int64_t day_length;
    Matrix2D<int64_t> requirements;
    Matrix2D<int16_t> available;
    Matrix2D<int16_t> convenient;
};

class DataReader {
public:
    static ProblemData ReadFromStream(std::istream& in);
    static ProblemData ReadFromFile(const std::string& filename);

private:
    static void StartSection(std::istream& in, const std::string& section_name);
};

#endif //MYACO_PROBLEM_DATA_H
