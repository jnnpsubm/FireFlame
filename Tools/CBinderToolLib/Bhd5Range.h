#pragma once
#include <iostream>

namespace CBinderToolLib{
class Bhd5Range
{
public:
    long StartOffset = 0;
    long GetStartOffset() const { return StartOffset; }
    void SetStartOffset(long offset) { StartOffset = offset; }

    long EndOffset = 0;
    long GetEndOffset() const { return EndOffset; }
    void SetEndOffset(long offset) { EndOffset = offset; }

    static Bhd5Range Read(std::istream& reader)
    {
        Bhd5Range result;
        std::int64_t val;

        reader.read((char*)&val, sizeof(val));
        result.StartOffset = (long)val;

        reader.read((char*)&val, sizeof(val));
        result.EndOffset = (long)val;
        return result;
    }
};
}
