cmake_minimum_required(VERSION 3.22)

project(TinyServer)

# 设置编译选项
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")

# 根据系统架构设置MySQL路径
if(CMAKE_SYSTEM_PROCESSOR MATCHES "x86_64")
    set(MYSQL_LIBRARY "/usr/lib/x86_64-linux-gnu/libmysqlclient.so")
elseif(CMAKE_SYSTEM_PROCESSOR MATCHES "aarch64")
    set(MYSQL_LIBRARY "/usr/lib/aarch64-linux-gnu/libmysqlclient.so")
else()
    message(FATAL_ERROR "Unsupported architecture: ${CMAKE_SYSTEM_PROCESSOR}")
endif()

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

# 设置输出目录
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# 添加子目录
add_subdirectory(src)

# 安装配置
install(DIRECTORY ${CMAKE_SOURCE_DIR}/resource/web
        DESTINATION share/${PROJECT_NAME}
        FILES_MATCHING PATTERN "*.html" PATTERN "*.css" PATTERN "*.js")

install(DIRECTORY ${CMAKE_SOURCE_DIR}/resource
        DESTINATION share/${PROJECT_NAME}
        FILES_MATCHING PATTERN "*.jpg" PATTERN "*.mp4")

# 启用测试（可选）
#enable_testing()
#add_subdirectory(tests)