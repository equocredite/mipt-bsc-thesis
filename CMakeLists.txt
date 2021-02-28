cmake_minimum_required(VERSION 3.16)
project(myaco)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -pthread")

include_directories("include")

add_executable(myaco src/main.cpp src/ant_optimizer.cpp src/problem_data.cpp src/colony.cpp src/matrix.cpp
               src/quality.cpp src/local_search.cpp src/annealing.cpp src/tabu_search.cpp)
target_link_libraries(myaco pthread)
