#include "optimizer.h"
#include <algorithm>

AntOptimizer::AntOptimizer(ProblemData&& data, Config config)
        : data_(std::move(data))
        , config_(config)
        , colonies_(config_.n_colonies, Colony(&data, config_)) {
}

void AntOptimizer::Run() {
    for (int64_t iter = 0; iter < config_.n_iterations_aco; ++iter) {
        std::vector<std::thread> threads;
        for (Colony& colony : colonies_) {
            threads.emplace_back(std::thread(&Colony::MakeIteration, colony));
        }
        for (auto& thread : threads) {
            thread.join();
        }
        if (iter % config_.sync_frequency == 0) {
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