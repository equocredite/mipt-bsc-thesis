#include "annealing.h"
#include "config.h"
#include "quality.h"
#include "time_util.h"
#include <cmath>

using namespace myaco;

Annealer::Annealer(Schedule&& initial_schedule, IPerturbator* perturbator)
        : LocalSearcher(std::move(initial_schedule), perturbator)
        , temperature_(config.sa_initial_temperature) {
}

void Annealer::Run(Timer::fsec max_time, Timer::fsec log_frequency) {
    Timer timer(max_time);
    Schedule current_schedule = best_schedule_;
    int64_t n_idle_iterations = 0;
    while (!timer.Expired()) {
        if (timer.Tick(log_frequency)) {
            Log(timer);
        }
        if (best_schedule_.GetQuality() == 0 || ++n_idle_iterations == config.sa_max_idle_iterations) {
            break;
        }

        auto moves = perturbator_->GenerateMoves(current_schedule);
        double delta_quality = current_schedule.ApplyMoves(moves);
        if (!AcceptableMove(delta_quality)) {
            current_schedule.RollbackMoves(moves);
        }
        if (best_schedule_.GetQuality() > current_schedule.GetQuality()) {
            best_schedule_ = current_schedule;
            n_idle_iterations = 0;
        }

        temperature_ *= config.sa_cooling_rate;
    }
    Log(timer);
}

bool Annealer::AcceptableMove(double delta) {
    if (delta <= 0) {
        return true;
    }
    if (temperature_ == 0) {
        return false;
    }
    return std::exp((-delta) / temperature_) > threshold_gen_(rng_);
}
