cmake_minimum_required(VERSION 3.14)
project(arghand-tests LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)

add_executable(arghand-tests "src/Test.cpp")
include_directories(arghand-tests PRIVATE "${CMAKE_SOURCE_DIR}/../Arghand/include")