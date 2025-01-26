#ifndef THREAD_POOL_H
#define THREAD_POOL_H
#include "common.h"
#include "logger.h"
class ThreadPool {
public:
  // 线程池构造函数
  explicit ThreadPool(size_t init_threads = 4, size_t max_threads = 16,
                      size_t queue_limit = 128);

  ThreadPool(const ThreadPool &) = delete;

  ThreadPool &operator=(const ThreadPool &) = delete;
  // 线程池析构函数
  ~ThreadPool();

  // 提交任务(支持返回值)
  template <typename Func, typename... Args>
  auto EnqueueTask(Func &&func,
                   Args &&...args) -> std::future<decltype(func(args...))> {
    using ReturnType = decltype(func(args...));
    auto task = std::make_shared<std::packaged_task<ReturnType()>>(
        std::bind(std::forward<Func>(func), std::forward<Args>(args)...));
    {
      std::unique_lock<std::mutex> lock(queue_mutex_);
      if (tasks_.size() >= queue_limit_) {
        logger_.Log(Logger::ERROR, "Task Queue is full");
        throw std::runtime_error("Task Queue is full");
      }
      tasks_.emplace([task]() { (*task)(); });
      logger_.Log(Logger::DEBUG, "Task Enqueued");
    }
    condition_.notify_one();
    return task->get_future();
  }

private:
  // 线程池私有成员变量
  std::vector<std::thread> workers_;        // 线程池
  std::queue<std::function<void()>> tasks_; // 任务队列
  std::mutex queue_mutex_;                  // 任务队列互斥锁
  std::condition_variable condition_;       // 条件变量，用于线程同步
  std::atomic<bool> stop_;                  // 线程池停止标志
  size_t max_threads_;                      // 线程池最大线程数
  size_t queue_limit_;                      // 任务队列最大容量
  Logger &logger_;                          // 日志记录器
  // 线程池私有成员函数
  void Worker();        // 工作线程函数
  void AdjustThreads(); // 调整线程数量
};
#endif