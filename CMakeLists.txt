cmake_minimum_required(VERSION 3.14)
project(cppbatis)

set(CMAKE_CXX_STANDARD 20)

# 基础依赖
include_directories(${CMAKE_SOURCE_DIR})
include_directories(/usr/local/include)
include_directories(/usr/local/jsoncpp-1.9.5/include)
message("###${CMAKE_SOURCE_DIR}")

#基础库路径
link_directories(/usr/local/lib)

#头文件
include_directories(connection data_struct prepare_stmt reflection)

# 生成动态库
add_library(${PROJECT_NAME}
        connection/connection.cpp
        prepare_stmt/prepare_stmt.cpp)

# 生成测试可执行文件
add_executable(${PROJECT_NAME}_test
        main.cpp)

target_link_libraries(${PROJECT_NAME}_test ${PROJECT_NAME} mysqlclient jsoncpp)
