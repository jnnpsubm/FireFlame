#include "DcxFile.h"
#include <assert.h>
#include "src\Utility\io\io.h"
#include "DeflateCompression.h"

namespace DSFS {
const std::string DcxFile::DcxSignature = { 'D','C','X','\0' };
const std::string DcxFile::DcsSignature = { 'D','C','S','\0' };
const std::string DcxFile::DcpSignature = { 'D','C','P','\0' };
const std::string DcxFile::DcaSignature = { 'D','C','A','\0' };

DcxFile::DcxFile(std::istream& inputStream)
{
    ReadCommonHeader(inputStream);
    ReadCompressionHeader(inputStream);
    CompressedData.resize(CompressedSize);
    inputStream.read((char*)CompressedData.data(), CompressedSize);
}

int DcxFile::ReadCompressedSize(std::istream& inputStream)
{
    DcxFile dcxFile;
    dcxFile.ReadCommonHeader(inputStream);
    return dcxFile.CompressedSize;
}

void DcxFile::ReadCompressionHeader(std::istream& reader)
{
    std::string signature(4,'\0');
    reader.read(&signature[0], 4);
    if (signature != DcpSignature)
        throw std::runtime_error("Signature was not DCP");

    reader.read(&signature[0], 4);
    if (signature != DeflateCompression::DeflateSignature)
        throw std::runtime_error("Compression not implemented ({signature})");

    Compression.reset(new DeflateCompression(reader));

    reader.read(&signature[0], 4);
    if (signature != DcaSignature)
        throw std::runtime_error("Signature was not DCA");

    unsigned dcaHeaderSize = 0;
    FireFlame::IO::read_type(reader, dcaHeaderSize);
    FireFlame::IO::endian_swap(dcaHeaderSize);
    if (dcaHeaderSize != DcaHeaderSize)
        throw std::runtime_error("Unsupported DCA header size.");
}

void DcxFile::ReadCommonHeader(std::istream& inputStream)
{
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

    FireFlame::IO::read_type(inputStream, UncompressedSize, CompressedSize);
    FireFlame::IO::endian_swap(UncompressedSize);
    FireFlame::IO::endian_swap(CompressedSize);
}
}