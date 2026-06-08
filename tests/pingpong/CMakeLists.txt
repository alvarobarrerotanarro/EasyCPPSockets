cmake_minimum_required(VERSION 3.10)
project(PingPongTest)

set(TEST_TARGET_PREFIX ${PROJECT_NAME})
set(TEST_DIR_PREFIX "${CMAKE_SOURCE_DIR}/tests/pingpong")

add_executable("${TEST_TARGET_PREFIX}Server" "${TEST_DIR_PREFIX}/server.cpp")
target_link_libraries("${TEST_TARGET_PREFIX}Server" PRIVATE "${TARGET}")

add_executable("${TEST_TARGET_PREFIX}Client" "${TEST_DIR_PREFIX}/client.cpp")
target_link_libraries("${TEST_TARGET_PREFIX}Client" PRIVATE "${TARGET}")