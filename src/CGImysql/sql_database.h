#ifndef SQL_DATABASE_H
#define SQL_DATABASE_H
#include "common.h"
#include "sql_connection_pool.h"
#include <mysql/mysql.h>

class SqlDatabase {
public:
    explicit SqlDatabase(ConnectionPool& connectionPool);
    //查询操作
    MYSQL_RES* Query(const std::string& sql);
    //更新操作(insert, delete, update)
    bool Update(const std::string& sql);
  

private:
  ConnectionPool& connectionPool_; // 连接池引用
};

#endif
