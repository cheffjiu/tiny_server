#pragma once
#ifndef BLOCK_QUEUE_H
#define BLOCK_QUEUE_H

#include "common.h"

template <typename T>
// 阻塞队列
class BlockQueue {
private:
  std::queue<T> queue_;              // 队列
  std::mutex mutex_;                 // 互斥锁
  std::condition_variable cond_var_; // 条件变量
  public:
    BlockQueue()=default;
    ~BlockQueue()=default;

void push(const T &item) {
    std::lock_guard<std::mutex>  lock(mutex_);
    queue_.push(item);
    cond_var_.notify_one();
}

T pop() {
    std::unique_lock<std::mutex> lock(mutex_);
    cond_var_.wait(lock,[this]{return !queue_.empty();});
    T item=queue_.front();
    queue_.pop();
    return item;
}

size_t size(){
    std::lock_guard<std::mutex> lock(mutex_);
    return queue_.size();
}

};
#endif