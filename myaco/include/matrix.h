#ifndef MYACO_MATRIX_H
#define MYACO_MATRIX_H

#include <vector>
#include <optional>
#include <cstdlib>

template<typename T>
using Matrix2D = std::vector<std::vector<T>>;

template<typename T>
using Matrix3D = std::vector<Matrix2D<T>>;


namespace myaco {

template<typename T>
Matrix2D<T> CreateMatrix2D(size_t n, size_t m, T value = T()) {
    return Matrix2D<T>(n, std::vector<T>(m, value));
}

template<typename T>
Matrix3D<T> CreateMatrix3D(size_t n, size_t m, size_t k, T value = T()) {
    return Matrix3D<T>(n, CreateMatrix2D<T>(m, k, value));
}

} // myaco

#endif //MYACO_MATRIX_H
