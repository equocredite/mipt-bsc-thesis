#include <iostream>

#include "optimizer.h"

int main(int argc, char** argv) {
    if (argc != 3) {
        throw std::invalid_argument("provide i/o file names");
    }
    std::string filename_in  = argv[1];
    std::string filename_out = argv[2];

    auto data = DataReader::ReadFromFile(filename_in);
    AntOptimizer opt(std::move(data));
    opt.Run();
    auto schedule = opt.GetSchedule();
}
