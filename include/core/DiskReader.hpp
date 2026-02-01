#ifndef DISKREADER_HPP
#define DISKREADER_HPP

#include <string>
#include <fstream>
#include <vector>

class DiskReader {
public:
    DiskReader(const std::string& path, size_t chunk_size);
    std::vector<unsigned char> read_next_chunk();

private:
    std::string file_path;
    size_t chunk_size;
    std::ifstream file_stream;
};

#endif
