#include "router.h"


Router::Router(UserManager &user_manager)
    : user_manager_(user_manager), logger_(Logger::GetInstance(LOGFILE)),
      resource_path_("../../resource/web/") {
  InitRouter(user_manager);
}

void Router::RegisterRouter(const std::string &path, const std::string &method,
                          RouterHandler handler) {
  RouterKey key{method, path};
  routes_[key] = std::move(handler);
  logger_.Log(Logger::INFO, "Register router: " + method + " " + path);
}

bool Router::HandleRequest(const HttpRequest &request,
                         HttpResponse &response) const {
  RouterKey key{request.GetMethod(), request.GetPath()};
  auto it = routes_.find(key);

  if (it != routes_.end()) {
    it->second(request, response);
    return true;
  }

  // 处理静态资源请求
  if (request.GetMethod() == "GET") {
    std::string file_path = resource_path_ + request.GetPath();
    std::string content = ReadHtmlFile(file_path);
    if (!content.empty()) {
      response.SetStatusCode("200 OK");
      response.SetHeader("Content-Type", "text/html; charset=utf-8");
      response.SetBody(content);
      return true;
    }
  }

  return false;
}

void Router::InitRouter(UserManager &user_manager) {
  // 注册登录路由
  RegisterRouter("/login", "POST",
                [&](const HttpRequest &req, HttpResponse &resp) {
                  // 从请求体中解析用户名和密码
                  std::string username = req.GetBody();
                  std::string password = req.GetBody();
                  
                  if (user_manager.Login(username, password)) {
                    resp.SetStatusCode("200 OK");
                    resp.SetHeader("Content-Type", "text/plain; charset=utf-8");
                    resp.SetBody("Login successful");
                  } else {
                    resp.SetStatusCode("401 Unauthorized");
                    resp.SetHeader("Content-Type", "text/plain; charset=utf-8");
                    resp.SetBody("Invalid username or password");
                  }
                });

  // 注册注册路由
  RegisterRouter("/register", "POST",
                [&](const HttpRequest &req, HttpResponse &resp) {
                  // 从请求体中解析用户名和密码
                  std::string username = req.GetBody();
                  std::string password = req.GetBody();

                  if (user_manager.Register(username, password)) {
                    resp.SetStatusCode("200 OK");
                    resp.SetHeader("Content-Type", "text/plain; charset=utf-8");
                    resp.SetBody("Registration successful");
                  } else {
                    resp.SetStatusCode("400 Bad Request");
                    resp.SetHeader("Content-Type", "text/plain; charset=utf-8");
                    resp.SetBody("Username already exists");
                  }
                });
}

std::string Router::ReadHtmlFile(const std::string &filename) const {
  std::ifstream file(filename);
  if (!file.is_open()) {
    logger_.Log(Logger::ERROR, "Failed to open file: " + filename);
    return "";
  }

  std::stringstream buffer;
  buffer << file.rdbuf();
  logger_.Log(Logger::DEBUG, "Successfully read file: " + filename);
  return buffer.str();
}