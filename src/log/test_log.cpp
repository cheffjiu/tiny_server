#include "logger.h"

int main() {
    Logger& logger = Logger::GetInstance();

    // 设置日志级别
    logger.SetLogLevel(Logger::INFO);

    // 记录日志
    logger.Log(Logger::INFO, "Server started.");
    logger.Log(Logger::DEBUG, "Debugging server initialization.");
    logger.Log(Logger::WARN, "Potential issue detected.");
    logger.Log(Logger::ERROR, "Critical error occurred!");

    std::this_thread::sleep_for(std::chrono::seconds(1)); 
    // 停止日志
    logger.Stop();
    std::cout<<"end"<<std::endl;
    return 0;
}
