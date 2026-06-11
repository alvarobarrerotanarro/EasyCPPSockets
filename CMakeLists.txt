cmake_minimum_required(VERSION 3.10)
project(EasyCPPSockets VERSION 1.0.0)

option(EASYCPPSOCKETS_BUILD_TESTS "Build EasyCPPSockets Tests" OFF)

# Configuracion EasyCPPSockets
file(GLOB EasyCPPSockets_SOURCE_FILES "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")
add_library(EasyCPPSockets SHARED ${EasyCPPSockets_SOURCE_FILES})
add_library(EasyCPPSockets::EasyCPPSockets ALIAS EasyCPPSockets)

target_compile_features(EasyCPPSockets PUBLIC cxx_std_17)

target_include_directories(EasyCPPSockets PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>
)


# Dependencias
include(FetchContent)

FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/refs/tags/v1.17.0.zip
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
if (EASYCPPSOCKETS_BUILD_TESTS)
    enable_testing()
    FetchContent_MakeAvailable(googletest)

    add_executable(EasyCPPSocketsTests
        ${CMAKE_CURRENT_SOURCE_DIR}/tests/ConnectionTest.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/tests/BasicSendAndReceiveTest.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/tests/SendAllAndRecvAllTest.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/tests/SockStreamTest.cpp
    )
    target_link_libraries(EasyCPPSocketsTests PRIVATE EasyCPPSockets::EasyCPPSockets GTest::gtest_main)
    target_compile_definitions(EasyCPPSocketsTests PRIVATE EASYCPPSOCKETS_SOURCE_DIR="${CMAKE_CURRENT_SOURCE_DIR}")

    include(GoogleTest)
    gtest_discover_tests(EasyCPPSocketsTests)
endif()