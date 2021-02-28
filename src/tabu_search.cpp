#include "config.h"
#include "tabu_search.h"
#include "quality.h"

using namespace myaco;

TabuSearcher::TabuSearcher(Schedule initial_schedule)
        : LocalSearcher(std::move(initial_schedule))
        , best_quality_(QualityEstimator::Estimate(best_schedule_)) {

}

void TabuSearcher::Run() {
    Schedule current_schedule = best_schedule_;
    double current_quality = best_quality_;
    for (int64_t iter = 0; iter < config.n_iterations_ts; ++iter) {
        Schedule best_neighbor;
        Move best_move{};
        double best_neighbor_quality = -1;
        for (int64_t i = 0; i < config.ts_neighborhood_size; ++i) {
            auto [neighbor, move] = GenerateNeighbor(current_schedule);
            double neighbor_quality = QualityEstimator::Estimate(neighbor);
            if ((best_neighbor_quality < 0 || best_neighbor_quality > neighbor_quality) && !IsTabu(move)) {
                best_neighbor = std::move(neighbor);
                best_neighbor_quality = neighbor_quality;
                best_move = move;
            }
        }
        if (best_neighbor_quality >= 0) {
            current_schedule = std::move(best_neighbor);
            current_quality = best_neighbor_quality;
            AddTabu(best_move);
        }
        if (best_quality_ > current_quality) {
            best_schedule_ = current_schedule;
            best_quality_ = current_quality;
        }
    }
}

bool TabuSearcher::IsTabu(LocalSearcher::Move move) const {
    return std::find(tabus.begin(), tabus.end(), move) != tabus.end();
}

void TabuSearcher::AddTabu(LocalSearcher::Move move) {
    tabus.push_back(move);
    if (tabus.size() > config.ts_tabu_tenure) {
        tabus.pop_front();
    }
}
