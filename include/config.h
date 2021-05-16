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

    Timer::fsec log_frequency = 1s;

    int64_t aco_n_ants = 10; // per colony
    int64_t aco_n_colonies = std::max(std::thread::hardware_concurrency(), 1U);
    //int64_t n_iterations_aco = 1000;
    //int64_t sync_frequency = 1;
    Timer::fsec aco_max_time = 60s;
    Timer::fsec aco_sync_frequency = 0.5s;
    double evaporation_rate = 0.1;
    double trail_store_factor = 1000;
    double local_decay_rate = 0.1;
    double initial_trail = 1.;
    double trail_weight = 1.;
    double visibility_weight = 1.;

    double sa_initial_temperature = 100;
    double sa_cooling_rate = 0.9999;
    int64_t sa_max_idle_iterations = std::numeric_limits<int64_t>::max();

    int64_t n_iterations_ts = 100000;
    int64_t ts_neighborhood_size = 10;
    int64_t ts_tabu_tenure = 10;
    int64_t ts_max_idle_iterations = std::numeric_limits<int64_t>::max();

    int64_t hc_max_idle_iterations = 100;

    int64_t lahc_tenure = 5;
    int64_t lahc_max_idle_iterations = std::numeric_limits<int64_t>::max();
};

namespace myaco {
inline Config config;
}

#endif //MYACO_CONFIG_H
