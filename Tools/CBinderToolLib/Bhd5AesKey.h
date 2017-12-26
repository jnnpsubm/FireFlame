#pragma once
#include <vector>
#include "Bhd5Range.h"

namespace CBinderToolLib{
class Bhd5AesKey
{
public:
    std::vector<std::uint8_t> Key;
    std::vector<Bhd5Range> Ranges;

    static Bhd5AesKey* Read(std::istream& reader)
    {
        Bhd5AesKey* result = new Bhd5AesKey();

        result->Key.resize(16);
        reader.read((char*)result->Key.data(), 16);

        std::int32_t rangeCount = 0;
        reader.read((char*)&rangeCount, sizeof(rangeCount));
        for (int i = 0; i < rangeCount; i++)
        {
            result->Ranges.push_back(Bhd5Range::Read(reader));
        }
        return result;
    }
};
}
