#ifndef SIGNATURESCANNER_HPP
#define SIGNATURESCANNER_HPP

#include <vector>
#include <string>

struct ArtifactInfo {
    std::string type;
    size_t offset;
};

class SignatureScanner {
public:
    static std::vector<ArtifactInfo> scan(const std::vector<unsigned char>& data, size_t base_offset);
};

#endif
