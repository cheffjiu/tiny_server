add_library(lib_server server.cpp)

set_target_properties(lib_server PROPERTIES
    CXX_STANDARD 11
    CXX_STANDARD_REQUIRED ON
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib)

target_link_libraries(lib_server
PUBLIC
lib_router
lib_threadpool
lib_timer
lib_http)

target_include_directories(lib_server PUBLIC
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/src/log
    ${CMAKE_SOURCE_DIR}/src/timer
    ${CMAKE_SOURCE_DIR}/src/threadpool
    ${CMAKE_SOURCE_DIR}/src/router
    ${CMAKE_SOURCE_DIR}/src/CGImysql
    ${CMAKE_SOURCE_DIR}/src/http
    )