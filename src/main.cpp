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

    ParallelRunner<DLASearcher> runner;
    runner.InitSchedules(initial_schedule);
    runner.Run(Timer::fsec(10), Timer::fsec(1e-2));

    auto hists = runner.GetHistories();
    for (auto& hist : hists) {
        for (auto i : hist) {
            std::cerr << i.second << " ";
        }
        std::cerr << std::endl;
    }

//    for (auto i : opt.GetHistory()) {
//        std::cerr << i.second << " ";
//    }
//    std::cerr << std::endl;
//
//    if (argc < 3) {
//        return 0;
//    }
//
//    std::ofstream out(argv[2]);
//    auto result = opt.GetSchedule();
//    out << std::right;
//    for (auto& row : result) {
//        for (auto& elem : row) {
//            out << std::setw(3);
//            if (elem.has_value()) {
//                out << elem.value();
//            } else {
//                out << "#";
//            }
//        }
//        out << std::endl;
//    }
}
