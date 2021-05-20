#include "late_acceptance.h"
#include "quality.h"
#include "config.h"

using namespace myaco;

LAHillClimber::LAHillClimber(Schedule&& initial_schedule, IPerturbator* perturbator)
        : LocalSearcher(std::move(initial_schedule), perturbator) {
    qualities_.resize(config.lahc_tenure, best_schedule_.GetQuality());
}

void LAHillClimber::Run(Timer::fsec max_time, Timer::fsec log_frequency) {
    Timer timer(max_time);
    Schedule current_schedule = best_schedule_;
    int64_t ptr = 0;
    int64_t n_idle_iterations = 0;
    while (!timer.Expired()) {
        if (timer.Tick(log_frequency)) {
            Log(timer);
        }
        if (best_schedule_.GetQuality() == 0 || ++n_idle_iterations == config.lahc_max_idle_iterations) {
            break;
        }

        auto moves = perturbator_->GenerateMoves(current_schedule);
        double delta_quality = current_schedule.ApplyMoves(moves);
        if (delta_quality <= 0 || qualities_[ptr] > current_schedule.GetQuality()) {
            if (best_schedule_.GetQuality() > current_schedule.GetQuality()) {
                best_schedule_ = current_schedule;
                n_idle_iterations = 0;
            }
        } else {
            current_schedule.RollbackMoves(moves);
        }

        qualities_[ptr] = std::min(qualities_[ptr], current_schedule.GetQuality());
        ptr = (ptr + 1) % qualities_.size();
    }
    Log(timer);
}

DLASearcher::DLASearcher(Schedule&& initial_schedule, IPerturbator* perturbator)
        : LAHillClimber(std::move(initial_schedule), perturbator) {
}

void DLASearcher::Run(Timer::fsec max_time, Timer::fsec log_frequency) {
    Timer timer(max_time);
    Schedule current_schedule = best_schedule_;
    double max_quality = best_schedule_.GetQuality();
    int64_t n_occurrences = qualities_.size();
    int64_t ptr = 0;
    int64_t n_idle_iterations = 0;

    while (!timer.Expired()) {
        if (timer.Tick(log_frequency)) {
            Log(timer);
        }
        if (best_schedule_.GetQuality() == 0 || ++n_idle_iterations == config.lahc_max_idle_iterations) {
            break;
        }

        double previous_quality = current_schedule.GetQuality();
        auto moves = perturbator_->GenerateMoves(current_schedule);
        double neighbor_quality = previous_quality + current_schedule.ApplyMoves(moves);
        if (neighbor_quality == previous_quality || neighbor_quality < max_quality) {
            if (best_schedule_.GetQuality() > current_schedule.GetQuality()) {
                best_schedule_ = current_schedule;
                n_idle_iterations = 0;
            }
        }
        if (qualities_[ptr] < current_schedule.GetQuality()) {
            qualities_[ptr] = current_schedule.GetQuality();
        } else if (qualities_[ptr] > current_schedule.GetQuality() && previous_quality > current_schedule.GetQuality()) {
            if (qualities_[ptr] == max_quality) {
                --n_occurrences;
            }
            qualities_[ptr] = current_schedule.GetQuality();
            if (n_occurrences == 0) {
                max_quality = *std::max(qualities_.begin(), qualities_.end());
                n_occurrences = std::count(qualities_.begin(), qualities_.end(), max_quality);
            }
        }

        ptr = (ptr + 1) % qualities_.size();
    }
    Log(timer);
}
