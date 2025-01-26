
add_subdirectory(log)

add_subdirectory(threadpool)

add_subdirectory(timer)

add_subdirectory(CGImysql)

add_subdirectory(http)

add_subdirectory(router)

add_subdirectory(server)

# #find_package(MySQL REQUIRED)

add_executable(main main.cpp)
set_target_properties(main PROPERTIES
    CXX_STANDARD 11
    CXX_STANDARD_REQUIRED ON
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
target_link_libraries(main
    lib_http
    lib_router
    lib_log
    lib_sql_connection_pool
    lib_timer
    lib_threadpool
    lib_sql_database
    lib_user_manager
    lib_server
    mysqlclient
    pthread
)
target_include_directories(main PUBLIC
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/src
    ${MYSQL_INCLUDE_DIR}
)

