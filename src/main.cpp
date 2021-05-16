#include "ant_optimizer.h"
#include "quality.h"
#include "late_acceptance.h"
#include "annealing.h"
#include "parallel_run.h"
#include "tabu_search.h"

#include <iostream>
#include <iomanip>

using namespace myaco;

int main(int argc, char** argv) {
    ProblemDataReader::ReadFromFile(argv[1]);

    AntOptimizer ant;
    //opt.Run(config.aco_max_time);
    auto initial_schedule = ant.GetSchedule();

    Annealer opt(std::move(initial_schedule));
    opt.Run(10s, Timer::fsec(1e-1));
    for (auto i : opt.GetHistory()) {
        std::cout << i.second << " ";
    }
    std::cout << std::endl;
    return 0;

    auto schedule = opt.GetSchedule();

    for (auto& row : schedule) {
        for (auto& elem : row) {
            std::cout << std::setw(3);
            if (elem.has_value()) {
                std::cout << elem.value();
            } else {
                std::cout << "#";
            }
        }
        std::cout << std::endl;
    }


}
