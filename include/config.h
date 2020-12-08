#ifndef MYACO_CONFIG_H
#define MYACO_CONFIG_H

#include <algorithm>
#include <cstdint>
#include <thread>

struct Config {
    Config() {
    }

    int64_t n_ants = 30; // per colony
    int64_t n_colonies = std::max(std::thread::hardware_concurrency(), 1U);
    int64_t n_iterations_aco = 100;
    int64_t n_iterations_ls = 100;
    int64_t sync_frequency = 10;
    double evaporation_rate = 0.1;
    double local_decay_rate = 0.1;
    double initial_trail = 1.;
    double trail_weight = 1.;
    double visibility_weight = 2.;
};

#endif //MYACO_CONFIG_H
