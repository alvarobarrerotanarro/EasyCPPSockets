cmake_minimum_required(VERSION 3.10)
project(PingPongExample)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/client)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/server)