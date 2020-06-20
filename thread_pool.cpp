#include <assert.h>
#include "thread_pool.h"

ThreadPool::ThreadPool(int thread_num) {
    _quit = false;
    assert(thread_num > 0);

    std::string notice_str = "creating thread_pool, thread_num=" + std::to_string(thread_num) + "\n";
    std::cout << notice_str;

    for (int i = 0; i < thread_num; ++i) {
        _pool.push_back(std::thread(&ThreadPool::run_task, this, i));
    }
}

ThreadPool::~ThreadPool() {
    quit();
}

void ThreadPool::quit() {
    if (!_quit) {
        _quit = true;
        _cond_var.notify_all();
        for (std::thread& thread : _pool) {
            if (thread.joinable()) {
                thread.join();
            }
        }
    }
}

void ThreadPool::add_task(const Task& task) {
    {
        std::lock_guard<std::mutex> lock(_mutex);
        _tasks.push(task);
    }
    _cond_var.notify_one();
}

int ThreadPool::get_thread_num() const {
    // 不需要加锁
    return _pool.size();
}

int ThreadPool::get_task_num() {
    std::lock_guard<std::mutex> lock(_mutex);
    return _tasks.size();
}

void ThreadPool::run_task(int thread_index) {
    std::string start_str = "thread " + std::to_string(thread_index) + " start\n";
    std::cout << start_str;

    Task task;
    while (!_quit) {
        {
            std::unique_lock<std::mutex> lock(_mutex);
            // while防止虚假唤醒
            while (!_quit && _tasks.empty()) {
                _cond_var.wait(lock, [this] { return _quit || !_tasks.empty(); });
            }
    
            if (_quit) {
                break;
            }
    
            task = _tasks.front();
            _tasks.pop();
        }
        task();
    }

    std::string end_str = "thread " + std::to_string(thread_index) + " end\n";
    std::cout << end_str;
}
