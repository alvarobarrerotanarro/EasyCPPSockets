cmake_minimum_required(VERSION 3.10)
project(PingPongClientExample)

include(FetchContent)

# TODO: Cambiar
# FetchContent_Declare(
#     EasyCPPSockets
#     GIT_REPOSITORY https://github.com/alvarobarrerotanarro/EasyCPPSockets.git
#     GIT_TAG dev
# )
FetchContent_Declare(
    EasyCPPSockets
    SOURCE_DIR /home/alvaro/code/cppsockets
)
FetchContent_MakeAvailable(EasyCPPSockets)

add_executable(${PROJECT_NAME} "${CMAKE_CURRENT_SOURCE_DIR}/client.cpp")
target_link_libraries(${PROJECT_NAME} PRIVATE EasyCPPSockets::EasyCPPSockets)