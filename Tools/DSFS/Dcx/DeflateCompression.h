#pragma once
#include "DcxCompression.h"

namespace DSFS {
class DeflateCompression : public DcxCompression{
public:
    static const std::string DeflateSignature; // "DFLT";

public:
    DeflateCompression(std::istream& reader);
    
    std::string CompressData(const std::vector<std::uint8_t>& uncompressedData) override;
    std::string DecompressData(const std::vector<std::uint8_t>& compressedData) override;

private:
    int Level;
};
}
