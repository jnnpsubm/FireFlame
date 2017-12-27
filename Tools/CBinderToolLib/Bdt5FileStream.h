#pragma once
#include <fstream>
#include <sstream>
#include <memory>

namespace CBinderToolLib{
class Bdt5FileStream
{
private:
    std::unique_ptr<std::ifstream> _inputStream;
    std::streampos _length;

public:
    Bdt5FileStream(std::ifstream* inputStream) :_inputStream(inputStream) {
        _inputStream->seekg(0, std::ios::end);
        _length = _inputStream->tellg();
        _inputStream->seekg(0, std::ios::beg);
    }
    ~Bdt5FileStream() = default;

    std::string Read(long fileOffset, long fileSize)
    {
        if (fileOffset + fileSize > _length)
            throw std::runtime_error("Bdt5FileStream Read over end......");
        _inputStream->seekg(fileOffset, std::ios::beg);
        std::string bytes(fileSize, '\0');
        _inputStream->read(&bytes[0], fileSize);
        return bytes;
    }

    static Bdt5FileStream* OpenFile(const std::string& path)
    {
        std::ifstream* fileStream = new std::ifstream(path, std::ios::in | std::ios::binary);
        return new Bdt5FileStream(fileStream);
    }
};
}

