cmake_minimum_required(VERSION 3.3)
project(progtest1)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wall -pedantic -pthread")

set(SOURCE_FILES main.cpp)
add_executable(progtest1 ${SOURCE_FILES})
