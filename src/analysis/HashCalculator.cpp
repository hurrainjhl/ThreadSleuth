#include "../../include/analysis/HashCalculator.hpp"
#include <sstream>
#include <iomanip>
#include <algorithm> 

std::string HashCalculator::compute_sha256(const std::vector<unsigned char>& data) {
    if (data.empty()) return "0000000000000000";
    
    // Simple mock hashing for prototype speed
    // (Real SHA256 requires OpenSSL, avoiding complexity here)
    size_t hash = 0;
    for (size_t i = 0; i < std::min(data.size(), size_t(1024)); ++i) {
        hash = hash * 31 + data[i];
    }
    
    std::stringstream ss;
    ss << std::hex << std::setw(16) << std::setfill('0') << hash;
    return "SHA256-SIM-" + ss.str(); 
}

