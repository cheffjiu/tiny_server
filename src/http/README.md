# HTTP模块

## 模块概述
HTTP模块是一个轻量级的HTTP请求处理模块，主要负责HTTP请求的解析和响应的构建。该模块包含两个核心类：`HttpRequest`和`HttpResponse`，分别用于处理HTTP请求和响应。

## 模块架构

### HttpRequest类
`HttpRequest`类负责解析和存储HTTP请求信息，包括：
- 请求方法（GET、POST等）
- 请求路径
- HTTP版本
- 请求头
- 请求体

### HttpResponse类
`HttpResponse`类负责构建HTTP响应，包括：
- 状态码
- 响应头
- 响应体

## 接口说明

### HttpRequest类接口

```cpp
// 解析HTTP请求
bool Parse(const std::string &raw_request);

// 获取请求方法
const std::string &GetMethod() const;

// 获取请求路径
const std::string &GetPath() const;

// 获取HTTP版本
const std::string &GetVersion() const;

// 获取请求头
const std::unordered_map<std::string, std::string> &GetHeaders() const;

// 获取请求体
const std::string &GetBody() const;
```

### HttpResponse类接口

```cpp
// 设置状态码
void SetStatusCode(const std::string &status_code);

// 设置响应头
void SetHeader(const std::string &key, const std::string &value);

// 设置响应体
void SetBody(const std::string &body);

// 构建完整HTTP响应
std::string BuildHttpResponse() const;
```

## 使用示例

```cpp
// 解析HTTP请求
HttpRequest request;
std::string raw_request = "GET /index.html HTTP/1.1\r\n"
                         "Host: localhost:8080\r\n"
                         "\r\n";
if (request.Parse(raw_request)) {
    // 获取请求信息
    std::string method = request.GetMethod();  // "GET"
    std::string path = request.GetPath();      // "/index.html"
}

// 构建HTTP响应
HttpResponse response;
response.SetStatusCode("200 OK");
response.SetHeader("Content-Type", "text/html");
response.SetBody("<html><body>Hello World!</body></html>");

// 获取完整的HTTP响应字符串
std::string http_response = response.BuildHttpResponse();
```

## 注意事项
1. `Parse`方法会解析原始HTTP请求字符串，确保输入的请求格式正确
2. 所有的getter方法都返回const引用，以提高性能
3. `BuildHttpResponse`方法会自动添加必要的HTTP头部分隔符（\r\n）
4. 状态码应该包含状态描述，例如"200 OK"，"404 Not Found"等