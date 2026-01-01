#include "analysis/MetadataParser.hpp"
#include <iostream>
#include <algorithm>

std::string MetadataParser::parse_mft_filename(const std::vector<uint8_t>& buffer, size_t offset) {
    // Ensure we have enough data (MFT records are typically 1024 bytes)
    if (offset + 1024 > buffer.size()) return "";

    // 1. Verify "FILE0" Signature
    if (buffer[offset] != 'F' || buffer[offset+1] != 'I' || 
        buffer[offset+2] != 'L' || buffer[offset+3] != 'E') {
        return "";
    }

    // 2. Scan for Attribute 0x30 ($FILE_NAME)
    // We start looking after the standard header (approx 48 bytes in)
    for (size_t i = offset + 48; i < offset + 1000; ++i) {
        
        // Check for Attribute Type 0x30 (Little Endian: 30 00 00 00)
        if (buffer[i] == 0x30 && buffer[i+1] == 0x00 && buffer[i+2] == 0x00 && buffer[i+3] == 0x00) {
            
            // Found Attribute Header!
            // The attribute size is at offset +4 (4 bytes)
            uint32_t attr_size = buffer[i+4];
            
            // The content offset is at offset +20 (2 bytes)
            uint16_t content_offset = buffer[i+20];
            
            // Calculate absolute position of the content
            size_t content_start = i + content_offset;
            
            // Inside $FILE_NAME content:
            // Offset 64: Filename Length (in characters, not bytes)
            // Offset 66: Filename (UTF-16 LE)
            
            if (content_start + 66 > offset + 1024) continue;

            uint8_t name_len = buffer[content_start + 64];
            if (name_len == 0 || name_len > 255) continue;

            // Extract Name (Convert UTF-16 to ASCII/UTF-8 for simplicity)
            std::string filename = "";
            for (int j = 0; j < name_len; ++j) {
                // UTF-16 characters are 2 bytes. We take the first byte (ASCII approx).
                // Example: 'A' is 41 00. We grab 41.
                char c = buffer[content_start + 66 + (j * 2)];
                if (c >= 32 && c <= 126) {
                    filename += c;
                } else {
                    filename += '?'; 
                }
            }
            return filename;
        }
    }
    return "Unknown_File";
}

