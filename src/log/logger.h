#ifndef LOGGER_H
#define LOGGER_H
#include "common.h"
class Logger {
public:
  // 日志级别
  enum LogLevel { INFO, WARN, ERROR, DEBUG };

  // 单例模式获取实例
  static Logger &GetInstance(const std::string &log_file_name,
                             bool async = true);

  Logger() = delete;

  // 禁用拷贝构造和赋值
  Logger(const Logger &) = delete;
  Logger &operator=(const Logger &) = delete;

  // 记录日志
  void Log(LogLevel level, const std::string &message);

private:
  Logger(const std::string &log_file_name, bool async = true);
  ~Logger();

  void AsyncWriteLog();
  std::string FormatLog(LogLevel level, const std::string &message);

  std::ofstream log_file_;                 // 日志文件
  std::atomic<bool> running_;              // 日志对象是否在运行
  std::queue<std::string> log_queue_;      // 日志队列
  std::thread log_thread_;                 // 日志写入线程
  std::mutex log_queue_mutex_;             // 日志队列互斥锁
  std::condition_variable log_queue_cond_; // 日志队列条件变量
  bool async_;                             // 是否异步写入日志
};

#endif