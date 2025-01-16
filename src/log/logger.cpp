#include "logger.h"
Logger::Logger() : running_(true), log_level_(INFO) {
  worker_ = std::thread(&Logger::LogThread, this); // 创建日志写入线程
}
Logger::~Logger() {
  if (running_) {
    Stop();
  }
  if (worker_.joinable()) {
    worker_.join();
  }
}

Logger &Logger::GetInstance() {
  static Logger logger;
  return logger;
}

void Logger::SetLogLevel(LogLevel level) { log_level_ = level; }

void Logger::Log(LogLevel level, const std::string &message) {
  if (level >= log_level_) {
    std::ostringstream log_stream;

    // 获取当前时间
    std::time_t now = std::time(nullptr);
    char time_buffer[20];
    std::strftime(time_buffer, sizeof(time_buffer), "%Y-%m-%d %H:%M:%S",
                  std::localtime(&now));

    // 将日志信息写入字符串流
    log_stream << "[" << time_buffer << "]["
               << (level == INFO    ? "INFO"
                   : level == WARN  ? "WARN"
                   : level == ERROR ? "ERROR"
                                    : "DEBUG")
               << "]" << message;

    // 将日志信息加入队列
    log_queue_.push(log_stream.str());
  }
}
void Logger::Stop() {
  running_ = false;
  log_queue_.push(""); // 向队列中添加一个空字符串，用于唤醒日志写入线程
  if (worker_.joinable()) {
    worker_.join();
  }
}
void Logger::LogThread() {
  std::ofstream log_file("server.log",std::ios::app);
  if(!log_file.is_open()){
    std::cerr<<"Failed to open log file"<<std::endl;
    return;
  }
  while(running_){
    std::string log_message=log_queue_.pop();
    if(!running_&&log_message.empty()){
      break;
    }
    log_file<<log_message<<std::endl;
    log_file.flush();
  }
  log_file.close();
}