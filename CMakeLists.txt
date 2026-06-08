cmake_minimum_required(VERSION 3.10)
project(EasyCPPSockets)

set(TARGET ${PROJECT_NAME})

add_library("${TARGET}" SHARED "${CMAKE_SOURCE_DIR}/src/SocketBuffer.cpp"  "${CMAKE_SOURCE_DIR}/src/ServerSocket.cpp" "${CMAKE_SOURCE_DIR}/src/Socket.cpp")

target_include_directories("${TARGET}" PUBLIC
  # Take headers from source when compiling
  $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/src/include>
  # Take headers from install dir for consumers
  $<INSTALL_INTERFACE:include>
)

# Install dir structure
install(TARGETS "${TARGET}"
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
)
install(DIRECTORY "${CMAKE_SOURCE_DIR}/src/include/"
    DESTINATION include
)

# Subdirectories for tests
add_subdirectory("${CMAKE_SOURCE_DIR}/tests/pingpong")
add_subdirectory("${CMAKE_SOURCE_DIR}/tests/pingpong-no-sockstream")