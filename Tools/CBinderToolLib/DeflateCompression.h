#pragma once
#include "DcxCompression.h"

namespace CBinderToolLib {
class DeflateCompression : public DcxCompression
{
public:
    static const std::string DeflateSignature; // "DFLT";

    int Level;
    std::string CompressData(const std::vector<std::uint8_t>& uncompressedData) override;
    std::string DecompressData(const std::vector<std::uint8_t>& compressedData) override;

    static DeflateCompression* Read(std::istream& reader);
};
}
