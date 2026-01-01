#include "core/ReportGenerator.hpp"
#include <iostream>
#include <iomanip>

void ReportGenerator::generate_html(const std::string& filename, 
                                    const std::vector<Partition>& partitions, 
                                    const std::vector<AnalysisResult>& artifacts,
                                    double duration) {
    std::ofstream file(filename);
    if (!file.is_open()) return;

    file << "<html><head><title>Forensic Analysis Report</title>"
         << "<style>"
         << "body { font-family: sans-serif; margin: 40px; background: #f4f4f9; }"
         << "h1 { color: #2c3e50; border-bottom: 3px solid #3498db; }"
         << ".card { background: white; padding: 20px; border-radius: 8px; box-shadow: 0 2px 5px rgba(0,0,0,0.1); margin-bottom: 20px; }"
         << "table { width: 100%; border-collapse: collapse; margin-top: 10px; }"
         << "th { background: #2c3e50; color: white; padding: 10px; text-align: left; }"
         << "td { border-bottom: 1px solid #ddd; padding: 10px; }"
         << ".highlight { color: #e74c3c; font-weight: bold; }"
         << "</style></head><body>";

    file << "<h1>🚀 Parallel Forensic Analysis Report</h1>";
    
    // Summary
    file << "<div class='card'><h3>Scan Summary</h3>"
         << "<p><b>Time Taken:</b> " << duration << " seconds</p>"
         << "<p><b>Artifacts Found:</b> " << artifacts.size() << "</p></div>";

    // Partition Table
    file << "<div class='card'><h3>1. Partition Table (MBR)</h3>";
    if (partitions.empty()) {
        file << "<p>No MBR or Raw Disk Detected.</p>";
    } else {
        file << "<table><tr><th>ID</th><th>Type</th><th>Start LBA</th><th>Size</th><th>Bootable</th></tr>";
        for (const auto& p : partitions) {
            file << "<tr><td>" << p.id << "</td><td>" << p.type << "</td>"
                 << "<td>" << p.start_lba << "</td><td>" << p.sector_count << "</td>"
                 << "<td>" << (p.bootable ? "YES" : "NO") << "</td></tr>";
        }
        file << "</table>";
    }
    file << "</div>";

    // Findings
    file << "<div class='card'><h3>2. Recovered Files</h3><table>"
         << "<tr><th>Chunk ID</th><th>Type</th><th>Offset</th></tr>";
    for (const auto& a : artifacts) {
        file << "<tr><td>" << a.chunk_id << "</td>"
             << "<td class='highlight'>" << a.type << "</td>"
             << "<td>" << a.offset << "</td></tr>";
    }
    file << "</table></div></body></html>";
    file.close();
    std::cout << "[+] Report Generated: " << filename << std::endl;
}

