#include "time_util.h"

Timer::Timer(fsec max_time)
        : max_time_(max_time)
        , start_(Now())
        , previous_tick_(start_) {
}

Timer::point Timer::Now() {
    return std::chrono::steady_clock::now();
}

Timer::fsec Timer::TimeElapsed() const {
    return Now() - start_;
}

Timer::fsec Timer::TimeLeft() const {
    return max_time_ - TimeElapsed();
}

bool Timer::Expired() const {
    return TimeLeft() <= std::chrono::seconds(0);
}

bool Timer::Tick(Timer::fsec time) {
    auto now = Now();
    if (now - previous_tick_ >= time) {
        previous_tick_ = now;
        return true;
    }
    return false;
}


