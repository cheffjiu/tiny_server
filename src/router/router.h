#ifndef ROUTER_H
#define ROUTER_H

#include "common.h"
#include "logger.h"
#include "user_manager.h"
#include "http_conn.h"

class Router {
public:
  using RouterHandler =
      std::function<void(const HttpRequest &, HttpResponse &)>;

  Router(UserManager &user_manager);
  // 注册路由
  void RegisterRouter(const std::string &path, const std::string &method,
                      RouterHandler handler);
  // 分发请求
  bool HandleRequest(const HttpRequest &request, HttpResponse &response) const;
  // 初始化所有路由
  void InitRouter(UserManager& user_manager);

private:
  // 读取HTML文件的辅助函数
  std::string ReadHtmlFile(const std::string &filename) const;
  struct RouterKey {
    std::string method_;
    std::string path_;
    bool operator==(const RouterKey &other) const {
      return method_ == other.method_ && path_ == other.path_;
    }
  };
  struct RouterKeyHash {
    std::size_t operator()(const RouterKey &key) const {
      return std::hash<std::string>()(key.method_ + key.path_);
    }
  };

  std::unordered_map<RouterKey, RouterHandler, RouterKeyHash> routes_;
  UserManager &user_manager_;
  Logger &logger_;
  std::string resource_path_;
};
#endif