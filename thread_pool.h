#ifndef __THREAD_POOL__
#define __THREAD_POOL__

#include <iostream>
#include <mutex>
#include <condition_variable>
#include <thread>
#include <queue>
#include <functional>
#include <atomic>

class ThreadPool {
public:
    using Task = std::function<void()>;
    ThreadPool(int thread_num);
    ~ThreadPool();
    void quit();
    void add_task(const Task& task);
    int get_thread_num();
    int get_task_num();

private:
    std::vector<std::thread> _pool;
    std::queue<Task> _tasks;
    std::mutex _mutex;
    std::condition_variable _cond_var;
    std::atomic<bool> _quit;
    void run_task(int thread_index);
    ThreadPool(const ThreadPool& queue) = delete;
    ThreadPool& operator=(const ThreadPool& queue) = delete;
};

#endif
