#ifndef MYACO_CONFIG_H
#define MYACO_CONFIG_H

#include <algorithm>
#include <cstdint>
#include <thread>

struct Config {
    Config() noexcept = default;

    int64_t n_ants = 30; // per colony
    int64_t n_colonies = std::max(std::thread::hardware_concurrency() - 1, 1U);
    int64_t n_iterations_aco = 100;
    int64_t n_iterations_ls = 100;
    int64_t sync_frequency = n_iterations_aco;
    double evaporation_rate = 0.1;
    double trail_store_factor = 1000;
    double local_decay_rate = 0.1;
    double initial_trail = 1.;
    double trail_weight = 1.;
    double visibility_weight = 2.;
    double simultaneous_assignments_penalty = 20.;
    double requirement_violation_penalty = 20.;
    double holes_weight = 1.;
    double inconvenience_weight = 3.;
    double splits_penalty = 6.;
};

namespace myaco {
inline const Config config;
}

#endif //MYACO_CONFIG_H
