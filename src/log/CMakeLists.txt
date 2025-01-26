add_library(lib_log STATIC logger.cpp)

set_target_properties(lib_log PROPERTIES
    CXX_STANDARD 11
    CXX_STANDARD_REQUIRED ON
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib)

target_include_directories(lib_log PUBLIC 
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR})

