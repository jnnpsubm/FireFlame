#pragma once
#include <string>
#include "DcxCompression.h"
#include "StreamEncoding.h"

namespace CBinderToolLib {
class DcxFile
{
public:
    static const std::string DcxSignature; // "DCX\0"
    static const int DcxSize = 76;

private:
    static const int DcxHeaderSize = 24;
    static const std::string DcsSignature; // "DCS\0"
    static const std::string DcpSignature; // "DCP\0"
    static const std::string DcaSignature; // "DCA\0"
    static const int DcaHeaderSize = 8;

public:
    DcxFile()
    {
    }

private:
    DcxFile(std::shared_ptr<DcxCompression> compression, const std::vector<std::uint8_t>& compressedData)
    {
        Compression = compression;
        CompressedData = compressedData;
    }

    std::shared_ptr<DcxCompression> Compression = nullptr;
    int CompressedSize;
    int UncompressedSize;

public:
    std::vector<std::uint8_t> CompressedData;
    void Read(const std::string& inputStream);

    static int ReadCompressedSize(std::istream& inputStream);

    std::string Decompress()
    {
        return Compression->DecompressData(CompressedData);
    }

private:
    void ReadCommonHeader(std::istream& inputStream, Encoding encoding, Endian endian);
    void ReadCompressionHeader(std::istream& reader);
};
}
