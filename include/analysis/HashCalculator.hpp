#pragma once
#include <vector>
#include <string>
#include <cstdint>

class HashCalculator {
public:
    static std::string sha256(const std::vector<uint8_t>& data);
};
