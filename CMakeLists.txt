cmake_minimum_required(VERSION 3.14)
project(cppbatis)

set(CMAKE_CXX_STANDARD 20)

include_directories(${CMAKE_SOURCE_DIR})
include_directories(/usr/local/include)
include_directories(/usr/local/jsoncpp-1.9.5/include)
message("###${CMAKE_SOURCE_DIR}")

link_directories(/usr/local/lib)


#include_directories(/Users/yangbo/Downloads/mysql-8.0.30-macos12-arm64/include)
#link_directories(/Users/yangbo/Downloads/mysql-8.0.30-macos12-arm64/lib)

add_executable(cppbatis
        main.cpp
        connection/connection_pool.h
        connection/connection.cpp
        connection/connection.h
        connection/singleton_base.h
        prepare_stmt/prepare_stmt.h
        data_struct/user.h
        reflection/easy_json.h data_struct/test.h prepare_stmt/prepare_stmt.cpp)

target_link_libraries(cppbatis mysqlclient jsoncpp)
