#ifndef USER_MANAGER_H
#define USER_MANAGER_H
#include "common.h"
#include "sql_database.h"
class UserManager{
    public:
    explicit UserManager(SqlDatabase & db_opreations);
    //用户注册
    bool Register(const std::string & username, const std::string & password);
    //用户登录
    bool Login(const std::string & username, const std::string & password);
    private:
         SqlDatabase& db_opreations_;//数据库操作对象
         
};
#endif