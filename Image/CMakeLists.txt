cmake_minimum_required(VERSION 3.05)
project(Image)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3")

add_executable(Image main.cpp generate_data.h utilityfunction.h MultiplexGreedy.h FairKnapStream.h MatKnapStream.h SmkStream.h StreamingHeuristic.h)