cmake_minimum_required(VERSION 3.22)

project(TinyServer)

# 设置 MySQL 相关变量，使用确切的路径
set(MYSQL_LIBRARY "/usr/lib/x86_64-linux-gnu/libmysqlclient.so")
set(MYSQL_INCLUDE_DIR "/usr/include/mysql")

# 设置 MySQL 相关变量，使用 aarch64 路径
set(MYSQL_LIBRARY "/usr/lib/aarch64-linux-gnu/libmysqlclient.so")
set(MYSQL_INCLUDE_DIR "/usr/include/mysql")

# 检查MySQL库文件是否存在
if(NOT EXISTS ${MYSQL_LIBRARY})
    message(STATUS "Looking for MySQL library in default location: ${MYSQL_LIBRARY}")
    message(FATAL_ERROR "MySQL library not found. Please install libmysqlclient-dev")
endif()

if(NOT EXISTS ${MYSQL_INCLUDE_DIR})
    message(STATUS "Looking for MySQL headers in default location: ${MYSQL_INCLUDE_DIR}")
    message(FATAL_ERROR "MySQL headers not found. Please install libmysqlclient-dev")
endif()

message(STATUS "Found MySQL library: ${MYSQL_LIBRARY}")
message(STATUS "Found MySQL include: ${MYSQL_INCLUDE_DIR}")

add_subdirectory(src)

#enable_testing()
#add_subdirectory(tests)