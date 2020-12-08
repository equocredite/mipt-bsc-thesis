cmake_minimum_required(VERSION 3.16)
project(myaco)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++20 -pthread")

include_directories("include")

add_executable(myaco src/main.cpp src/optimizer.cpp src/problem_data.cpp src/colony.cpp src/matrix.cpp)
target_link_libraries(myaco pthread)