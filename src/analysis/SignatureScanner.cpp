#include "../../include/analysis/SignatureScanner.hpp"
#include <iostream>

std::vector<ArtifactInfo> SignatureScanner::scan(const std::vector<unsigned char>& data, size_t base_offset) {
    std::vector<ArtifactInfo> found;
    
    if (data.size() < 4) return found;

    for (size_t i = 0; i < data.size() - 4; ++i) {
        // 1. JPEG Signature: FF D8 FF
        if (data[i] == 0xFF && data[i+1] == 0xD8 && data[i+2] == 0xFF) {
            found.push_back({"JPEG", base_offset + i});
            i += 512; // Skip ahead to avoid duplicate detection close by
        }
        
        // 2. PNG Signature: 89 50 4E 47
        else if (data[i] == 0x89 && data[i+1] == 0x50 && 
                 data[i+2] == 0x4E && data[i+3] == 0x47) {
            found.push_back({"PNG", base_offset + i});
            i += 512;
        }
        
        // 3. PDF Signature: %PDF (25 50 44 46)
        else if (data[i] == 0x25 && data[i+1] == 0x50 && 
                 data[i+2] == 0x44 && data[i+3] == 0x46) {
            found.push_back({"PDF", base_offset + i});
            i += 512;
        }
    }
    
    return found;
}
