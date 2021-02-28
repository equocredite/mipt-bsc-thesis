#include "annealing.h"
#include "config.h"
#include "quality.h"

using namespace myaco;

Annealer::Annealer(Schedule initial_schedule)
        : LocalSearcher(std::move(initial_schedule))
        , temperature_(config.sa_initial_temperature)
        , best_quality_(QualityEstimator::Estimate(initial_schedule)) {
}

void Annealer::Run() {
    // TODO
}
