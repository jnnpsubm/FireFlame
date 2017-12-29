#pragma once
#include <iostream>

namespace DSFS {
    class Bhd5Range{
    public:
        Bhd5Range(std::istream& reader) {
            reader.read((char*)&StartOffset, sizeof(StartOffset));
            reader.read((char*)&EndOffset, sizeof(EndOffset));
        }
        
        std::int64_t GetStartOffset() const { return StartOffset; }
        std::int64_t GetEndOffset() const { return EndOffset; }

    private:
        std::int64_t StartOffset = 0;
        std::int64_t EndOffset = 0;
    };
}
