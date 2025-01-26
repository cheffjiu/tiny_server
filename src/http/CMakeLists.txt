add_library(lib_http STATIC http_conn.cpp)
set_target_properties(lib_http PROPERTIES
    CXX_STANDARD 11
    CXX_STANDARD_REQUIRED ON
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib)

target_include_directories(lib_http PUBLIC 
    ${CMAKE_SOURCE_DIR}/include 
    ${CMAKE_CURRENT_SOURCE_DIR})
