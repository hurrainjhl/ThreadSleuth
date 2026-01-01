#pragma once
#include <vector>
#include <string>
#include <cstdint>

struct Partition {
    int id;
    std::string type;
    uint32_t start_lba;
    uint32_t sector_count;
    bool bootable;
};

class PartitionParser {
public:
    static std::vector<Partition> parse(const std::vector<uint8_t>& mbrData);
};
