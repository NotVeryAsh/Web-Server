cmake_minimum_required(VERSION 3.29)
project(Web_Server)

set(CMAKE_CXX_STANDARD 14)

include_directories(.)

add_executable(Web_Server
    Server.cpp)

target_link_libraries(Web_Server wsock32 ws2_32)
set(CMAKE_EXE_LINKER_FLAGS "-static")