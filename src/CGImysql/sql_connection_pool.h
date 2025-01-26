#ifndef SQL_CONNECTION_POOL_H
#define SQL_CONNECTION_POOL_H
#include "common.h"
#include <mysql/mysql.h>
class ConnectionPool {
public:
  static ConnectionPool &GetInstance(); // 获取单例实例
  void Init(const std::string &host, int port, const std::string &user,
            const std::string &password, const std::string &dbname,
            int init_size, int max_size, int idle_timeout); // 初始化连接池

  std::unique_ptr<MYSQL,std::function<void(MYSQL *)>> GetConnection(); // 获取连接
  void ReleaseConnection(MYSQL *conn);                       // 释放连接
  ~ConnectionPool();                                         // 析构函数
  void PrintStatistics()const;                                    // 打印连接池统计信息
private:
  ConnectionPool();     // 私有化构造函数，防止外部实例化
  void DynamicAdjust(); // 动态调整线程（扩容，缩容，健康检查）
  MYSQL *CreateConnection();           // 创建连接
  void DestroyConnection(MYSQL *conn); // 销毁连接

  // 连接池资源
  std::queue<std::pair<MYSQL *, std::chrono::steady_clock::time_point>>
      conn_queue_;               // 连接队列
  std::mutex mutex_;             // 互斥锁
  std::condition_variable cond_; // 条件变量

  // 配置参数
  std::string host_;                 // 数据库服务器地址
  std::string user_;                 // 数据库用户名
  std::string password_;             // 数据库密码
  std::string dbname_;               // 数据库名
  int port_;                         // 数据库端口
  int max_size_;                     // 最大连接数
  int init_size_;                    // 初始连接数
  int idle_timeout_;                 // 连接超时时间
  std::atomic<int> current_size_{0}; // 当前连接数

  // 动态调整线程
  std::thread adjust_thread_; // 动态调整线程
  std::atomic<bool> running_; // 连接池是否在运行

  // 统计信息
  std::atomic<int> max_used_connections_{0};//历史最大使用连接数
  std::atomic<int>total_allocations_{0}; //总分配次数
  std::atomic<int>total_releases_{0}; //总归还次数

};

#endif