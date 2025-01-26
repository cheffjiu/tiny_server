#include "thread_pool.h"

ThreadPool::ThreadPool(size_t init_threads, size_t max_threads,
                       size_t queue_limit)
    : max_threads_(max_threads), queue_limit_(queue_limit),
      logger_(Logger::GetInstance(LOGFILE)) {
  stop_.store(false);
  logger_.Log(Logger::INFO, "Initializing ThreadPool ");
  // 创建初始线程
  for (auto i = 0; i < init_threads; ++i) {
    workers_.emplace_back(std::thread(&ThreadPool::Worker, this));
    logger_.Log(Logger::DEBUG, "Created Thread" + std::to_string(i + 1));
  }
  logger_.Log(Logger::INFO, "ThreadPool Initialized with" +
                                std::to_string(init_threads) + "threads");
}

ThreadPool::~ThreadPool() {
  {
    std::lock_guard<std::mutex> lock(queue_mutex_);
    stop_.store(true);
  }
  condition_.notify_all();
  for (std::thread &worker : workers_) {
    if (worker.joinable()) {
      worker.join();
    }
  }
  logger_.Log(Logger::INFO, "ThreadPool Destroyed");
}



void ThreadPool::Worker() {
    logger_.Log(Logger::INFO, "Worker Thread Started");
    while (true) {
        std::function<void()> task;

        {
            std::unique_lock<std::mutex> lock(this->queue_mutex_);
            this->condition_.wait(
                lock, [this] { 
                 logger_.Log(Logger::DEBUG, "Worker checking for tasks");
                  return this->stop_ || !this->tasks_.empty(); });

            if (this->stop_ && this->tasks_.empty()) {
              logger_.Log(Logger::DEBUG, "Worker Thread Stopping");
                return;
            }

            task = std::move(this->tasks_.front());
            this->tasks_.pop();
            logger_.Log(Logger::DEBUG, "Task Dequeued");
        }
        try {
            task();
            logger_.Log(Logger::DEBUG, "Task Executed successfully");
        } catch (const std::exception &e) {
            logger_.Log(Logger::ERROR,
                        "Task Failed with Exception: " + std::string(e.what()));
        }
    }
    logger_.Log(Logger::DEBUG, "Worker Thread Stopped");
}

void ThreadPool::AdjustThreads() {
  logger_.Log(Logger::INFO, "Adjusting Threads");
  while (!stop_.load()) {
    std::this_thread::sleep_for(std::chrono::seconds(5));

    std::unique_lock<std::mutex> lock(queue_mutex_);
    size_t task_count = tasks_.size();
    size_t current_threads = workers_.size();
    // 扩容：如果任务数远大于线程数，且线程数小于最大限制
    if (task_count > current_threads * 2 && current_threads < max_threads_) {
      workers_.emplace_back(std::thread(&ThreadPool::Worker, this));
      logger_.Log(Logger::INFO, "ThreadPool expanded,Created new Thread");
    } else if (task_count < current_threads / 2 && current_threads > 1) {
      workers_.back().detach();
      workers_.pop_back();
      logger_.Log(Logger::INFO, "ThreadPool shrinked");
    }
  }
}
