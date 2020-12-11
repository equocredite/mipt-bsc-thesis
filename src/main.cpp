#include "optimizer.h"

int main(int argc, char** argv) {
    if (argc != 3) {
        throw std::invalid_argument("provide i/o file names");
    }
    std::string filename_in  = argv[1];
    std::string filename_out = argv[2];

    auto data = ProblemDataReader::ReadFromFile(filename_in);
    AntOptimizer opt(std::move(data));
    opt.Run();
    auto schedule = opt.GetSchedule();

    std::ofstream out(filename_out);
    for (auto& row : schedule) {
        for (auto& elem : row) {
            out << (elem.has_value() ? elem.value() : -1) << " ";
        }
        out << std::endl;
    }
}
