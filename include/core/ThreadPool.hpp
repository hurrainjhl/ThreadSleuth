#pragma once
#include <vector>
#include <thread>
#include <atomic>
#include "TaskQueue.hpp"
#include "ResultCollector.hpp"

class ThreadPool {
private:
    std::vector<std::thread> workers;
    TaskQueue& task_queue;
    ResultCollector& collector;
    std::atomic<bool> stop_flag;

    void worker_thread();

public:
    ThreadPool(size_t num_threads, TaskQueue& tq, ResultCollector& rc);
    void shutdown();
};
