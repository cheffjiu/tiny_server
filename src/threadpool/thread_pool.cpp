#include "thread_pool.h"

ThreadPool::ThreadPool(size_t threads_count) : stop_(false) {
  for (size_t i = 0; i < threads_count; i++) {
    workers_.emplace_back([this] {
      while (true) {
        std::function<void()> task;

        {
          std::unique_lock<std::mutex> lock(this->queue_mutex_);
          this->condition_.wait(
              lock, [this] { return this->stop_ || !this->tasks_.empty(); });

          if (this->stop_ && this->tasks_.empty()) {
            return;
          }

          task = std::move(this->tasks_.front());
          this->tasks_.pop();
        }
        task();
      }
    });
  }
}

void ThreadPool::EnqueueTask(std::function<void()> task) {
  {
    std::unique_lock<std::mutex> lock(this->queue_mutex_);
    tasks_.push(std::move(task));
  }
  condition_.notify_one();
}

ThreadPool::~ThreadPool(){
    {
        std::lock_guard<std::mutex> lock(queue_mutex_);
        stop_ = true;
    }
    condition_.notify_all();
    for (std::thread &worker : workers_) {
        if(worker.joinable()){
            worker.join();
        }
    }
}