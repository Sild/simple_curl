cmake_minimum_required(VERSION 3.14)
project(simple_curl)

set(CMAKE_CXX_STANDARD 14)
set(CXX_STANDARD_REQUIRED ON)
set(COMPILE_OPTIONS "-Wpedantic;-Wall;-Wextra")

add_executable(simple_curl
    main.cpp
    tcp.cpp
    http.cpp
)