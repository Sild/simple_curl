cmake_minimum_required(VERSION 3.8)
project(simple_curl)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_REQUIRED_FLAGS -std=c++17)
set(CMAKE_CXX_FLAGS -pthread)
set(CMAKE_CXX_COMPILER g++)

find_package(Threads REQUIRED)

add_executable(simple_curl
    main.cpp
)

target_compile_options(
        simple_curl
        PRIVATE
        -Wpedantic -Wall -Wextra -std=c++17
)

add_library(http
        http.cpp
        tcp.cpp
)

target_compile_options(
        http
        PRIVATE
        -Wpedantic -Wall -Wextra -std=c++17
)

target_link_libraries(simple_curl
        http
)



