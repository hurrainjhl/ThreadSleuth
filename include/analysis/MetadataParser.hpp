#pragma once
#include <vector>
#include <string>
#include <cstdint>

class MetadataParser {
public:
    // Tries to extract a filename from a raw NTFS MFT Record
    static std::string parse_mft_filename(const std::vector<uint8_t>& buffer, size_t offset);
};
