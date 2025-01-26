#find_package(MySQL REQUIRED)

add_library(lib_sql_connection_pool sql_connection_pool.cpp)
set_target_properties(lib_sql_connection_pool PROPERTIES
    CXX_STANDARD 11
    CXX_STANDARD_REQUIRED ON
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib
)

target_link_libraries(lib_sql_connection_pool 
    mysqlclient
    lib_log
)

target_include_directories(lib_sql_connection_pool PUBLIC
    ${MySQL_INCLUDE_DIR}
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/src/log
)

add_library(lib_sql_database sql_database.cpp)
set_target_properties(lib_sql_database PROPERTIES
    CXX_STANDARD 11
    CXX_STANDARD_REQUIRED ON
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib
)

target_link_libraries(lib_sql_database lib_sql_connection_pool)
target_include_directories(lib_sql_database PUBLIC
    ${MySQL_INCLUDE_DIR}
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}
)

add_library(lib_user_manager user_manager.cpp)
set_target_properties(lib_user_manager PROPERTIES
    CXX_STANDARD 11
    CXX_STANDARD_REQUIRED ON
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib
)

target_link_libraries(lib_user_manager lib_sql_database)
target_include_directories(lib_user_manager PUBLIC
    ${MySQL_INCLUDE_DIR}
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}
)

