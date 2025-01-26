#ifndef HTTP_CONN_H
#define HTTP_CONN_H
#include "common.h"

// http请求类
class HttpRequest {
public:
  // 解析http请求
  bool Parse(const std::string &raw_request);
  // 获取请求方法
  const std::string &GetMethod() const;
  // 获取请求路径
  const std::string &GetPath() const;
  // 获取http版本号
  const std::string &GetVersion()const;
  // 获取请求头
  const std::unordered_map<std::string, std::string> &GetHeaders() const;
  // 获取请求体
  const std::string &GetBody() const;

private:
  std::string method_;                                   // http请求方法
  std::string path_;    // 请求路径
  std::string version_;                  // http版本号               
  std::unordered_map<std::string, std::string> headers_; // 请求头
  std::string body_;                                     // 请求体
};

// http响应类
class HttpResponse {
public:
  // 设置状态码
  void SetStatusCode(const std::string &status_code);
  // 设置响应头
  void SetHeader(const std::string &key, const std::string &value);
  // 设置响应体
  void SetBody(const std::string &body);
  // 构造完整http响应
  std::string BuildHttpResponse() const;

private:
  std::string status_code_;                              // 状态码
  std::unordered_map<std::string, std::string> headers_; // 响应头
  std::string body_;                                     // 响应体
};

#endif