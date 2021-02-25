#include "problem_data.h"

#include <fstream>
#include <filesystem>
#include <stdexcept>

ProblemData ProblemDataReader::ReadFromStream(std::istream& in) {
    ProblemData data;
    in >> data.n_teachers >> data.n_students >> data.week_length >> data.day_length;
    if (std::min({data.n_teachers, data.n_students, data.week_length, data.day_length}) <= 0) {
        throw std::domain_error("all principal parameters must be positive");
    }
    if (data.week_length % data.day_length != 0) {
        throw std::domain_error("week length should be a multiple of day length");
    }

    data.requirements = myaco::CreateMatrix2D<int64_t>(data.n_teachers, data.n_students);
    data.available = data.convenient = myaco::CreateMatrix2D<int16_t>(data.n_teachers, data.week_length);

    StartSection(in, "requirements");
    for (int64_t i = 0; i < data.n_teachers; ++i) {
        for (int64_t j = 0; j < data.n_students; ++j) {
            in >> data.requirements[i][j];
            if (data.requirements[i][j] < 0) {
                throw std::domain_error("requirements should be non-negative");
            }
        }
    }

    StartSection(in, "teacher_availability");
    for (int64_t i = 0; i < data.n_teachers; ++i) {
        for (int64_t k = 0; k < data.week_length; ++k) {
            in >> data.available[i][k];
            if (data.available[i][k] != 0 && data.available[i][k] != 1) {
                throw std::domain_error("teacher_availability should be a binary matrix");
            }
        }
    }

    StartSection(in, "teacher_convenience");
    for (int64_t i = 0; i < data.n_teachers; ++i) {
        for (int64_t k = 0; k < data.week_length; ++k) {
            in >> data.convenient[i][k];
            if (data.convenient[i][k] != 0 && data.convenient[i][k] != 1) {
                throw std::domain_error("teacher_availability should be a binary matrix");
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
        throw std::domain_error("wrong format of problem data: could not find section " + section_name);
    }
}