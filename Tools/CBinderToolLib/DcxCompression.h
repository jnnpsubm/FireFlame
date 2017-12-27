#pragma once
#include <memory>
#include <vector>

namespace CBinderToolLib{
class DcxCompression
{
public:
    virtual std::unique_ptr<std::istream> CompressData(std::vector<std::uint8_t> uncompressedData) = 0;
    virtual std::unique_ptr<std::istream> DecompressData(std::vector<std::uint8_t> compressedData) = 0;
};
}
