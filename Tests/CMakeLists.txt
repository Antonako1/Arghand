cmake_minimum_required(VERSION 3.14)
project(arghand-tests LANGUAGES CXX)


list(APPEND CMAKE_PREFIX_PATH "./install")
find_package(Arghand REQUIRED)
add_executable(arghand-tests "src/Test.cpp")
target_link_libraries(arghand-tests PRIVATE Arghand::Arghand)