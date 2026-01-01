#include "analysis/EntropyCalculator.hpp"
#include <cmath>
#include <iostream>

double EntropyCalculator::calculate(const std::vector<uint8_t>& data) {
    if (data.empty()) return 0.0;

    std::map<uint8_t, size_t> frequencies;
    for (uint8_t byte : data) {
        frequencies[byte]++;
    }

    double entropy = 0.0;
    double total_len = static_cast<double>(data.size());

    for (auto const& [key, count] : frequencies) {
        double p = count / total_len;
        entropy -= p * std::log2(p);
    }

    return entropy;
}

