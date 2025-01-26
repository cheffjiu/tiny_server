# CGImysql 模块

## 模块架构
CGImysql模块是TinyServer的数据库管理模块，主要包含三个核心组件：

1. 数据库连接池 (sql_connection_pool)
2. 数据库操作接口 (sql_database)
3. 用户管理系统 (user_manager)

## 功能说明

### 1. 数据库连接池 (sql_connection_pool)
- 管理MySQL数据库连接的生命周期
- 实现连接的动态分配和回收
- 支持连接池大小的配置
- 使用RAII机制确保连接资源的安全释放

主要接口：
```cpp
// 获取连接池实例
SQLConnectionPool* GetInstance();
// 获取一个数据库连接
MYSQL* GetConnection();
// 释放连接回连接池
void ReleaseConnection(MYSQL* conn);
```

### 2. 数据库操作接口 (sql_database)
- 封装常用的数据库操作
- 提供事务支持
- 支持参数化查询
- 异常处理机制

主要接口：
```cpp
// 执行SQL查询
bool Query(const std::string& sql);
// 获取查询结果
MYSQL_RES* GetResult();
// 开始事务
bool StartTransaction();
// 提交事务
bool Commit();
// 回滚事务
bool Rollback();
```

### 3. 用户管理系统 (user_manager)
- 用户注册和登录功能
- 用户信息的增删改查
- 密码加密存储
- 会话管理

主要接口：
```cpp
// 用户注册
bool Register(const std::string& username, const std::string& password);
// 用户登录
bool Login(const std::string& username, const std::string& password);
// 检查用户是否存在
bool CheckUser(const std::string& username);
```

## 使用示例

### 连接池配置和初始化
```cpp
// 初始化连接池
auto pool = SQLConnectionPool::GetInstance();
pool->init("localhost", "root", "password", "webserver", 3306, 8);

// 获取连接
MYSQL* conn = pool->GetConnection();
// 使用完后释放连接
pool->ReleaseConnection(conn);
```

### 数据库操作
```cpp
// 创建数据库操作对象
SQLDatabase db;

// 执行查询
db.Query("SELECT * FROM users WHERE username = 'test'");

// 获取结果
MYSQL_RES* result = db.GetResult();
while (MYSQL_ROW row = mysql_fetch_row(result)) {
    // 处理结果
}
```

### 用户管理
```cpp
// 创建用户管理对象
UserManager userMgr;

// 注册新用户
if (userMgr.Register("newuser", "password123")) {
    // 注册成功
}

// 用户登录
if (userMgr.Login("newuser", "password123")) {
    // 登录成功
}
```

## 注意事项
1. 确保在使用前正确配置MySQL服务器信息
2. 合理设置连接池大小，避免资源浪费
3. 所有数据库操作都应该使用连接池获取的连接
4. 注意处理SQL注入等安全问题
5. 在多线程环境下注意线程安全

## 依赖关系
- MySQL客户端库
- C++11及以上
- 项目的日志模块（用于记录数据库操作日志）