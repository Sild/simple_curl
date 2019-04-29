cmake_minimum_required(VERSION 3.5)
project(simple_curl)

set(CMAKE_CXX_STANDARD 14)
set(CXX_STANDARD_REQUIRED ON)
set(COMPILE_OPTIONS "-Wpedantic;-Wall;-Wextra")
set(CMAKE_CXX_FLAGS -pthread)
set(CMAKE_CXX_COMPILER g++)

find_package(Threads REQUIRED)

add_executable(simple_curl
    main.cpp
)

add_library(tcp
        tcp.cpp
)

add_library(http
        http.cpp
)

target_link_libraries(simple_curl
        tcp
        http
)