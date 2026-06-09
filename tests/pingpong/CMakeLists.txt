cmake_minimum_required(VERSION 3.10)
project(PingPongTest)

set(TEST_TARGET_PREFIX ${PROJECT_NAME})

add_executable("${TEST_TARGET_PREFIX}Server" "${PROJECT_SOURCE_DIR}/server.cpp")
target_link_libraries("${TEST_TARGET_PREFIX}Server" PRIVATE EasyCPPSockets::EasyCPPSockets)

add_executable("${TEST_TARGET_PREFIX}Client" "${PROJECT_SOURCE_DIR}/client.cpp")
target_link_libraries("${TEST_TARGET_PREFIX}Client" PRIVATE EasyCPPSockets::EasyCPPSockets)