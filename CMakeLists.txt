cmake_minimum_required (VERSION 3.5)

add_definitions(-Wall)

add_library(showgraph STATIC showgraph.c resampling.c)

add_executable(test_showgraph test.c)

target_link_libraries(test_showgraph showgraph)