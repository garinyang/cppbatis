cmake_minimum_required(VERSION 3.14)
project(cppbatis)

set(CMAKE_CXX_STANDARD 20)

include_directories(${CMAKE_SOURCE_DIR})
include_directories(/usr/local/include)
message("###${CMAKE_SOURCE_DIR}")

add_executable(cppbatis
        main.cpp
        connection/connection_pool.h
        connection/connection.cpp
        connection/connection.h
        connection/singleton_base.h
        prepare_stmt/prepare_stmt.h
        data_struct/user.h)

target_link_libraries(cppbatis mysqlclient)
