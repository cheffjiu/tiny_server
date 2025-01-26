# TinyServer

TinyServer 是一个基于 C++11 实现的轻量级 Web 服务器，支持 HTTP 请求处理、数据库连接池、定时器和日志系统等功能。

## 功能特性

- 使用 Epoll 边缘触发的 I/O 多路复用技术
- 基于线程池的并发处理模型
- MySQL 数据库连接池
- 基于优先队列的定时器，处理非活动连接
- 异步日志系统
- 支持 HTTP GET 和 POST 请求
- 支持静态资源访问
- 支持用户注册和登录功能

## 开发环境

- Ubuntu 22.04 
- C++11
- MySQL 8.0
- CMake 3.22

## 构建和运行

1. 安装必要的依赖：
   ```bash
   # Ubuntu
   sudo apt-get install cmake g++ mysql-server libmysqlclient-dev
   ```

2. 克隆项目：
   ```bash
   git clone https://github.com/cheffjiu/tiny_server.git
   cd tiny_server
   ```
3. 配置数据库：
    在你的 MySQL 数据库中创建一个名为 `webserver` 的数据库，执行以下 SQL 语句：

    ```sql
    CREATE DATABASE webserver;
    USE webserver;
    CREATE TABLE users (
        id INT AUTO_INCREMENT PRIMARY KEY,
        username VARCHAR(255) UNIQUE NOT NULL,
        password VARCHAR(255) NOT NULL
    );
    ```
4. 构建项目：
   ```bash
   mkdir build && cd build
   cmake ..
   make
   ```
5. 运行项目：
   在main.cpp中修改数据库连接信息
   ```bash
   ./bin/main
   ```

6. 访问服务器：
   在浏览器中访问 `URL_ADDRESS:8080`/login.html 或 `URL_ADDRESS:8080/register.html` 进行注册或登录。
## 项目结构

```
- /src            # 源代码目录
  - /CGImysql     # 数据库连接池和用户管理
  - /http         # HTTP 请求处理
  - /log          # 日志系统
  - /router       # 路由管理
  - /server       # 服务器核心
  - /threadpool   # 线程池
  - /timer        # 定时器
- /tests          # 单元测试（使用GoogleTest，上传项目以及删除测试代码）
- /resource       # 静态资源文件
```

## 性能测试

- 并发连接数：10000+
- QPS：10000+

## 待完善功能

- HTTPS 支持
- 完善单元测试
- 配置文件支持
- 会话管理
- 更多 HTTP 方法支持

## 贡献指南

欢迎提交 Issue 和 Pull Request。

## 许可证

该项目采用 MIT 许可证。详情请参见 LICENSE 文件。