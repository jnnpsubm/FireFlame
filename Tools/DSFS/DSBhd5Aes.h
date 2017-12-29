#pragma once
#include <vector>
#include "DSBhd5Range.h"

namespace DSFS {
    class Bhd5Aes{
    public:
        Bhd5Aes(std::istream& reader) {
            Key.resize(16);
            reader.read((char*)Key.data(), 16);

            std::int32_t rangeCount = 0;
            reader.read((char*)&rangeCount, sizeof(rangeCount));
            for (int i = 0; i < rangeCount; i++){
                Ranges.emplace_back(reader);
            }
        }

    private:
        std::vector<std::uint8_t> Key;
        std::vector<Bhd5Range> Ranges;
    };
}
