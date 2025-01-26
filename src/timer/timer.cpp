#include "timer.h"

Timer::Timer(std::function<void(std::function<void()>)> threadpool_executor)
    : threadpool_executor_(std::move(threadpool_executor)) , running_(true){
  
  scheduler_thread_ = std::thread(&Timer::Scheduler, this);
}

Timer::~Timer() { Stop(); }

Timer::TimerId Timer::AddTimer(TimerCallback callback, size_t interval,
                               bool is_repeat) {
  auto expiration_ =
      std::chrono::steady_clock::now() + std::chrono::milliseconds(interval);
  TimerId timer_id_ = next_timer_id_++;
  std::lock_guard<std::mutex> lock(mutex_);
  timer_queue_.push(
      {timer_id_, std::move(callback), expiration_, interval, is_repeat});
  condition_.notify_one();
  return timer_id_;
}

void Timer::RemoveTimer(TimerId timer_id) {
  std::lock_guard<std::mutex> lock(mutex_);
  // 清除任务时不直接删除，而是过滤掉已过期任务时跳过该任务，定时器重新实现时才清理
}
void Timer::Stop() {
  {
    std::lock_guard<std::mutex> lock(mutex_);
    running_.store(false);
    while (!timer_queue_.empty()) {
      timer_queue_.pop();
    }
  }
  condition_.notify_all();
  if (scheduler_thread_.joinable()) {
    scheduler_thread_.join();
  }
}

void Timer::Scheduler() {
  while (running_.load()) {
    TimerTask task;
    {
      std::unique_lock<std::mutex> lock(mutex_);
      if(timer_queue_.empty()){
        condition_.wait(lock,[this](){
          return !timer_queue_.empty()||!running_.load();
        });
      }
      if(!running_.load()){
        break;
      }

      auto now=std::chrono::steady_clock::now();
      
      if(timer_queue_.top().expiration_<=now){// 任务过期
        task=timer_queue_.top();
        timer_queue_.pop();
      }else{
        condition_.wait_until(lock,timer_queue_.top().expiration_);
        continue;
      }
    }

    if(task.callback_){
      // auto callback=std::move(task.callback_);
      // threadpool_executor_([callback](){
      //   callback();
      // });
      threadpool_executor_(task.callback_); 
      if(task.is_repeat_){
        AddTimer(std::move(task.callback_),task.interval_,true);
      }
    }
  }
}