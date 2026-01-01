#pragma once
#include <vector>
#include <string>
#include <cstdint>

struct SignatureMatch {
    std::string type;
    size_t offset;
};

class SignatureScanner {
public:
    static std::vector<SignatureMatch> scan(const std::vector<uint8_t>& data, size_t baseOffset);
};
