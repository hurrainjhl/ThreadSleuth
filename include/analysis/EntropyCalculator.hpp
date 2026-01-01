#pragma once
#include <vector>
#include <cmath>
#include <map>
#include <cstdint> // <--- This fixes the 'uint8_t' error

class EntropyCalculator {
public:
    static double calculate(const std::vector<uint8_t>& data);
};

