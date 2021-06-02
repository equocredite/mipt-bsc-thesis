#include "ant_optimizer.h"
#include "quality.h"
#include "late_acceptance.h"
#include "hill_climbing.h"
#include "annealing.h"
#include "parallel_run.h"
#include "tabu_search.h"

#include <algorithm>
#include <iostream>
#include <iomanip>
#include <filesystem>

using namespace myaco;
namespace fs = std::filesystem;

void RunTest(const fs::path& path_in, std::optional<fs::path> path_out = std::nullopt) {
    data = ProblemDataReader::ReadFromFile(path_in);
    auto initial_schedule = AntOptimizer::CreateInitialSchedule();
    Annealer opt(std::move(initial_schedule));
//    ParallelRunner<Annealer> opt;
//    opt.InitSchedules(initial_schedule);
    opt.Run(300s, Timer::fsec(1));
    std::cout << opt.GetSchedule().GetNormalizedQuality() << " " << opt.GetSchedule().GetQuality() << std::endl;
    return;
//    for (auto& entry : opt.GetHistory()) {
//        std::cout << entry.second << " ";
//    }
//    std::cout << opt.GetSchedule().GetNormalizedQuality() << " " << opt.GetSchedule().GetQuality() << std::endl;
//    return;
//    std::pair<double, double> result = {opt.GetSchedule().GetNormalizedQuality(), opt.GetHistory().back().first.count()};
//
//    if (path_out.has_value()) {
//        std::ofstream out(path_out.value());
//        out << "(" << data.teacher_availability_factor << ", " << data.student_availability_factor << ", "
//            << data.distribution_factor << ", " << data.total_requirements << "): (" << result.first << ", " << result.second << "),"
//            << std::endl;
//        out.close();
//    } else {
//        std::cout << "(" << data.teacher_availability_factor << ", " << data.student_availability_factor << ", "
//                  << data.distribution_factor << ", " << data.total_requirements << "): (" << result.first << ", " << result.second << "),"
//                  << std::endl;
//    }
}

int main(int argc, char** argv) {
//    for (int i = 35; i >= 2; --i) {
//        RunTest("../../tests/test" + std::to_string(i));
//    }
    const char* path = argv[1];
    if (fs::is_directory(path)) {
        for (auto& p : fs::directory_iterator(path)) {
            RunTest(p, argc > 2 ? std::make_optional(argv[2]) : std::nullopt);
        }
    } else if (fs::is_regular_file(path)) {
        RunTest(path, argc > 2 ? std::make_optional(argv[2]) : std::nullopt);
    } else {
        std::cerr << "bad input path" << std::endl;
    }
}
