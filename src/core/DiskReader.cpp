#include "core/DiskReader.hpp"
#include <fstream>
#include <stdexcept>

DiskReader::DiskReader(const std::string& path) : file_path(path) {
    std::ifstream file(path, std::ios::binary | std::ios::ate);
    if (!file.is_open())
        throw std::runtime_error("Failed to open disk image: " + path);
    total_size = file.tellg();
}

DiskReader::~DiskReader() {}

uint64_t DiskReader::size() const { return total_size; }

std::vector<uint8_t> DiskReader::read_chunk(uint64_t offset, uint64_t size) {
    std::ifstream file(file_path, std::ios::binary);
    file.seekg(offset, std::ios::beg);
    std::vector<uint8_t> buffer(size);
    file.read(reinterpret_cast<char*>(buffer.data()), size);
    buffer.resize(file.gcount());
    return buffer;
}
