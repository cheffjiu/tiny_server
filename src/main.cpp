#include <iostream>
#include <fstream>
#include <string>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <memory>
#include <atomic>

// 日志级别定义
enum class LogLevel {
    DEBUG,
    INFO,
    WARNING,
    ERROR
};

// 工具函数：将日志级别转换为字符串
std::string logLevelToString(LogLevel level) {
    switch (level) {
        case LogLevel::DEBUG: return "[DEBUG]";
        case LogLevel::INFO: return "[INFO]";
        case LogLevel::WARNING: return "[WARNING]";
        case LogLevel::ERROR: return "[ERROR]";
        default: return "[UNKNOWN]";
    }
}

// 抽象基类：日志输出接口
class ILogOutput {
public:
    virtual ~ILogOutput() = default;
    virtual void write(const std::string& message) = 0;
};

// 控制台日志输出
class ConsoleLogOutput : public ILogOutput {
public:
    void write(const std::string& message) override {
        std::cout << message << std::endl;
    }
};

// 文件日志输出
class FileLogOutput : public ILogOutput {
private:
    std::ofstream outFile;
public:
    explicit FileLogOutput(const std::string& filePath) {
        outFile.open(filePath, std::ios::app);
        if (!outFile.is_open()) {
            throw std::runtime_error("Failed to open log file: " + filePath);
        }
    }

    ~FileLogOutput() {
        if (outFile.is_open()) {
            outFile.close();
        }
    }

    void write(const std::string& message) override {
        if (outFile.is_open()) {
            outFile << message << std::endl;
        }
    }
};

// 日志管理类（单例模式）
class Logger {
public:
    static Logger& getInstance() {
        static Logger instance;
        return instance;
    }

    // 禁用拷贝构造和赋值运算符
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;

    // 设置日志输出目标
    void setOutput(std::shared_ptr<ILogOutput> output) {
        this->output = output;
    }

    // 记录同步日志
    void logSync(LogLevel level, const std::string& message) {
        std::string logMessage = formatLogMessage(level, message);
        if (output) {
            output->write(logMessage);
        }
    }

    // 记录异步日志
    void logAsync(LogLevel level, const std::string& message) {
        {
            std::lock_guard<std::mutex> lock(queueMutex);
            logQueue.push(formatLogMessage(level, message));
        }
        condVar.notify_one();
    }

    // 停止异步日志线程
    void stopAsyncLogging() {
        {
            std::lock_guard<std::mutex> lock(queueMutex);
            stop = true;
        }
        condVar.notify_all();
        if (asyncThread.joinable()) {
            asyncThread.join();
        }
    }

private:
    std::shared_ptr<ILogOutput> output;
    std::queue<std::string> logQueue;
    std::mutex queueMutex;
    std::condition_variable condVar;
    std::atomic<bool> stop;
    std::thread asyncThread;

    Logger() : stop(false) {
        asyncThread = std::thread(&Logger::asyncLogWorker, this);
    }

    ~Logger() {
        stopAsyncLogging();
    }

    // 异步日志工作线程
    void asyncLogWorker() {
        while (true) {
            std::string logMessage;
            {
                std::unique_lock<std::mutex> lock(queueMutex);
                condVar.wait(lock, [this] { return !logQueue.empty() || stop; });

                if (stop && logQueue.empty()) {
                    break;
                }

                logMessage = logQueue.front();
                logQueue.pop();
            }

            if (output) {
                output->write(logMessage);
            }
        }
    }

    // 格式化日志消息
    std::string formatLogMessage(LogLevel level, const std::string& message) {
        return logLevelToString(level) + " " + message;
    }
};

int main() {
    try {
        // 获取日志单例实例
        Logger& logger = Logger::getInstance();

        // 设置日志输出到文件
        logger.setOutput(std::make_shared<FileLogOutput>("log.txt"));

        // 记录同步日志
        logger.logSync(LogLevel::INFO, "This is an INFO log.");
        logger.logSync(LogLevel::ERROR, "This is an ERROR log.");

        // 切换日志输出到控制台
        logger.setOutput(std::make_shared<ConsoleLogOutput>());

        // 记录异步日志
        logger.logAsync(LogLevel::DEBUG, "This is a DEBUG log.");
        logger.logAsync(LogLevel::WARNING, "This is a WARNING log.");

        std::this_thread::sleep_for(std::chrono::seconds(1)); // 等待异步线程完成写入

        // 停止异步日志线程
        logger.stopAsyncLogging();
    } catch (const std::exception& ex) {
        std::cerr << "Error: " << ex.what() << std::endl;
    }

    return 0;
}
