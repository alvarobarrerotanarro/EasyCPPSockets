cmake_minimum_required(VERSION 3.10)
project(EasyCPPSockets)

set(TARGET ${PROJECT_NAME})

add_library("${TARGET}" SHARED "${CMAKE_SOURCE_DIR}/src/SocketBuffer.cpp"  "${CMAKE_SOURCE_DIR}/src/ServerSocket.cpp" "${CMAKE_SOURCE_DIR}/src/Socket.cpp")
target_include_directories("${TARGET}" PUBLIC "${CMAKE_SOURCE_DIR}/src/include")

add_subdirectory("${CMAKE_SOURCE_DIR}/tests/pingpong")
add_subdirectory("${CMAKE_SOURCE_DIR}/tests/pingpong-no-sockstream")