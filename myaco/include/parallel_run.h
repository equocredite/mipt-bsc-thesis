#ifndef MYACO_PARALLEL_RUN_H
#define MYACO_PARALLEL_RUN_H

#include <cstdint>
#include <cassert>
#include <thread>
#include "optimizer.h"

template<typename Optimizer, typename Perturbator = SimplePerturbator>
class ParallelRunner {
public:
    explicit ParallelRunner(uint32_t n_threads = std::thread::hardware_concurrency() - 1)
            : n_threads_(n_threads) {
        optimizers_.reserve(n_threads_);
    }

    void InitSchedules(const Schedule& initial_schedule) {
        for (uint32_t i = 0; i < n_threads_; ++i) {
            Schedule schedule = initial_schedule;
            optimizers_.emplace_back(Optimizer(std::move(schedule), new Perturbator()));
        }
    }

    void InitSchedules(std::vector<Schedule>& initial_schedules) {
        assert(initial_schedules.size() == n_threads_);
        for (uint32_t i = 0; i < n_threads_; ++i) {
            optimizers_.emplace_back(Optimizer(std::move(initial_schedules[i]), new Perturbator()));
        }
    }

    void Run(Timer::fsec max_time, Timer::fsec log_frequency) {
        std::vector<std::thread> threads;
        for (Optimizer& opt : optimizers_) {
            threads.emplace_back(std::thread(&Optimizer::Run, std::ref(opt), max_time, log_frequency));
        }
        for (auto& thread : threads) {
            thread.join();
        }
    }

    auto GetHistories() {
        Matrix2D<std::pair<Timer::fsec, double>> history;
        for (Optimizer& opt : optimizers_) {
            history.push_back(opt.GetHistory());
        }
        return history;
    }

    Schedule& GetSchedule() {
        auto best_opt = std::min_element(optimizers_.begin(), optimizers_.end(), [](Optimizer& opt1, Optimizer& opt2) {
            return opt1.GetSchedule().GetQuality() < opt2.GetSchedule().GetQuality();
        });
        return best_opt->GetSchedule();
    };

private:
    std::vector<Optimizer> optimizers_{};
    IPerturbator* perturbator_;
    const uint32_t n_threads_;
};

#endif //MYACO_PARALLEL_RUN_H
