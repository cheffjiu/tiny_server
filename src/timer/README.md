# Timer 定时器模块

## 模块概述

Timer模块是一个基于C++11的高性能定时器实现，支持单次定时任务和重复定时任务的调度执行。该模块采用线程池进行任务处理，确保高效的任务执行和资源利用。

## 核心组件

### 1. 定时器任务管理
- 使用优先队列（timer_queue_）存储定时任务
- 支持动态添加和移除定时任务
- 每个定时任务包含：
  - 唯一标识符（timer_id）
  - 回调函数（callback）
  - 过期时间（expiration）
  - 时间间隔（interval）
  - 是否重复（is_repeat）

### 2. 调度器
- 独立的调度线程，负责任务的定时检查和触发
- 使用条件变量实现高效的任务等待和唤醒机制
- 支持优雅停止和资源回收

### 3. 线程池交互
- 通过线程池执行器（threadpool_executor）处理定时任务
- 避免阻塞调度线程，提高并发性能

## 接口文档

### 构造函数
```cpp
Timer(std::function<void(std::function<void()>)> threadpool_executor)
```
- 参数：threadpool_executor - 线程池执行器函数
- 功能：初始化定时器，启动调度线程

### 添加定时任务
```cpp
TimerId AddTimer(TimerCallback callback, size_t interval, bool is_repeat)
```
- 参数：
  - callback: 定时任务回调函数
  - interval: 时间间隔（毫秒）
  - is_repeat: 是否重复执行
- 返回：定时器ID（用于后续管理）
- 功能：添加一个定时任务到队列

### 移除定时任务
```cpp
void RemoveTimer(TimerId timer_id)
```
- 参数：timer_id - 要移除的定时器ID
- 功能：从队列中移除指定的定时任务

### 停止定时器
```cpp
void Stop()
```
- 功能：停止定时器，清理所有任务，等待调度线程结束

## 使用示例

```cpp
// 创建线程池执行器
auto executor = [&](std::function<void()> task) {
    // 将任务提交到线程池
    thread_pool.submit(std::move(task));
};

// 创建定时器
Timer timer(executor);

// 添加一个每5秒执行一次的重复任务
auto timer_id = timer.AddTimer(
    []() { std::cout << "Timer task executed!" << std::endl; },
    5000,  // 5000ms = 5s
    true   // 重复执行
);

// 移除定时任务
timer.RemoveTimer(timer_id);

// 停止定时器
timer.Stop();
```

## 性能优化

1. 使用优先队列优化任务调度
   - 自动维护任务的执行顺序
   - O(log n)的插入和删除复杂度

2. 条件变量优化等待机制
   - 避免忙等待，降低CPU占用
   - 支持精确的定时唤醒

3. 线程池解耦
   - 将任务执行与调度分离
   - 支持灵活的任务处理策略

## 注意事项

1. 线程安全
   - 所有公共接口都是线程安全的
   - 内部使用互斥锁保护共享资源

2. 资源管理
   - 确保在程序退出前调用Stop()
   - 避免在回调函数中执行耗时操作

3. 定时精度
   - 使用steady_clock保证时间单调性
   - 实际执行时间可能略有延迟