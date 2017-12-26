#pragma once
#include <string>
#include <vector>
#include <array>
#include "Bhd5Bucket.h"

namespace CBinderToolLib{
class Bhd5File
{
private:
    static const std::string Bhd5Signature; // "BHD5";
    static const unsigned char Bhd5UnknownConstant1 = 255;
    static const int Bh5UnknownConstant2 = 1;

    std::vector<Bhd5Bucket> _buckets;

public:
    std::vector<Bhd5Bucket>& GetBuckets()
    {
        return _buckets;
    }

    static std::unique_ptr<Bhd5File> Read(std::istream& inputStream, GameVersion version);
};
}
