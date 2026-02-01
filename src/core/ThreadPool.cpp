#include "../../include/core/ThreadPool.hpp"

ThreadPool::ThreadPool(size_t num_threads) : stop(false), active_tasks(0) {
    for(size_t i = 0; i < num_threads; ++i) {
        workers.emplace_back([this] {
            while(true) {
                std::function<void()> task;
                {
                    std::unique_lock<std::mutex> lock(this->queue_mutex);
                    this->condition.wait(lock, [this]{ return this->stop || !this->tasks.empty(); });
                    
                    if(this->stop && this->tasks.empty()) return;
                    
                    task = std::move(this->tasks.front());
                    this->tasks.pop();
                    this->active_tasks++;
                }
                
                task();
                
                this->active_tasks--;
                this->finished_condition.notify_all();
            }
        });
    }
}

void ThreadPool::enqueue(std::function<void()> task) {
    {
        std::unique_lock<std::mutex> lock(queue_mutex);
        tasks.emplace(task);
    }
    condition.notify_one();
}

void ThreadPool::wait_until_empty() {
    std::unique_lock<std::mutex> lock(queue_mutex);
    finished_condition.wait(lock, [this]() { 
        return tasks.empty() && (active_tasks == 0); 
    });
}

ThreadPool::~ThreadPool() {
    {
        std::unique_lock<std::mutex> lock(queue_mutex);
        stop = true;
    }
    condition.notify_all();
    for(std::thread &worker: workers) {
        if(worker.joinable()) worker.join();
    }
}
