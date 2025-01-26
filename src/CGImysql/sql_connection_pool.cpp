#include "sql_connection_pool.h"
#include "logger.h"

/**
 * @brief 获取连接池实例
 *
 * 使用单例模式返回连接池实例。如果实例不存在，则创建新实例。
 *
 * @return 返回连接池实例的引用
 */
ConnectionPool &ConnectionPool::GetInstance() {
  // 使用静态局部变量确保instance在程序运行期间只被创建一次
  static ConnectionPool instance;
  // 返回创建的ConnectionPool实例
  return instance;
}

ConnectionPool::ConnectionPool() : current_size_(0) { running_.store(true); }

/**
 * @brief 初始化连接池
 *
 * 初始化连接池参数，并创建初始连接池中的连接数量，同时启动动态调整线程。
 *
 * @param host 数据库主机地址
 * @param port 数据库端口号
 * @param user 数据库用户名
 * @param password 数据库密码
 * @param dbname 数据库名称
 * @param init_size 初始连接池中的连接数量
 * @param max_size 连接池中的最大连接数量
 * @param idle_timeout 连接空闲超时时间（秒）
 */
void ConnectionPool::Init(const std::string &host, int port,
                          const std::string &user, const std::string &password,
                          const std::string &dbname, int init_size,
                          int max_size, int idle_timeout) {
  // 设置连接池的基本信息
  host_ = host;
  port_ = port;
  user_ = user;
  password_ = password;
  dbname_ = dbname;
  init_size_ = init_size;
  max_size_ = max_size;
  idle_timeout_ = idle_timeout;

  // 创建初始连接池中的连接数量
  // 遍历init_size次，每次创建一个连接
  for (auto i = 0; i < init_size_; ++i) {
    MYSQL *conn = CreateConnection(); // 创建连接
    if (conn) {
      // 使用互斥锁保证线程安全
      std::lock_guard<std::mutex> lock(mutex_);
      // 将连接加入队列，并记录创建时间
      conn_queue_.emplace(conn, std::chrono::steady_clock::now());
      current_size_++; // 连接数量增加
    }
  }

  // 启动动态调整线程
  adjust_thread_ = std::thread(&ConnectionPool::DynamicAdjust,
                               this); // 启动线程执行DynamicAdjust函数
}

/**
 * @brief 创建并返回一个数据库连接对象
 *
 * 该函数用于创建一个数据库连接对象，并返回该对象。如果创建失败，将记录错误信息。
 *
 * @return 成功时返回连接对象指针，失败时返回nullptr。
 */
MYSQL *ConnectionPool::CreateConnection() {
  MYSQL *conn = mysql_init(nullptr);
  Logger &logger = Logger::GetInstance(LOGFILE);
  if (!conn) {
    logger.Log(Logger::ERROR, "Mysql init failed");
  }
  conn =
      mysql_real_connect(conn, host_.c_str(), user_.c_str(), password_.c_str(),
                         dbname_.c_str(), port_, nullptr, 0);
  if (!conn) {
    logger.Log(Logger::ERROR,
               "Mysql connect failed" + std::string(mysql_error(conn)));
  }
    logger.Log(Logger::INFO,"Mysql connect success");
  return conn;
}

/**
 * @brief 销毁数据库连接
 *
 * 释放并关闭一个数据库连接。
 *
 * @param conn 数据库连接指针
 */
void ConnectionPool::DestroyConnection(MYSQL *conn) {
  if (conn) {
    mysql_close(conn);
  }
}

/**
 * @brief 从连接池中获取一个连接
 *
 * 从连接池中获取一个连接，如果连接池为空，则尝试创建一个新的连接。
 * 如果连接池已满且不能扩容，则等待直到有可用连接。
 *
 * @return std::unique_ptr<MYSQL, std::function<void(MYSQL*)>>
 * 包含MYSQL连接的智能指针，当智能指针销毁时会自动释放连接
 */
std::unique_ptr<MYSQL, std::function<void(MYSQL *)>>
ConnectionPool::GetConnection() {
  std::unique_lock<std::mutex> lock(mutex_);
  // 如果连接池为空，且可以扩容，则创建新连接
  while (conn_queue_.empty()) {
    // 如果当前连接数小于最大连接数且服务正在运行
    if (current_size_ < max_size_ && running_.load()) {
      MYSQL *conn = CreateConnection();
      if (conn) {
        current_size_++;
        total_allocations_++;
        max_used_connections_ =
            std::max(max_used_connections_.load(), current_size_.load());
        // 返回一个指向新创建的连接的unique_ptr，并设置自定义删除器
        return std::unique_ptr<MYSQL, std::function<void(MYSQL *)>>(
            conn, [](MYSQL *conn) {
              // 当unique_ptr被销毁时，释放连接
              ConnectionPool::GetInstance().ReleaseConnection(conn);
            });
      }
    } else {
      // 等待条件变量被通知
      cond_.wait(lock);
    }
  }
  // 从连接池中取出一个空闲连接，并从队列中移除该连接
  auto conn_pair = conn_queue_.front();
  conn_queue_.pop();
  total_allocations_++;
  max_used_connections_ =
      std::max(max_used_connections_.load(), current_size_.load());

  // 返回一个指向从连接池中取出的连接的unique_ptr，并设置自定义删除器
  return std::unique_ptr<MYSQL, std::function<void(MYSQL *)>>(
      conn_pair.first, [](MYSQL *conn) {
        // 当unique_ptr被销毁时，释放连接
        ConnectionPool::GetInstance().ReleaseConnection(conn);
      });
}

