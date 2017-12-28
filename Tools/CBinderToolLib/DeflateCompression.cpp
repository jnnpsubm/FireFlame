#include "DeflateCompression.h"
#include "FireFlameHeader.h"
#include "..\..\Tools_Ext\TinyDeflate\gunzip.hh"

namespace CBinderToolLib {
const std::string DeflateCompression::DeflateSignature = "DFLT";

DeflateCompression* DeflateCompression::Read(std::istream& reader)
{
    DeflateCompression* result = new DeflateCompression();
    unsigned headerSize = 0, level = 0;
    FireFlame::IO::read_type(reader, headerSize, level);
    FireFlame::IO::endian_swap(headerSize);
    FireFlame::IO::endian_swap(level);
    std::string skip(16, '\0');
    reader.read(&skip[0], 16);
    result->Level = (int)level;
    return result;
}

std::string DeflateCompression::CompressData(const std::vector<std::uint8_t>& uncompressedData)
{
    return nullptr;
    /*MemoryStream compressedBufferStream = new MemoryStream();
    using (DeflaterOutputStream deflaterStream = new DeflaterOutputStream(compressedBufferStream))
    {
        deflaterStream.Write(uncompressedData, 0, uncompressedData.Length);
    }
    return compressedBufferStream;*/
}

std::string DeflateCompression::DecompressData(const std::vector<std::uint8_t>& compressedData)
{
    std::string output(compressedData.size() * 2,'\0');
    int cnt = 0;
    while (cnt++ < 10)
    {
        auto result = Deflate(compressedData.data(), &output[0], output.size(), DeflateTrackOutSize{});
        if (result.first == 0)
        {
            output.resize(result.second);
        }
        else
        {
            output.resize(output.size() * 2);
        }
    }
    return output;
}
}