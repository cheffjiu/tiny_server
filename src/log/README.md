# 日志模块

## 模块概述
日志模块是一个独立的、线程安全的日志记录系统，支持同步和异步两种日志记录方式。该模块采用单例模式设计，确保整个系统使用统一的日志记录实例。

## 功能特点
- 支持同步/异步日志记录
- 四种日志级别（INFO、WARN、ERROR、DEBUG）
- 线程安全的日志队列
- 自动日志文件管理
- 格式化日志输出

## 核心类
### Logger类
`Logger`类是日志模块的核心类，采用单例模式设计。

#### 主要成员
```cpp
class Logger {
  std::ofstream log_file_;                 // 日志文件
  std::atomic<bool> running_;              // 日志对象是否在运行
  std::queue<std::string> log_queue_;      // 日志队列
  std::thread log_thread_;                 // 日志写入线程
  std::mutex log_queue_mutex_;             // 日志队列互斥锁
  std::condition_variable log_queue_cond_; // 日志队列条件变量
  bool async_;                             // 是否异步写入日志
};
```

#### 关键接口
1. 获取日志实例
```cpp
static Logger& GetInstance(const std::string& log_file_name, bool async = true);
```
- `log_file_name`: 日志文件路径
- `async`: 是否使用异步模式（默认为true）

2. 记录日志
```cpp
void Log(LogLevel level, const std::string& message);
```
- `level`: 日志级别（INFO/WARN/ERROR/DEBUG）
- `message`: 日志消息内容

## 使用示例
```cpp
// 获取日志实例（异步模式）
auto& logger = Logger::GetInstance("server.log", true);

// 记录不同级别的日志
logger.Log(Logger::INFO, "Server started");
logger.Log(Logger::WARN, "Low memory warning");
logger.Log(Logger::ERROR, "Database connection failed");
logger.Log(Logger::DEBUG, "Processing request #1234");
```

## 工作流程
### 同步模式
1. 直接将日志消息写入文件
2. 适用于对实时性要求高的场景

### 异步模式
1. 日志消息首先进入队列
2. 专门的日志线程负责从队列中取出消息并写入文件
3. 提供更好的性能，避免IO操作阻塞主线程

## 依赖关系
- C++11及以上（使用了线程、互斥量等特性）
- 标准库组件：
  - `<thread>`
  - `<mutex>`
  - `<condition_variable>`
  - `<queue>`
  - `<fstream>`
  - `<atomic>`

## 注意事项
1. 异步模式下，程序退出时会等待所有日志写入完成
2. 日志文件路径需要具有写入权限
3. 建议在程序启动时初始化日志实例