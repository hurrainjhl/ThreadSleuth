#ifndef RESULTCOLLECTOR_HPP
#define RESULTCOLLECTOR_HPP

#include <vector>
#include <string>
#include <mutex>

struct Finding {
    std::string type;
    size_t offset;
    std::string hash;
};

class ResultCollector {
public:
    void add_finding(const std::string& type, size_t offset, const std::string& hash) {
        std::lock_guard<std::mutex> lock(mtx);
        findings.push_back({type, offset, hash});
    }

    std::vector<Finding> get_all_findings() const {
        return findings;
    }

private:
    std::vector<Finding> findings;
    std::mutex mtx;
};

#endif
