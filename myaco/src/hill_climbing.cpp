#include "hill_climbing.h"
#include "quality.h"
#include "config.h"

using namespace myaco;

HillClimber::HillClimber(Schedule&& initial_schedule, IPerturbator* perturbator)
        : LocalSearcher(std::move(initial_schedule), perturbator) {
}

void HillClimber::Run(Timer::fsec max_time, Timer::fsec log_frequency) {
    Timer timer(max_time);
    int64_t n_idle_iterations = 0;
    while (!timer.Expired()) {
        if (timer.Tick(log_frequency)) {
            Log(timer);
        }
        if (best_schedule_.GetQuality() == 0 || ++n_idle_iterations == config.hc_max_idle_iterations) {
            break;
        }

        auto moves = perturbator_->GenerateMoves(best_schedule_);
        double delta_quality = best_schedule_.ApplyMoves(moves);
        if (delta_quality < 0) {
            n_idle_iterations = 0;
        } else {
            best_schedule_.RollbackMoves(moves);
        }
    }
    Log(timer);
}
