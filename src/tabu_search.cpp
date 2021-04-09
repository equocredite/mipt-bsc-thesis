#include "config.h"
#include "tabu_search.h"
#include "quality.h"
#include "time_util.h"

using namespace myaco;

TabuSearcher::TabuSearcher(Schedule&& initial_schedule, IPerturbator* perturbator)
        : LocalSearcher(std::move(initial_schedule), perturbator) {
}

void TabuSearcher::Run(Timer::fsec max_time, Timer::fsec log_frequency) {
    Timer timer(max_time);
    Schedule current_schedule = best_schedule_;
    int64_t n_idle_iterations = 0;
    while (!timer.Expired()) {
        if (timer.Tick(log_frequency)) {
            Log(timer);
        }
        if (best_schedule_.GetQuality() == 0 || ++n_idle_iterations == config.ts_max_idle_iterations) {
            break;
        }

        std::vector<Schedule::Move> best_moves{};
        double best_delta = std::numeric_limits<double>::max();

        for (int64_t i = 0; i < config.ts_neighborhood_size; ++i) {
            auto moves = perturbator_->GenerateMoves(current_schedule);
            double delta = current_schedule.SimulateMoves(moves);
            if (best_delta > delta && (delta < 0 || !Tabu(moves))) {
                best_moves = std::move(moves);
                best_delta = delta;
            }
        }
        if (best_delta < std::numeric_limits<double>::max()) {
            current_schedule.ApplyMoves(best_moves);
            AddTabus(best_moves);
        }
        if (best_schedule_.GetQuality() > current_schedule.GetQuality()) {
            best_schedule_ = current_schedule;
            n_idle_iterations = 0;
        }
    }
    Log(timer);
}

bool TabuSearcher::Tabu(const std::vector<Schedule::Move>& moves) const {
    return std::any_of(moves.begin(), moves.end(), [this](const Schedule::Move& move) {
        return tabus_.find({move, 0}) != tabus_.end();
    });
}

void TabuSearcher::AddTabus(const std::vector<Schedule::Move>& moves) {
    for (auto it = tabus_.begin(); it != tabus_.end(); ) {
        if (--it->tenure == 0) {
            it = tabus_.erase(it);
        } else {
            ++it;
        }
    }
    for (const auto& move : moves) {
        tabus_.insert({move, config.ts_tabu_tenure});
    }
}
