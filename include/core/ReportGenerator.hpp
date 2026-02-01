#ifndef REPORTGENERATOR_HPP
#define REPORTGENERATOR_HPP

#include <string>
#include <vector>

// Structure to hold evidence details
struct ArtifactRecord {
    int id;
    std::string type;
    size_t offset;
    std::string hash;
    std::string status;
};

// Structure to hold scan statistics
struct ScanStats {
    double duration;
    double throughput;
    int threads_used;
    size_t total_chunks;
    size_t encrypted_chunks;
};

class ReportGenerator {
private:
    std::string filename;
    std::vector<ArtifactRecord> artifacts;
    ScanStats stats;

    // Private Helper Functions (These were missing!)
    std::string get_current_timestamp();
    std::string generate_css();

public:
    // Constructor
    ReportGenerator(std::string filename);

    // Core Methods
    void set_stats(double duration, double throughput, int threads, size_t total, size_t encrypted);
    void add_artifact(int id, std::string type, size_t offset, std::string hash);
    void generate();
};

#endif
