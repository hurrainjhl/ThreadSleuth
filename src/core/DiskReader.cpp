#include "../../include/core/DiskReader.hpp"
#include <iostream>

DiskReader::DiskReader(const std::string& path, size_t chunk_size) 
    : file_path(path), chunk_size(chunk_size) {
    file_stream.open(path, std::ios::binary);
}

std::vector<unsigned char> DiskReader::read_next_chunk() {
    if (!file_stream.is_open() || file_stream.eof()) {
        return {};
    }

    std::vector<unsigned char> buffer(chunk_size);
    file_stream.read(reinterpret_cast<char*>(buffer.data()), chunk_size);
    
    size_t bytes_read = file_stream.gcount();
    if (bytes_read == 0) return {};

    buffer.resize(bytes_read);
    return buffer;
}
