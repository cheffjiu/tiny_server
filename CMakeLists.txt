cmake_minimum_required(VERSION 3.22)

project(TinyServer)

# 检测系统架构并设置MySQL库路径
if(CMAKE_SYSTEM_PROCESSOR MATCHES "x86_64")
    set(MYSQL_LIBRARY "/usr/lib/x86_64-linux-gnu/libmysqlclient.so")
elseif(CMAKE_SYSTEM_PROCESSOR MATCHES "aarch64")
    set(MYSQL_LIBRARY "/usr/lib/aarch64-linux-gnu/libmysqlclient.so")
else()
    # 尝试其他常见路径
    if(EXISTS "/usr/lib/libmysqlclient.so")
        set(MYSQL_LIBRARY "/usr/lib/libmysqlclient.so")
    elseif(EXISTS "/usr/local/mysql/lib/libmysqlclient.so")
        set(MYSQL_LIBRARY "/usr/local/mysql/lib/libmysqlclient.so")
    else()
        message(FATAL_ERROR "Could not find MySQL library in common locations")
    endif()
endif()

# 设置MySQL头文件路径
set(MYSQL_INCLUDE_DIR "/usr/include/mysql")
if(NOT EXISTS ${MYSQL_INCLUDE_DIR})
    if(EXISTS "/usr/local/mysql/include")
        set(MYSQL_INCLUDE_DIR "/usr/local/mysql/include")
    endif()
endif()

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