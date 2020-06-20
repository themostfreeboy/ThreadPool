#include <iostream>
#include <csignal>
#include <string>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <atomic>
#include "thread_pool.h"

// 线程数
const static int THREAD_NUM = 10;

static std::atomic<bool> quit;

// 捕捉信号，用于程序的优雅退出
static void signalHandler(int signum) {
    quit = true;
}

static void rand_task() {
    int rand_x = rand() % 1000;
    int rand_y = rand() % 1000;
    int op = rand() % 2;
    std::string task_str;
    switch (op) {
        case 0: {
            task_str = std::to_string(rand_x) + " + " + std::to_string(rand_y) + " = " + std::to_string(rand_x + rand_y) + "\n";
            break;
        }
        case 1: {
            task_str = std::to_string(rand_x) + " - " + std::to_string(rand_y) + " = " + std::to_string(rand_x - rand_y) + "\n";
            break;
        }
        default: {
            task_str = "error\n";
        }
    }
    std::cout << task_str;
    // 为了保证观察效果，此处sleep 1s
    usleep(1000000);
}

int main(int argc, char** argv) {
    quit = false;
    signal(SIGINT, signalHandler);
    signal(SIGTERM, signalHandler);

    srand((unsigned int)time(NULL));

    ThreadPool thread_pool(THREAD_NUM);

    std::string status_str;
    int sleep_time_us = 0;
    while (!quit) {
        thread_pool.add_task(rand_task);
        // sleep 50 ~ 150 ms
        sleep_time_us = (rand() % 100 + 50) * 1000;
        status_str = "task num=" + std::to_string(thread_pool.get_task_num()) + ", thread num=" + std::to_string(thread_pool.get_thread_num()) + ", sleep_time=" + std::to_string(sleep_time_us/1000) + " ms\n";
        std::cout << status_str;
        usleep(sleep_time_us);
    }

    thread_pool.quit();

    std::cout << "\nprogram going to quit\n";

    return 0;
}
