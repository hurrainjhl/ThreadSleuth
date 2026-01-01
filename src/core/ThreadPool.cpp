#include "core/ThreadPool.hpp"
#include "analysis/SignatureScanner.hpp"
#include "analysis/MetadataParser.hpp"
#include "analysis/EntropyCalculator.hpp" // <--- NEW
#include <iostream>
#include <fstream>
#include <sys/stat.h>
#include <mutex>

// Mutex for synchronized console output
std::mutex cout_mtx;

ThreadPool::ThreadPool(size_t num_threads, TaskQueue& tq, ResultCollector& rc)
    : task_queue(tq), collector(rc), stop_flag(false) {
    mkdir("recovered_files", 0777);
    for (size_t i = 0; i < num_threads; ++i) {
        workers.emplace_back(&ThreadPool::worker_thread, this);
    }
}

void ThreadPool::worker_thread() {
    while (!stop_flag.load()) {
        Chunk chunk;
        if (task_queue.pop(chunk)) {
            
            // --- 1. ENTROPY ANALYSIS (NEW) ---
            double entropy = EntropyCalculator::calculate(chunk.data);
            
            // Log high entropy chunks for the GUI to see
            {
                std::lock_guard<std::mutex> lock(cout_mtx);
                // Format: [ENTROPY] ChunkID:Value
                std::cout << "[ENTROPY] " << chunk.id << ":" << entropy << "\n";
            }
            
            if (entropy > 7.5) {
                 collector.add(chunk.id, "High Entropy / Encrypted Data", chunk.offset);
            }

            // --- 2. SIGNATURE SCAN ---
            auto matches = SignatureScanner::scan(chunk.data, chunk.offset);
            
            for (const auto& m : matches) {
                std::string extra_info = "";

                if (m.type == "NTFS_MFT") {
                    std::string filename = MetadataParser::parse_mft_filename(chunk.data, m.offset - chunk.offset);
                    if (!filename.empty()) extra_info = " (File: " + filename + ")";
                }

                collector.add(chunk.id, m.type + extra_info, m.offset);

                // Carving Logic
                bool is_file = (m.type == "JPEG" || m.type == "PDF" || m.type == "ELF");
                if (is_file) {
                    size_t local_index = m.offset - chunk.offset;
                    size_t bytes_available = chunk.data.size() - local_index;
                    size_t save_size = std::min((size_t)5120, bytes_available); 
                    
                    std::string filename = "recovered_files/" + m.type + "_" + std::to_string(m.offset) + ".bin";
                    std::ofstream out(filename, std::ios::binary);
                    if (out.is_open()) {
                        out.write(reinterpret_cast<const char*>(&chunk.data[local_index]), save_size);
                        out.close();
                    }
                }
            }
        } else {
            if (stop_flag.load()) break;
        }
    }
}

void ThreadPool::shutdown() {
    stop_flag.store(true);
    task_queue.set_finished();
    for (auto& t : workers) if (t.joinable()) t.join();
}

