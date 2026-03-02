cmake_minimum_required(VERSION 3.10)
project(Transformation LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Eigen3 REQUIRED)

add_executable(Transformation main.cpp)

target_link_libraries(Transformation PRIVATE Eigen3::Eigen)
