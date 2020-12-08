#include "matrix.h"

Schedule myaco::CreateSchedule(size_t n, size_t p) {
    return CreateMatrix2D<std::optional<int64_t>>(n, p);
}