#include <iostream>
#include <algorithm>
#include <string>
#include "core/DiskReader.hpp"
#include "core/TaskQueue.hpp"
#include "core/ResultCollector.hpp"
#include "core/ThreadPool.hpp"
#include "core/PartitionParser.hpp"
#include "core/ReportGenerator.hpp"
#include "utils/Timer.hpp"

// 1MB Chunk + 4KB Overlap
const uint64_t CHUNK_SIZE = 1024 * 1024; 
const uint64_t OVERLAP_SIZE = 4096; 

int main(int argc, char *argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: ./parallel_engine <disk_image.dd> [num_threads]\n";
        return 1;
    }

    try {
        std::string image_path = argv[1];
        size_t num_threads = std::thread::hardware_concurrency();

        // Allow user to override thread count
        if (argc >= 3) {
            num_threads = std::stoi(argv[2]);
        }

        std::cout << "🔥 Parallel Forensic Analyzer (Final Release) 🔥\n";
        std::cout << "[*] Target: " << image_path << "\n";
        std::cout << "[*] Threads: " << num_threads << "\n";
        
        DiskReader disk(image_path);
        
        // 1. MBR Analysis
        auto mbrData = disk.read_chunk(0, 512);
        auto parts = PartitionParser::parse(mbrData);
        if (!parts.empty()) std::cout << "[+] MBR Found (" << parts.size() << " partitions)\n";

        // 2. Parallel Scan Setup
        TaskQueue queue;
        ResultCollector collector;
        uint64_t total_size = disk.size();
        uint64_t total_chunks = (total_size + CHUNK_SIZE - 1) / CHUNK_SIZE;

        Timer timer;
        timer.start();
        ThreadPool pool(num_threads, queue, collector);

        // 3. Producer Loop
        for (uint64_t i = 0; i < total_chunks; ++i) {
            uint64_t offset = i * CHUNK_SIZE;
            
            // Read extra bytes for overlap
            uint64_t read_size = CHUNK_SIZE + OVERLAP_SIZE;
            if (offset + read_size > total_size) {
                read_size = total_size - offset;
            }

            auto data = disk.read_chunk(offset, read_size);
            queue.push(Chunk(i, offset, std::move(data)));
        }

        pool.shutdown();
        double elapsed = timer.stop();
        
        // 4. Reporting
        ReportGenerator::generate_html("forensic_report.html", parts, collector.get_all(), elapsed);
        
        std::cout << "------------------------------------------------\n";
        std::cout << "✅ Analysis Complete\n";
        std::cout << "⏱️  Time Taken : " << elapsed << " seconds\n";
        std::cout << "📂 Artifacts  : " << collector.count() << "\n";
        std::cout << "🚀 Throughput : " << (total_size / (1024.0*1024.0)) / elapsed << " MB/s\n";
        std::cout << "------------------------------------------------\n";

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
        return 1;
    }
    return 0;
}
