#ifndef MYACO_TIME_UTIL_H
#define MYACO_TIME_UTIL_H

#include <chrono>

class Timer {
public:
    using point = std::chrono::time_point<std::chrono::steady_clock>;
    using fsec = std::chrono::duration<double>;

    static constexpr auto kInfiniteSeconds = fsec(std::numeric_limits<int64_t>::max());

    explicit Timer(fsec max_time);

    static point Now();

    fsec TimeElapsed() const;

    fsec TimeLeft() const;

    bool Expired() const;

    bool Tick(fsec time);

private:
    point start_;
    point previous_tick_;
    fsec max_time_;
};

#endif //MYACO_TIME_UTIL_H
