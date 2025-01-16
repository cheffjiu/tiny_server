# tests/CMakeLists.txt

# 为测试创建可执行文件
add_executable(test_log test_log.cpp)

#设置可执行文件的输出路径
set_target_properties(test_log PROPERTIES
   CXX_STANDARD 11
   CXX_STANDARD_REQUIRED ON
   RUNTIME_OUTPUT_DIRECTORY
   ${CMAKE_BINARY_DIR}/tests/bin)

# 查找 GoogleTest 包
find_package(GTest REQUIRED)

# 链接 GoogleTest 和 pthread 库
target_link_libraries(test_log lib_log ${GTEST_LIBRARIES} pthread)

#包含头文件
# 为测试目标添加 GoogleTest 的头文件路径
target_include_directories(test_log PRIVATE
   ${GTEST_INCLUDE_DIRS}
   ${CMAKE_SOURCE_DIR}/src/log
   ${CMAKE_SOURCE_DIR}/include
)

