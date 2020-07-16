cmake_minimum_required(VERSION 3.5)
project(data_check)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -pthread -Wall")
set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(SRC_LIST
        src/main.cpp
        src/argparse.h
        src/write.h
        libs/CRC.h
        src/data.h
        src/read.h
)

add_executable(${PROJECT_NAME} ${SRC_LIST})

