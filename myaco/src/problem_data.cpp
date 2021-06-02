#include "problem_data.h"

#include <fstream>
#include <filesystem>
#include <stdexcept>

using namespace myaco;

ProblemData ProblemDataReader::ReadFromStream(std::istream& in) {
    ProblemData data;

    in >> data.n_teachers >> data.n_students >> data.n_slots
       >> data.teacher_availability_factor >> data.student_availability_factor >> data.distribution_factor;

    if (std::min({data.n_teachers, data.n_students, data.n_slots}) <= 0) {
        throw std::domain_error("all principal parameters must be positive");
    }

    data.requirements = CreateMatrix2D<int64_t>(data.n_teachers, data.n_students);
    data.teacher_available = CreateMatrix2D<int64_t>(data.n_teachers, data.n_slots);
    data.student_available = CreateMatrix2D<int64_t>(data.n_students, data.n_slots);

    StartSection(in, "requirements");
    for (int64_t i = 0; i < data.n_teachers; ++i) {
        for (int64_t j = 0; j < data.n_students; ++j) {
            in >> data.requirements[i][j];
            if (data.requirements[i][j] < 0) {
                throw std::domain_error("requirements should be non-negative");
            }
            data.total_requirements += data.requirements[i][j];
        }
    }

    StartSection(in, "teacher_availability");
    for (int64_t i = 0; i < data.n_teachers; ++i) {
        for (int64_t k = 0; k < data.n_slots; ++k) {
            in >> data.teacher_available[i][k];
            if (data.teacher_available[i][k] != 0 && data.teacher_available[i][k] != 1) {
                throw std::domain_error("teacher_availability should be a binary matrix");
            }
        }
    }

    StartSection(in, "student_availability");
    for (int64_t i = 0; i < data.n_students; ++i) {
        for (int64_t k = 0; k < data.n_slots; ++k) {
            in >> data.student_available[i][k];
            if (data.student_available[i][k] != 0 && data.student_available[i][k] != 1) {
                throw std::domain_error("student_availability should be a binary matrix");
            }
        }
    }

    return data;
}

ProblemData ProblemDataReader::ReadFromFile(const std::string &filename) {
    if (!std::filesystem::exists(filename)) {
        throw std::invalid_argument(filename + ": no such file");
    }
    std::ifstream in(filename);
    if (!in.good()) {
        throw std::runtime_error("couldn't open file");
    }
    return ReadFromStream(in);
}

void ProblemDataReader::StartSection(std::istream& in, const std::string& section_name) {
    std::string buf;
    in >> buf;
    if (buf != section_name) {
        throw std::domain_error("could not find section " + section_name);
    }
}