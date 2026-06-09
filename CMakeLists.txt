cmake_minimum_required(VERSION 3.10)
project(EasyCPPSockets VERSION 1.0.0)

file(GLOB EasyCPPSockets_SOURCE_FILES "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")
add_library(EasyCPPSockets SHARED ${EasyCPPSockets_SOURCE_FILES})
add_library(EasyCPPSockets::EasyCPPSockets ALIAS EasyCPPSockets)

target_compile_features(EasyCPPSockets PUBLIC cxx_std_17)

target_include_directories(EasyCPPSockets PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>
)

# Soporte para instalacion
install(TARGETS EasyCPPSockets
    EXPORT EasyCPPSocketsTargets
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
    INCLUDES DESTINATION include
)
install(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/include/EasyCPPSockets" DESTINATION include)
install(EXPORT EasyCPPSocketsTargets
    FILE EasyCPPSocketsTargets.cmake
    NAMESPACE EasyCPPSockets::
    DESTINATION lib/cmake/EasyCPPSockets
)

# Config file (find_package support)
include(CMakePackageConfigHelpers)

configure_package_config_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/cmake/EasyCPPSocketsConfig.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/EasyCPPSocketsConfig.cmake
    INSTALL_DESTINATION lib/cmake/EasyCPPSockets
)
write_basic_package_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/EasyCPPSocketsConfigVersion.cmake
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
)
install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/EasyCPPSocketsConfig.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/EasyCPPSocketsConfigVersion.cmake
    DESTINATION lib/cmake/EasyCPPSockets
)

# Tests
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/tests/pingpong")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/tests/pingpong-no-sockstream")