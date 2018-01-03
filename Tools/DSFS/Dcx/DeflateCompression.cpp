#include "DeflateCompression.h"
#include "src\Utility\io\io.h"
#include "..\..\..\Tools_Ext\TinyDeflate\gunzip.hh"
#include "..\..\..\Tools_Ext\zlib-1.2.11\zlib.h"

namespace DSFS {
const std::string DeflateCompression::DeflateSignature = "DFLT";

DeflateCompression::DeflateCompression(std::istream& reader)
{
    unsigned headerSize = 0, level = 0;
    FireFlame::IO::read_type(reader, headerSize, level);
    FireFlame::IO::endian_swap(headerSize);
    FireFlame::IO::endian_swap(level);
    std::string skip(16, '\0');
    reader.read(&skip[0], 16);
    Level = (int)level;
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
    int failed_number = 0;
    std::string output(compressedData.size() * 2, '\0');
    while (failed_number < 10)
    {
        uLongf dst_len = (uLongf)output.size();
        if (uncompress((Bytef*)&output[0], &dst_len, compressedData.data(), (uLong)compressedData.size()) != Z_OK)
        {
            output.resize(output.size() * 2);
            failed_number++;
        }
        else
        {
            output.resize(dst_len);
            break;
        }
    }
    return output;
}
}