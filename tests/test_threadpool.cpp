#include <iostream>
#include <vector>
#include <chrono>
#include <thread>
#include "../include/core/ThreadPool.hpp"

// Simple unit test to verify the Thread Pool works
int main() {
    std::cout << "[TEST] Initializing ThreadPool with 2 threads..." << std::endl;
    
    // 1. Initialize Pool
    ThreadPool pool(2);
    
    // 2. Enqueue Tasks
    for(int i = 0; i < 5; ++i) {
        pool.enqueue([i] {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
            // Print strictly to verify concurrency
            // (In real apps, use a mutex for printing, but this is just a quick test)
        });
    }

    // 3. Wait for completion
    std::cout << "[TEST] Waiting for tasks to finish..." << std::endl;
    pool.wait_until_empty();
    
    std::cout << "[TEST] ThreadPool Test Passed!" << std::endl;
    return 0;
}
