#include "logger.h"

Logger::Logger(const std::string &log_file_name, bool async)
    : running_(true), async_(async) {
  log_file_.open(log_file_name, std::ios::app);
  if (!log_file_.is_open()) {
    throw std::runtime_error("Failed to open log file: " + log_file_name);
  }
  if (async_) {
    log_thread_ = std::thread(&Logger::AsyncWriteLog, this);
  }
}

Logger &Logger::GetInstance(const std::string &log_file_name, bool async) {
  static Logger instance(log_file_name, async);
  return instance;
}

Logger::~Logger() {
  running_.store(false);
  log_queue_cond_.notify_all();
  if (async_ && log_thread_.joinable()) {
    log_thread_.join();
  }
  log_file_.close();
}

std::string Logger::FormatLog(LogLevel level, const std::string &message) {
  std::ostringstream oss;
  auto now = std::chrono::system_clock::now();
  auto time = std::chrono::system_clock::to_time_t(now);
  oss << "[" << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S")
      << "] ";
  switch (level) {
  case INFO:
    oss << "[INFO] ";
    break;
  case WARN:
    oss << "[WARN] ";
    break;
  case ERROR:
    oss << "[ERROR] ";
    break;
  case DEBUG:
    oss << "[DEBUG] ";
    break;
  }
  oss << message;
  return oss.str();
}

void Logger::Log(LogLevel level, const std::string &message) {
  std::string format_log = FormatLog(level, message);
  if (async_) {
    std::unique_lock<std::mutex> lock(log_queue_mutex_);
    log_queue_.push(format_log);
    log_queue_cond_.notify_one();
  } else {
    log_file_ << format_log << std::endl;
  }
}

void Logger::AsyncWriteLog(){
  while(running_.load()){
    std::unique_lock<std::mutex> lock(log_queue_mutex_);
    log_queue_cond_.wait(lock,[this](){
      return !log_queue_.empty()||!running_.load();
    });
    while(!log_queue_.empty()){
      log_file_<<log_queue_.front()<<std::endl;
      log_queue_.pop();
    }
  }
}