#pragma once
#include <vector>
#include "Bhd5Range.h"

namespace CBinderToolLib{
class Bhd5SaltedShaHash
{
public:
    std::vector<std::uint8_t> Hash;
    std::vector<Bhd5Range> Ranges;

    static Bhd5SaltedShaHash* Read(std::istream& reader)
    {
        Bhd5SaltedShaHash* result = new Bhd5SaltedShaHash();

        result->Hash.resize(32);
        reader.read((char*)result->Hash.data(), 32);

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
