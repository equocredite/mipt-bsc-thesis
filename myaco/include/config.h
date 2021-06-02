#ifndef MYACO_CONFIG_H
#define MYACO_CONFIG_H

#include <algorithm>
#include <cstdint>
#include <thread>
#include <chrono>
#include "time_util.h"

using namespace std::chrono_literals;

struct Config {
    Config() noexcept = default;

    int64_t aco_n_ants = 10; // per colony
    int64_t aco_n_colonies = std::max(std::thread::hardware_concurrency() - 1, 1U);
    double decay_rate = 0.2;
    double initial_trail = 10.;
    double trail_weight = 4.;
    double visibility_weight = 2.;
    double trail_store_factor = 10.;

    double sa_initial_temperature = 100;
    double sa_cooling_rate = 0.9999;
    int64_t sa_max_idle_iterations = std::numeric_limits<int64_t>::max();

    int64_t n_iterations_ts = 100000;
    int64_t ts_neighborhood_size = 10;
    int64_t ts_tabu_tenure = 10;
    int64_t ts_max_idle_iterations = std::numeric_limits<int64_t>::max();

    int64_t hc_max_idle_iterations = std::numeric_limits<int64_t>::max();

    int64_t lahc_tenure = 5;
    int64_t lahc_max_idle_iterations = std::numeric_limits<int64_t>::max();
};

namespace myaco {
inline Config config;
}

#endif //MYACO_CONFIG_H
