#include "annealing.h"
#include "config.h"
#include "quality.h"
#include <cmath>

using namespace myaco;

Annealer::Annealer(Schedule initial_schedule)
        : LocalSearcher(std::move(initial_schedule))
        , temperature_(config.sa_initial_temperature)
        , best_quality_(QualityEstimator::Estimate(initial_schedule)) {
}

void Annealer::Run() {
    Schedule current_schedule = best_schedule_;
    double current_quality = best_quality_;
    for (int64_t iter = 0; iter < config.n_iterations_sa; ++iter) {
        Schedule neighbor = GenerateNeighbor(current_schedule);
        double neighbor_quality = QualityEstimator::Estimate(neighbor);
        if (AcceptMove(current_quality, neighbor_quality)) {
            current_schedule = std::move(neighbor);
            current_quality = neighbor_quality;
        }
        if (best_quality_ < current_quality) {
            best_schedule_ = current_schedule;
            best_quality_ = current_quality;
        }
        temperature_ *= config.sa_cooling_rate;
    }
}

bool Annealer::AcceptMove(double from, double to) {
    if (from >= to) {
        return true;
    }
    if (temperature_ == 0) {
        return false;
    }
    return std::exp((from - to) / temperature_) > threshold_gen_(rng_);
}
