#pragma once
#include <string>
#include <vector>
#include <cstdint>

class DiskReader {
public:
    explicit DiskReader(const std::string& path);
    ~DiskReader();
    uint64_t size() const;
    std::vector<uint8_t> read_chunk(uint64_t offset, uint64_t size);

private:
    std::string file_path;
    uint64_t total_size;
};
