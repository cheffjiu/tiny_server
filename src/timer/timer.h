#ifndef TIMER_H
#define TIMER_H
#include "common.h"

class Timer {
public:
  using TimerId = size_t;
  using TimerCallback = std::function<void()>;
  explicit Timer(
      std::function<void(std::function<void()>)> threadpool_executor);

  ~Timer();
  Timer(const Timer &) = delete;
  Timer &operator=(const Timer &) = delete;

  TimerId AddTimer(TimerCallback callback, size_t interval,
                   bool is_repeat = false);

  void RemoveTimer(TimerId timer_id);

  void Stop();

private:
  struct TimerTask {
    TimerId timer_id_;       // 定时器id
    TimerCallback callback_; // 定时器回调函数
    std::chrono::time_point<std::chrono::steady_clock>
        expiration_;  // 定时器过期时间
    size_t interval_; // 定时器间隔时间
    bool is_repeat_;  // 是否周期性任务

    bool operator>(const TimerTask &other) const {
      return this->expiration_ > other.expiration_;
    }
  };
  std::priority_queue<TimerTask, std::vector<TimerTask>,
                      std::greater<TimerTask>>
      timer_queue_;                       // 定时器队列
  std::mutex mutex_;                      // 互斥锁
  std::condition_variable condition_;     // 条件变量
  std::thread scheduler_thread_;          // 调度线程
  std::atomic<bool> running_;             // 定时器运行标志
  std::atomic<TimerId> next_timer_id_{1}; // 下一个定时器id
  std::function<void(std::function<void()>)>
      threadpool_executor_; // 线程池执行器

  void Scheduler(); // 定时器调度函数
};
#endif
