#pragma once
#include <memory>
#include <vector>

namespace CBinderToolLib{
class DcxCompression
{
public:
    virtual std::string CompressData(const std::vector<std::uint8_t>& uncompressedData) = 0;
    virtual std::string DecompressData(const std::vector<std::uint8_t>& compressedData) = 0;
};
}