/**
 * @brief 释放连接
 *
 * 将连接放回连接池中的连接队列中，并通知等待连接的线程。
 *
 * @param conn 要释放的数据库连接指针
 */
void ConnectionPool::ReleaseConnection(MYSQL *conn) {
  // 使用互斥锁锁定，以确保线程安全
  std::lock_guard<std::mutex> lock(mutex_);

  // 将连接放入队列，并记录当前时间
  conn_queue_.emplace(conn, std::chrono::steady_clock::now());

  // 通知等待连接的线程，有一个连接可用
  cond_.notify_one();

  // 记录释放连接的总次数
  total_releases_++;
}
/**
 * @brief 动态调整连接池
 *
 * 该函数动态调整连接池中的连接数量，包括健康检查和缩容操作。
 *
 * 首先，函数进入一个循环，循环条件是运行标志running_为true。
 * 在循环内部，函数首先让当前线程休眠1秒，然后使用互斥锁lock_guard保护临界区代码。
 *
 * 在临界区内，函数首先获取空闲连接数idle_conn_num，然后获取当前时间now。
 *
 * 接下来，函数进行健康检查，将空闲连接队列conn_queue_中的每个连接进行mysql_ping检测，
 * 如果连接不可用（mysql_ping检测失败）或者超过空闲时间idle_timeout_，则销毁该连接并减少连接总数current_size_。
 * 如果连接可用，则将其加入临时队列temp_queue中。
 *
 * 然后，函数将临时队列temp_queue赋值给空闲连接队列conn_queue_。
 *
 * 接着，函数进行缩容操作，如果空闲连接数idle_conn_num大于初始连接数init_size_，
 * 并且空闲连接队列conn_queue_不为空，则关闭多余的空闲连接，
 * 减少连接总数current_size_和空闲连接数idle_conn_num。
 */
void ConnectionPool::DynamicAdjust() {
  while (running_.load()) {
    std::this_thread::sleep_for(std::chrono::seconds(1));
    // 获取锁，保护共享资源
    std::lock_guard<std::mutex> lock(mutex_);
    int idle_conn_num = conn_queue_.size();
    auto now = std::chrono::steady_clock::now();

    // 健康检查：检测所有空闲连接是否有效
    std::queue<std::pair<MYSQL *, std::chrono::steady_clock::time_point>>
        temp_queue;
    while (!conn_queue_.empty()) {
      auto conn_pair = conn_queue_.front();
      conn_queue_.pop();

      // 如果连接不可用（mysql_ping检测失败）或者超过空闲时间,销毁该连接并减少连接总数
      if (mysql_ping(conn_pair.first) != 0 ||
          std::chrono::duration_cast<std::chrono::seconds>(now -
                                                           conn_pair.second)
                  .count() > idle_timeout_) {
        DestroyConnection(conn_pair.first);
        current_size_--;
      } else {
        temp_queue.push(conn_pair);
      }
    }
    conn_queue_ = std::move(temp_queue);

    // 缩容：关闭多余的空闲连接，但保持不少于初始连接数
    while (idle_conn_num > init_size_*2 && !conn_queue_.empty()) {
      auto conn_pair = conn_queue_.front();
      conn_queue_.pop();
      DestroyConnection(conn_pair.first);
      current_size_--;
      idle_conn_num--;
    }
  }
}

/**
 * @brief 析构函数，用于销毁连接池对象
 *
 * 停止连接池的运行，并销毁所有连接。
 */
ConnectionPool::~ConnectionPool() {
  running_.store(false);
  if (adjust_thread_.joinable()) {
    adjust_thread_.join();
  }

  while (!conn_queue_.empty()) {
    auto conn_pair = conn_queue_.front();
    conn_queue_.pop();
    DestroyConnection(conn_pair.first);
  }
}

/**
 * @brief 打印连接池统计信息
 *
 * 该函数用于打印连接池的统计信息，包括当前连接数、当前空闲连接数、最大使用连接数、总分配连接数和总释放连接数。
 *
 * @note 该函数是常成员函数，不会修改对象的状态。
 */
void ConnectionPool::PrintStatistics() const {
  std::cout << "Connection Pool Statistics:" << std::endl;
  std::cout << "----------------------------------------" << std::endl;
  std::cout << "  Current Connection Count: " << current_size_.load()
            << std::endl;
  std::cout << "  Current Idle Connections: " << conn_queue_.size()
            << std::endl;
  std::cout << "  Max Used Connections:     " << max_used_connections_.load()
            << std::endl;
  std::cout << "  Total Allocations:        " << total_allocations_.load()
            << std::endl;
  std::cout << "  Total Releases:           " << total_releases_.load()
            << std::endl;
  std::cout << "----------------------------------------" << std::endl;
}

