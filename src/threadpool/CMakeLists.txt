add_library(lib_threadpool thread_pool.cpp)

set_target_properties(lib_threadpool
    PROPERTIES
    CXX_STANDARD 11
    CXX_STANDARD_REQUIRED ON
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib
)
target_link_libraries(lib_threadpool
    PUBLIC
    lib_log)
target_include_directories(lib_threadpool
    PUBLIC
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/src/log)
