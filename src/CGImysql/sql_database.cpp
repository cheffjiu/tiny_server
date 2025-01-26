#include "sql_database.h"
#include "logger.h"

SqlDatabase::SqlDatabase(ConnectionPool &pool) : connectionPool_(pool) {}

MYSQL_RES *SqlDatabase::Query(const std::string &sql) {
    auto conn=connectionPool_.GetConnection();
    auto& logger=Logger::GetInstance(LOGFILE);
    if(!conn){
        logger.Log(Logger::LogLevel::ERROR,"Failed to get connection from pool");
        return nullptr;
    }
    if (mysql_query(conn.get(),sql.c_str())){
        logger.Log(Logger::LogLevel::ERROR,"Failed to execute query");
        return nullptr;
    }
    return mysql_store_result(conn.get());
}

bool SqlDatabase::Update(const std::string &sql) {
    auto conn=connectionPool_.GetConnection();
    auto& logger=Logger::GetInstance(LOGFILE);
    if(!conn){
        logger.Log(Logger::LogLevel::ERROR,"Failed to get connection from pool");
        return false;
    }
    if (mysql_query(conn.get(),sql.c_str())){
        logger.Log(Logger::LogLevel::ERROR,"Failed to execute query");
        return false;
    }
    return true;
}