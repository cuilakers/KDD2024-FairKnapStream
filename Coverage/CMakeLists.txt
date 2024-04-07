cmake_minimum_required(VERSION 3.05)
project(Coverage)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3")

add_executable(Coverage main.cpp utilityfunction.h StreamingHeuristic.h MatKnapStream.h FairKnapStream.h MultiplexGreedy.h SmkStream.h)

