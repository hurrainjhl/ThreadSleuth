#pragma once
#include <vector>
#include <mutex>
#include <string>
#include <iostream>

struct AnalysisResult {
    uint32_t chunk_id;
    std::string type;
    uint64_t offset;
};

class ResultCollector {
private:
    std::vector<AnalysisResult> results;
    std::mutex mtx;

public:
    void add(uint32_t id, const std::string& type, uint64_t offset) {
        std::lock_guard<std::mutex> lock(mtx);
        results.push_back({id, type, offset});
        
        // Print real-time feedback with filenames!
        // std::cout << "[Found] " << type << " at offset " << offset << "\n";
    }

    std::vector<AnalysisResult> get_all() {
        std::lock_guard<std::mutex> lock(mtx);
        return results;
    }

    size_t count() {
        std::lock_guard<std::mutex> lock(mtx);
        return results.size();
    }
    
    void print_summary() {
        std::lock_guard<std::mutex> lock(mtx);
        // std::cout << "\n--- Console Summary ---\n";
    }
};

