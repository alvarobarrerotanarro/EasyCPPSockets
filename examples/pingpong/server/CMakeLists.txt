cmake_minimum_required(VERSION 3.10)
project(PingPongServerExample)

find_package(EasyCPPSockets REQUIRED)

add_executable(${PROJECT_NAME} "${CMAKE_CURRENT_SOURCE_DIR}/server.cpp")
target_link_libraries(${PROJECT_NAME} PRIVATE EasyCPPSockets::EasyCPPSockets)