#pragma once
#ifndef THREAD_POOL_H
#define THREAD_POOL_H
#include "common.h"

class ThreadPool {
public:
  explicit ThreadPool(size_t threads_count);

  ThreadPool(const ThreadPool &) = delete;

  ThreadPool &operator=(const ThreadPool &) = delete;

  ~ThreadPool();

  void EnqueueTask(std::function<void()> task);

private:
  std::vector<std::thread> workers_;        // 线程池
  std::queue<std::function<void()>> tasks_; // 任务队列

  std::mutex queue_mutex_;            // 互斥锁
  std::condition_variable condition_; // 条件变量
  std::atomic<bool> stop_;            // 线程池停止标志
};
#endif