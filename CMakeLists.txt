cmake_minimum_required(VERSION 3.5)
project(simple_curl)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_REQUIRED_FLAGS -std=c++17)
set(COMPILE_OPTIONS "-Wpedantic;-Wall;-Wextra")
set(CMAKE_CXX_FLAGS -pthread)
set(CMAKE_CXX_COMPILER g++)

find_package(Threads REQUIRED)

add_executable(simple_curl
    main.cpp
)

add_library(http
        http.cpp
        tcp.cpp
)

target_link_libraries(simple_curl
        http
)