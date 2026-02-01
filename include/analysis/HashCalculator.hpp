#ifndef HASHCALCULATOR_HPP
#define HASHCALCULATOR_HPP

#include <vector>
#include <string>

class HashCalculator {
public:
    static std::string compute_sha256(const std::vector<unsigned char>& data);
};

#endif

