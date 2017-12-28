#pragma once
#include <fstream>
#include <string>

namespace DSFS {
    class Bdt5FileStream
    {
    public:
        Bdt5FileStream(const std::string& path)
        {
            _inputStream.open(path, std::ios::in | std::ios::binary);
            if (!_inputStream)
                throw std::runtime_error("Bdt5FileStream open failed......");
            _inputStream.seekg(0, std::ios::end);
            _length = _inputStream.tellg();
            _inputStream.seekg(0, std::ios::beg);
        }

        std::string Read(long fileOffset, long fileSize)
        {
            if (fileOffset + fileSize > _length)
                throw std::runtime_error("Bdt5FileStream Read overflow......");
            _inputStream.seekg(fileOffset, std::ios::beg);
            std::string bytes(fileSize, '\0');
            _inputStream.read(&bytes[0], fileSize);
            return bytes;
        }

    private:
        std::ifstream _inputStream;
        std::streampos _length;
    };
}

