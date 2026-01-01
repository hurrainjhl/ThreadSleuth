#include "core/PartitionParser.hpp"
#include <sstream>
#include <iomanip>

std::string get_partition_type(uint8_t type) {
    switch(type) {
        case 0x07: return "NTFS / exFAT";
        case 0x83: return "Linux Native (Ext4)";
        case 0x0B: return "FAT32";
        case 0x05: return "Extended Partition";
        case 0x00: return "Empty";
        default:   return "Unknown";
    }
}

std::vector<Partition> PartitionParser::parse(const std::vector<uint8_t>& data) {
    std::vector<Partition> partitions;
    
    if (data.size() < 512 || data[510] != 0x55 || data[511] != 0xAA) {
        return partitions; 
    }

    for (int i = 0; i < 4; i++) {
        size_t offset = 446 + (i * 16);
        uint8_t status = data[offset];
        uint8_t type   = data[offset + 4];
        
        uint32_t start_lba = *reinterpret_cast<const uint32_t*>(&data[offset + 8]);
        uint32_t size      = *reinterpret_cast<const uint32_t*>(&data[offset + 12]);

        if (size > 0) {
            partitions.push_back({
                i + 1,
                get_partition_type(type),
                start_lba,
                size,
                (status == 0x80)
            });
        }
    }
    return partitions;
}

