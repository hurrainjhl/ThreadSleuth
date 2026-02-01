/**
 * @file main.cpp
 * @brief Entry point for ThreadSleuth Parallel Forensic Engine
 */
#include <iostream>
#include <vector>
#include <string>
#include <filesystem>
#include <fstream>  // REQUIRED FOR SAVING FILES
#include <iomanip>

// Include Core Modules
#include "../include/core/DiskReader.hpp"
#include "../include/core/ThreadPool.hpp"
#include "../include/core/TaskQueue.hpp"
#include "../include/core/ResultCollector.hpp"
#include "../include/core/ReportGenerator.hpp"
#include "../include/utils/Timer.hpp"

// Include Analysis Modules
#include "../include/analysis/SignatureScanner.hpp"
#include "../include/analysis/EntropyCalculator.hpp"
#include "../include/analysis/HashCalculator.hpp"
#include "../include/analysis/MetadataParser.hpp"

// Standard 10MB Chunk Size
const size_t CHUNK_SIZE = 10 * 1024 * 1024; 

int main(int argc, char* argv[]) {
    // 1. Argument Parsing
    if (argc < 3) {
        std::cerr << "Usage: " << argv[0] << " <image_path> <num_threads>" << std::endl;
        return 1;
    }

    std::string image_path = argv[1];
    int num_threads = std::stoi(argv[2]);

    if (!std::filesystem::exists(image_path)) {
        std::cerr << "[ERROR] File not found: " << image_path << std::endl;
        return 1;
    }

    // --- NEW: Create Recovery Directory ---
    if (!std::filesystem::exists("recovered_files")) {
        std::filesystem::create_directory("recovered_files");
    }

    // 2. Initialization
    std::cout << "[INFO] Engine Started | Target: " << image_path << " | Threads: " << num_threads << std::endl;
    
    DiskReader reader(image_path, CHUNK_SIZE);
    ThreadPool pool(num_threads);
    ResultCollector collector;
    Timer timer;
    
    size_t file_size = std::filesystem::file_size(image_path);
    size_t total_chunks = (file_size + CHUNK_SIZE - 1) / CHUNK_SIZE;
    std::atomic<size_t> encrypted_chunk_count{0};

    // 3. Start Timer & Processing
    timer.start();
    
    for (size_t i = 0; i < total_chunks; ++i) {
        std::vector<unsigned char> chunk_data = reader.read_next_chunk();
        
        if (chunk_data.empty()) break;

        pool.enqueue([i, chunk_data, &collector, &encrypted_chunk_count]() {
            // A. Entropy Analysis
            double entropy = EntropyCalculator::calculate(chunk_data);
            std::cout << "[ENTROPY] Chunk " << i << " : " << std::fixed << std::setprecision(3) << entropy << "\n";

            if (entropy > 7.5) {
                encrypted_chunk_count++;
            }

            // B. Signature Scanning
            auto artifacts = SignatureScanner::scan(chunk_data, i * CHUNK_SIZE);

            // C. Process & SAVE Found Artifacts
            for (auto& art : artifacts) {
                std::string file_hash = HashCalculator::compute_sha256(chunk_data); 
                collector.add_finding(art.type, art.offset, file_hash);

                std::cout << "[FOUND] " << art.type << " at Offset " << art.offset << "\n";
                std::cout << "[HASH] " << file_hash.substr(0, 16) << "...\n";

                // --- NEW: FILE SAVING LOGIC ---
                // If it is a JPEG, save it to the disk so the Dashboard can see it!
                if (art.type == "JPEG" || art.type == "PNG") {
                    std::string ext = (art.type == "JPEG") ? ".jpg" : ".png";
                    std::string filename = "recovered_files/recovered_" + std::to_string(art.offset) + ext;
                    
                    std::ofstream outfile(filename, std::ios::binary);
                    if (outfile.is_open()) {
                        // Calculate where in this chunk the file starts
                        size_t local_offset = art.offset - (i * CHUNK_SIZE);
                        
                        // We save up to 2MB or end of chunk (Simplification for demo)
                        size_t save_size = std::min(chunk_data.size() - local_offset, (size_t)(2 * 1024 * 1024));
                        
                        outfile.write(reinterpret_cast<const char*>(&chunk_data[local_offset]), save_size);
                        outfile.close();
                    }
                }
            }
        });
    }

    pool.wait_until_empty(); 
    
    // 5. Performance Metrics
    double duration = timer.stop();
    double file_size_mb = static_cast<double>(file_size) / (1024 * 1024);
    double throughput = (duration > 0) ? (file_size_mb / duration) : 0.0;

    // 6. Generate Report
    std::cout << "[INFO] Generating Case Report..." << std::endl;
    ReportGenerator report("forensic_report.html");
    report.set_stats(duration, throughput, num_threads, total_chunks, encrypted_chunk_count.load());

    auto findings = collector.get_all_findings(); 
    int id_counter = 1;
    for(const auto& f : findings) {
        report.add_artifact(id_counter++, f.type, f.offset, f.hash);
    }
    report.generate();

    std::cout << "------------------------------------------------\n";
    std::cout << "Time Taken   : " << std::fixed << std::setprecision(2) << duration << " s\n";
    std::cout << "Throughput   : " << throughput << " MB/s\n";
    std::cout << "Total Chunks : " << total_chunks << "\n";
    std::cout << "Encrypted    : " << encrypted_chunk_count << "\n";
    std::cout << "Artifacts    : " << findings.size() << "\n";
    std::cout << "------------------------------------------------\n";

    return 0;
}
