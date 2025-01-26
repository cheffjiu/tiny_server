# Router模块

## 模块概述

Router模块是一个轻量级的HTTP路由管理模块，负责处理HTTP请求的路由分发和静态资源访问。该模块通过注册回调函数的方式实现灵活的路由管理，支持不同HTTP方法（GET、POST等）的请求处理。

## 模块架构

### 核心功能

1. **路由注册**
   - 支持注册不同HTTP方法的路由处理函数
   - 使用哈希表实现高效的路由查找
   - 支持动态路由注册

2. **请求处理**
   - 根据请求的方法和路径匹配对应的处理函数
   - 自动处理请求参数和响应
   - 支持自定义响应头和状态码

3. **静态资源访问**
   - 支持静态HTML文件的访问
   - 自动处理文件读取和错误处理
   - 支持设置资源根目录

### 关键类

#### Router类

主要负责路由的注册和请求的分发处理，核心接口包括：

```cpp
// 注册路由处理函数
void RegisterRouter(const std::string &path, const std::string &method,
                   RouterHandler handler);

// 处理HTTP请求
bool HandleRequest(const HttpRequest &request, HttpResponse &response) const;

// 初始化路由表
void InitRouter(UserManager& user_manager);
```

## 使用方法

### 1. 创建Router实例

```cpp
UserManager user_manager;
Router router(user_manager);
```

### 2. 注册路由

```cpp
// 注册POST请求处理
router.RegisterRouter("/login", "POST",
    [&](const HttpRequest &req, HttpResponse &resp) {
        // 处理登录请求
        std::string username = req.GetBody();
        std::string password = req.GetBody();
        
        if (user_manager.Login(username, password)) {
            resp.SetStatusCode("200 OK");
            resp.SetBody("Login successful");
        } else {
            resp.SetStatusCode("401 Unauthorized");
            resp.SetBody("Invalid username or password");
        }
    });
```

### 3. 处理请求

```cpp
HttpRequest request;
HttpResponse response;

if (router.HandleRequest(request, response)) {
    // 请求处理成功
} else {
    // 未找到匹配的路由
    response.SetStatusCode("404 Not Found");
}
```

## 注意事项

1. 路由处理函数使用引用捕获外部变量时，需确保变量生命周期长于路由处理函数
2. 静态资源访问需要正确设置资源根目录路径
3. 路由注册时，路径区分大小写
4. 建议在程序启动时完成所有路由的注册
5. 处理请求时注意检查返回值，确保请求被正确处理

## 配置说明

静态资源目录配置：
```cpp
std::string resource_path = "/path/to/resources/";
```

## 性能优化

1. 使用哈希表存储路由信息，确保O(1)的路由查找性能
2. 路由处理函数使用右值引用和移动语义，减少不必要的拷贝
3. 静态资源读取使用缓冲区优化，提高读取效率