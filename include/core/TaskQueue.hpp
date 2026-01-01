#pragma once
#include <queue>
#include <mutex>
#include <condition_variable>
#include "Chunk.hpp"

class TaskQueue {
private:
    std::queue<Chunk> tasks;
    std::mutex mtx;
    std::condition_variable cv;
    bool finished = false;

public:
    void push(Chunk chunk) {
        std::lock_guard<std::mutex> lock(mtx);
        tasks.push(std::move(chunk));
        cv.notify_one();
    }

    bool pop(Chunk &chunk) {
        std::unique_lock<std::mutex> lock(mtx);
        cv.wait(lock, [this] { return !tasks.empty() || finished; });
        if (!tasks.empty()) {
            chunk = std::move(tasks.front());
            tasks.pop();
            return true;
        }
        return false;
    }

    void set_finished() {
        std::lock_guard<std::mutex> lock(mtx);
        finished = true;
        cv.notify_all();
    }
};
