#include "user_manager.h"
#include "logger.h"

UserManager::UserManager(SqlDatabase &dbop) : db_opreations_(dbop) {}

/**
 * @brief 注册用户
 *
 * 在数据库中注册一个新用户。
 *
 * @param username 用户名
 * @param password 用户密码
 *
 * @return 如果注册成功返回 true，否则返回 false
 */
bool UserManager::Register(const std::string &username,
                           const std::string &password) {

  // 检测用户是否已经存在
  std::string check_query =
      "SELECT 1 FROM users WHERE username='" + username + "'";

  MYSQL_RES *res = db_opreations_.Query(check_query);
  Logger &logger = Logger::GetInstance(LOGFILE);
  if (res) {
    // 判断用户是否已存在
    bool exits = (mysql_fetch_row(res) != nullptr);
    mysql_free_result(res);
    if (exits) {
      // 用户已存在，记录日志并返回false
      logger.Log(Logger::LogLevel::WARN, "用户已存在");
      return false;
    }
  }

  // 构建插入用户的SQL语句
  std::string insert_query =
      "INSERT INTO users (username, password) VALUES ('" + username + "', '" +
      password + "')";
  // 执行插入操作
  if (db_opreations_.Update(insert_query)) {
    // 注册成功，记录日志并返回true
    logger.Log(Logger::LogLevel::INFO, "注册成功");
    return true;
  } else {
    // 注册失败，记录日志并返回false
    logger.Log(Logger::LogLevel::ERROR, "注册失败");
    return false;
  }
}

/**
 * @brief 用户登录
 *
 * 根据用户名和密码进行用户登录验证。
 *
 * @param username 用户名
 * @param password 密码
 *
 * @return 登录成功返回true，登录失败返回false
 */
bool UserManager::Login(const std::string &username,
                        const std::string &password) {

  // 构造SQL查询语句
  std::string query = "SELECT 1 FROM users WHERE username='" + username +
                      "' AND password='" + password + "'";

  // 执行查询
  MYSQL_RES *res = db_opreations_.Query(query);
  Logger &logger = Logger::GetInstance(LOGFILE);
  // 检查查询结果是否为空
  if (!res) {
    // 如果查询结果为空，则记录错误日志并返回登录失败
    logger.Log(Logger::LogLevel::ERROR, "登录失败");
    return false;
  }

  // 获取查询结果
  MYSQL_ROW row = mysql_fetch_row(res);
  bool valid = false;
  // 判断查询结果是否有效
  if (row != nullptr) {
    // 如果查询结果有效，则验证密码
    valid = (password == row[0]);
  }
  // 释放查询结果
  mysql_free_result(res);
  // 判断验证结果
  if (valid) {
    // 如果验证通过，则记录登录成功日志并返回登录成功
    logger.Log(Logger::LogLevel::INFO, "登录成功");
    return true;
  } else {
    // 如果验证失败，则记录登录失败日志并返回登录失败
    logger.Log(Logger::LogLevel::ERROR, "登录失败");
    return false;
  }
}