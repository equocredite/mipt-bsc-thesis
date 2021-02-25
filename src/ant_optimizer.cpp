#include "ant_optimizer.h"
#include <algorithm>

using namespace myaco;

AntOptimizer::AntOptimizer()
        : AntOptimizer(CreateInitialSchedule()) {
}

AntOptimizer::AntOptimizer(Schedule&& initial_schedule) {

}

void AntOptimizer::Run() {
    for (int64_t iter = 1; iter <= config.n_iterations_aco; ++iter) {
        std::vector<std::thread> threads;
        for (Colony& colony : colonies_) {
            threads.emplace_back(std::thread(&Colony::MakeIteration, std::ref(colony)));
        }
        for (auto& thread : threads) {
            thread.join();
        }
        if (config.sync_frequency != 0 && iter % config.sync_frequency == 0) {
            SyncColonies();
        }
    }
}

Schedule AntOptimizer::GetSchedule() {
    return GetBestColony()->GetSchedule();
}

void AntOptimizer::SyncColonies() {
    auto best_colony = GetBestColony();
    auto best_trail = best_colony->GetTrail();
    for (auto it = colonies_.begin(); it != colonies_.end(); ++it) {
        if (it != best_colony) {
            it->SetTrail(best_trail);
        }
    }
}

std::vector<Colony>::iterator AntOptimizer::GetBestColony() {
    return std::min_element(colonies_.begin(),colonies_.end(),[](const Colony& c1, const Colony& c2) {
        return c1.GetQuality() < c2.GetQuality();
    });
}

Schedule AntOptimizer::CreateInitialSchedule() {
    // TODO
    return CreateSchedule(data.n_teachers, data.week_length);
}


