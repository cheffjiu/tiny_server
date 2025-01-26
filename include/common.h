
#ifndef COMMON_H
#define COMMON_H

#include <chrono>
#include <ctime>
#include <iomanip>
#include <memory>
#include <unistd.h>

#include <arpa/inet.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <sys/epoll.h>
#include <sys/socket.h>

#include <fstream>
#include <iostream>
#include <sstream>

#include <cstring>
#include <functional>
#include <queue>
#include <string>
#include <vector>
#include<unordered_map>

#include <atomic>
#include <condition_variable>
#include <mutex>
#include <thread>
#include<future>

constexpr char LOGFILE[] = "webserver.log";// 定义日志文件名，类型是char[]，在使用时，会自动转为const
                                           // string&
constexpr int MAX_EVENTS=1024;
constexpr int BUFFER_SIZE=1024;
#endif