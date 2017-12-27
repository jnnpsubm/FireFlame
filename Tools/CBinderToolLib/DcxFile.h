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

    std::shared_ptr<DcxCompression> Compression;
    int CompressedSize;
    int UncompressedSize;

public:
    std::vector<std::uint8_t> CompressedData;

    /*static DcxFile Read(Stream inputStream)
    {
        DcxFile result = new DcxFile();
        BigEndianBinaryReader reader = new BigEndianBinaryReader(inputStream, Encoding.UTF8, true);
        result.ReadCommonHeader(reader);
        result.ReadCompressionHeader(reader);
        result.CompressedData = reader.ReadBytes(result.CompressedSize);
        return result;
    }*/

public:
    static int ReadCompressedSize(std::istream& inputStream);

private:
    void ReadCommonHeader(std::istream& inputStream, Encoding encoding, Endian endian);

    /*private void ReadCompressionHeader(BinaryReader reader)
    {
        string signature = reader.ReadString(4);
        if (signature != DcpSignature)
            throw new Exception("Signature was not DCP");
        signature = reader.ReadString(4);
        if (signature != DeflateCompression.DeflateSignature)
            throw new NotImplementedException($"Compression not implemented ({signature}) ");

        Compression = DeflateCompression.Read(reader);

        signature = reader.ReadString(4);
        if (signature != DcaSignature)
            throw new Exception("Signature was not DCA");
        int dcaHeaderSize = reader.ReadInt32();
        if (dcaHeaderSize != DcaHeaderSize)
            throw new Exception("Unsupported DCA header size.");
    }

    public byte[] Decompress()
    {
        return Compression.DecompressData(CompressedData).ToArray();
    }*/
};
}
