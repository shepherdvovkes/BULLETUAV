cmake_minimum_required(VERSION 3.10)
project(tracker_lib CXX)

add_library(tracker SHARED tracker.cpp)
set_target_properties(tracker PROPERTIES POSITION_INDEPENDENT_CODE ON)
target_include_directories(tracker PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
