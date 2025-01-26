#include "http_conn.h"
/**
 * @brief 解析HTTP请求
 *
 * 解析传入的原始HTTP请求字符串，提取出请求行、请求头等信息。
 *
 * @param raw_request 原始HTTP请求字符串
 * @return 解析成功返回true，否则返回false
 */
bool HttpRequest::Parse(const std::string &raw_request) {
  std::istringstream stream(raw_request);
  std::string line;

  // 解析请求行
  if (getline(stream, line)) {
    std::istringstream line_stream(line);
    line_stream >> method_ >> path_ >> version_;
  } else {
    return false;
  }

  // 解析请求头
  while (getline(stream, line) && line != "\r") {
    auto colon_pos = line.find(":");
    std::string key_temp = line.substr(0, colon_pos);
    std::string value_temp = line.substr(colon_pos + 1);
    headers_[key_temp] = value_temp;
  }
  // 解析请求体
  body_ = std::string(std::istreambuf_iterator<char>(stream), {});
  return true;
}

/**
 * @brief 获取HTTP请求的方法
 *
 * 返回HTTP请求的方法，例如GET、POST等。
 *
 * @return HTTP请求的方法
 */
const std::string &HttpRequest::GetMethod() const { return method_; }

/**
 * @brief 获取HTTP请求的路径
 *
 * 返回HTTP请求的路径部分，例如"/index.html"。
 *
 * @return HTTP请求的路径
 */
const std::string &HttpRequest::GetPath() const { return path_; }

/**
 * @brief 获取HTTP请求的版本号
 *
 * 返回HTTP请求的版本号，例如HTTP/1.1。
 *
 * @return HTTP请求的版本号
 */
const std::string &HttpRequest::GetVersion() const { return version_; }

/**
 * @brief 获取HTTP请求的头部信息
 *
 * 返回HTTP请求的头部信息，以键值对的形式存储在一个无序映射中。
 *
 * @return 一个常量引用，指向包含HTTP请求头部信息的无序映射
 */
const std::unordered_map<std::string, std::string> &
HttpRequest::GetHeaders() const {
  return headers_;
}

/**
 * @brief 获取HTTP请求的请求体
 *
 * 返回HTTP请求的请求体内容。
 *
 * @return HTTP请求的请求体内容
 */
const std::string &HttpRequest::GetBody() const { return body_; }

/**
 * @brief 设置HTTP响应的状态码
 *
 * 该函数用于设置HTTP响应的状态码。
 *
 * @param status_code 状态码字符串，例如 "200 OK"
 */
void HttpResponse::SetStatusCode(const std::string &status_code) {
  status_code_ = status_code;
}

/**
 * @brief 设置HTTP响应头
 *
 * 将指定的键和值设置为HTTP响应头。
 *
 * @param key 要设置的HTTP响应头的键
 * @param value 要设置的HTTP响应头的值
 */
void HttpResponse::SetHeader(const std::string &key, const std::string &value) {
  headers_[key] = value;
}

/**
 * @brief 设置HTTP响应的主体内容
 *
 * 该函数用于设置HTTP响应的主体内容。
 *
 * @param body 响应主体的内容
 */
void HttpResponse::SetBody(const std::string &body) {
  // 将传入的主体内容赋值给body_成员变量
  body_ = body;
}


std::string HttpResponse::BuildHttpResponse()const{
  std::ostringstream response;
  response<<"HTTP/1.1"<<status_code_<<"\r\n";
  for(const auto&pair:headers_){
    response<<pair.first<<":"<<pair.second<<"\r\n";
  }
  response<<"\r\n"<<body_;
  return response.str();
}