cmake_minimum_required(VERSION 3.10)
project(CPPSockets)

set(TARGET ${PROJECT_NAME})

add_executable("${TARGET}Server" "${CMAKE_SOURCE_DIR}/src/server.cpp")
target_include_directories("${TARGET}Server" PUBLIC "${CMAKE_SOURCE_DIR}/src/include")

add_executable("${TARGET}Client" "${CMAKE_SOURCE_DIR}/src/client.cpp")
target_include_directories("${TARGET}Client" PUBLIC "${CMAKE_SOURCE_DIR}/src/include")