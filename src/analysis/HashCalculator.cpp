#include "analysis/HashCalculator.hpp"
#include <openssl/sha.h>
#include <sstream>
#include <iomanip>

std::string HashCalculator::sha256(const std::vector<uint8_t>& data) {
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256(data.data(), data.size(), hash);
    std::ostringstream out;
    for (int i = 0; i < SHA256_DIGEST_LENGTH; i++)
        out << std::hex << std::setw(2) << std::setfill('0') << (int)hash[i];
    return out.str();
}
