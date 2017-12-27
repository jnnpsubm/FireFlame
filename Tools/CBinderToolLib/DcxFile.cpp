#include "DcxFile.h"
#include <assert.h>
#include "FireFlameHeader.h"

namespace CBinderToolLib {
const std::string DcxFile::DcxSignature = { 'D','C','X','\0' };
const std::string DcxFile::DcsSignature = { 'D','C','S','\0' };
const std::string DcxFile::DcpSignature = { 'D','C','P','\0' };
const std::string DcxFile::DcaSignature = { 'D','C','A','\0' };

int DcxFile::ReadCompressedSize(std::istream& inputStream)
{
    DcxFile result;
    result.ReadCommonHeader(inputStream, Encoding::UTF8, Endian::Big);
    return result.CompressedSize;
}

void DcxFile::ReadCommonHeader(std::istream& inputStream, Encoding encoding, Endian endian)
{
    assert(encoding == Encoding::UTF8);
    assert(endian == Endian::Big);

    std::string signature(4,'\0');
    inputStream.read(&signature[0], 4);
    if (signature != DcxSignature)
        throw std::runtime_error("Signature was not DCX");

    inputStream.read(&signature[0], 4);

    unsigned int dcxHeaderSize = 0;
    FireFlame::IO::read_type(inputStream, dcxHeaderSize);
    FireFlame::IO::endian_swap(dcxHeaderSize);
    if (dcxHeaderSize != DcxHeaderSize)
        throw std::runtime_error("Unsupported DCX header size.");

    signature.resize(12);
    inputStream.read(&signature[0], 12);

    signature.resize(4);
    inputStream.read(&signature[0], 4);
    if (signature != DcsSignature)
        throw std::runtime_error("Signature was not DCS");

    unsigned int size1, size2;
    FireFlame::IO::read_type(inputStream, size1, size2);
    FireFlame::IO::endian_swap(size1);
    FireFlame::IO::endian_swap(size2);
    UncompressedSize = (int)size1;
    CompressedSize = (int)size2;
}
}