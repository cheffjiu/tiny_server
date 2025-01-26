#include "server.h"

Server::Server(const std::string &ip, int port, UserManager &user_manager,
               size_t thread_count)
    : ip_(ip), port_(port),  thread_pool_(thread_count),
      timer_([this](std::function<void()> task) {
        thread_pool_.EnqueueTask(std::move(task));
      }),router_(user_manager),
      logger_(Logger::GetInstance(LOGFILE)) {
  InitSocket();
  // 添加一个测试定时任务
  // timer_.AddTimer([this]() {
  //   logger_.Log(Logger::DEBUG, "Timer task executed");
  //    // 添加一些实际的工作来验证任务是否执行
  //   std::this_thread::sleep_for(std::chrono::milliseconds(100));
  //   logger_.Log(Logger::DEBUG, "Timer task completed");
  // }, 5000, true);  // 每5秒执行一次
  logger_.Log(Logger::INFO,
              "Server init success on" + ip_ + " " + std::to_string(port_));
}

Server::~Server() { close(server_fd_); }

void Server::InitSocket() {
  server_fd_ = socket(AF_INET, SOCK_STREAM, 0);
  int opt = 1;
  setsockopt(server_fd_, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
  sockaddr_in server_addr{};
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(port_);
  inet_pton(AF_INET, ip_.c_str(), &server_addr.sin_addr);
  bind(server_fd_, reinterpret_cast<sockaddr *>(&server_addr),
       sizeof(server_addr));
  listen(server_fd_, SOMAXCONN);
}

void Server::Start() { EventLoop(); }

void Server::EventLoop() {
  int epoll_fd = epoll_create1(0);
  epoll_event event{}, events[MAX_EVENTS];
  event.events = EPOLLIN;
  event.data.fd = server_fd_;
  epoll_ctl(epoll_fd, EPOLL_CTL_ADD, server_fd_, &event);
  while (true) {
    int epoll_count = epoll_wait(epoll_fd, events, MAX_EVENTS, -1);
    for (int i = 0; i < epoll_count; ++i) {
      if (events[i].data.fd == server_fd_) {
        int client_fd = accept(server_fd_, nullptr, nullptr);
        epoll_event client_event{};
        client_event.events = EPOLLIN | EPOLLET;
        client_event.data.fd = client_fd;
        epoll_ctl(epoll_fd, EPOLL_CTL_ADD, client_fd, &client_event);
      } else {
        int client_fd = events[i].data.fd;
        thread_pool_.EnqueueTask(
            [this, client_fd]() { HandleClient(client_fd); });
      }
    }
  }
}

void Server::HandleClient(int fd) {
  char buffer[BUFFER_SIZE] = {0};
  ssize_t bytes_read = read(fd, buffer, sizeof(buffer));
  if (bytes_read <= 0) {
    logger_.Log(Logger::ERROR,"Failed to read from client");
    close(fd);
    return;
  }
 // 输出请求内容
  logger_.Log(Logger::DEBUG, "Request: " + std::string(buffer));
  HttpRequest request;
  if (!request.Parse(buffer)) {
    close(fd);
    return;
  }
// 添加请求信息日志
  logger_.Log(Logger::DEBUG, "Method: " + request.GetMethod());
  logger_.Log(Logger::DEBUG, "Path: " + request.GetPath());
  logger_.Log(Logger::DEBUG, "Version: " + request.GetVersion());



//输出请求路径
logger_.Log(Logger::DEBUG, "Request path: " + request.GetPath());
  HttpResponse response;
  if (!router_.HandleRequest(request, response)) {
    response.SetStatusCode("404 Not Found");
    response.SetHeader("Content-Type", "text/plain; charset=utf-8");
    response.SetBody("Path Not Found");
  }

  std::string response_str = response.BuildHttpResponse();
  logger_.Log(Logger::DEBUG, "Response: " + response_str);
  ssize_t bytes_sent = send(fd, response_str.c_str(), response_str.size(), 0);
  if (bytes_sent <= 0) {
    logger_.Log(Logger::ERROR, "Failed to send response");
  }
}

Router &Server::GetRouter() { return router_; }
