#include "analysis/SignatureScanner.hpp"

std::vector<SignatureMatch> SignatureScanner::scan(const std::vector<uint8_t>& data, size_t baseOffset) {
    std::vector<SignatureMatch> matches;
    if (data.size() < 8) return matches;

    // We scan up to size - 8 to ensure we don't crash on long signatures
    for (size_t i = 0; i < data.size() - 8; ++i) {
        
        // --- FILES ---
        // JPEG (FF D8 FF)
        if (data[i] == 0xFF && data[i+1] == 0xD8 && data[i+2] == 0xFF)
            matches.push_back({"JPEG", baseOffset + i});

        // PDF (%PDF)
        else if (data[i] == 0x25 && data[i+1] == 0x50 && data[i+2] == 0x44 && data[i+3] == 0x46)
            matches.push_back({"PDF", baseOffset + i});
            
        // ELF (Linux Executable)
        else if (data[i] == 0x7F && data[i+1] == 0x45 && data[i+2] == 0x4C && data[i+3] == 0x46)
            matches.push_back({"ELF", baseOffset + i});

        // --- OS METADATA (New!) ---
        
        // NTFS Master File Table Entry ("FILE0")
        // Hex: 46 49 4C 45 30
        else if (data[i] == 0x46 && data[i+1] == 0x49 && data[i+2] == 0x4C && data[i+3] == 0x45 && data[i+4] == 0x30)
            matches.push_back({"NTFS_MFT", baseOffset + i});

        // GPT Partition Header ("EFI PART")
        // Hex: 45 46 49 20 50 41 52 54
        else if (data[i] == 0x45 && data[i+1] == 0x46 && data[i+2] == 0x49 && data[i+3] == 0x20 && 
                 data[i+4] == 0x50 && data[i+5] == 0x41 && data[i+6] == 0x52 && data[i+7] == 0x54)
            matches.push_back({"GPT_HEADER", baseOffset + i});
    }
    return matches;
}
