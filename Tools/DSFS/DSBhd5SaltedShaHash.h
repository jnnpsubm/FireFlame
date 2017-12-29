#pragma once
#include <vector>
#include "DSBhd5Range.h"

namespace DSFS {
    class Bhd5SaltedShaHash{
    public:
        Bhd5SaltedShaHash(std::istream& reader){
            Hash.resize(32);
            reader.read((char*)Hash.data(), 32);

            std::int32_t rangeCount = 0;
            reader.read((char*)&rangeCount, sizeof(rangeCount));
            for (int i = 0; i < rangeCount; i++){
                Ranges.emplace_back(reader);
            }
        }

    private:
        std::vector<std::uint8_t> Hash;
        std::vector<Bhd5Range> Ranges;
    };
}
