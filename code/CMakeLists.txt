cmake_minimum_required(VERSION 3.8)
project(scheduler)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES main.cpp process.cpp process.h)
add_executable(scheduler ${SOURCE_FILES})