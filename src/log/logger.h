#pragma once
#ifndef LOGGER_H
#define LOGGER_H

#include "block_queue.h"
#include "common.h"
class Logger {
public:
  // 日志级别
  enum LogLevel { INFO, WARN, ERROR, DEBUG };

  // 单例模式获取实例
  static Logger &GetInstance();

  // 禁用拷贝构造和赋值
  Logger(const Logger &) = delete;

  Logger &operator=(const Logger &) = delete;

  // 设置日志级别
  void SetLogLevel(LogLevel level);

  // 记录日志
  void Log(LogLevel level, const std::string &message);

  // 停止日志线程
  void Stop();

private:
  Logger();
  ~Logger();
  // 日志写入线程
  void LogThread();

  std::atomic<bool> running_;         // 日志线程是否运行
  BlockQueue<std::string> log_queue_; // 日志队列
  std::thread worker_;                // 日志写入线程
  LogLevel log_level_;                // 日志级别

  // 测试接口
// public:
//   int getLogLevel() const { return log_level_; }
//   bool isRunning() const { return running_; }
};
#endif