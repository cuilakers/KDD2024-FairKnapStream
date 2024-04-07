cmake_minimum_required(VERSION 3.05)
project(Movie)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3")

add_executable(Movie main.cpp utilityfunction.h MultiplexGreedy.h FairKnapStream.h MatKnapStream.h SmkStream.h StreamingHeuristic.h)
