#pragma once
#include <string>
#include "DcxCompression.h"

namespace DSFS {
class DcxFile{
public:
    static const std::string DcxSignature; // "DCX\0"
    static const int DcxSize = 76;

public:
    DcxFile() {}
    DcxFile(std::istream& inputStream);

    std::string Decompress() {
        return Compression->DecompressData(CompressedData);
    }

public:
    static int ReadCompressedSize(std::istream& inputStream);

private:
    void ReadCommonHeader(std::istream& inputStream);
    void ReadCompressionHeader(std::istream& reader);

    static const int DcxHeaderSize = 24;
    static const std::string DcsSignature; // "DCS\0"
    static const std::string DcpSignature; // "DCP\0"
    static const std::string DcaSignature; // "DCA\0"
    static const int DcaHeaderSize = 8;

    std::vector<std::uint8_t> CompressedData;
    std::shared_ptr<DcxCompression> Compression = nullptr;
    unsigned CompressedSize;
    unsigned UncompressedSize;
};
}
