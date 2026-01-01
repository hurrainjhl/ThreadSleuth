#pragma once
#include <cstdint>
#include <vector>

struct Chunk {
    uint32_t id;
    uint64_t offset;
    std::vector<uint8_t> data; // Holds the actual file bytes

    Chunk() = default;
    Chunk(uint32_t i, uint64_t off, std::vector<uint8_t> d) 
        : id(i), offset(off), data(std::move(d)) {}
};
