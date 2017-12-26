#include "Bhd5File.h"
#include "FireFlameHeader.h"

namespace CBinderToolLib {
const std::string Bhd5File::Bhd5Signature = "BHD5";

std::unique_ptr<Bhd5File> Bhd5File::Read(std::istream& inputStream, GameVersion version)
{
    std::unique_ptr<Bhd5File> result = std::make_unique<Bhd5File>();

    std::string signature(4,'\0');
    inputStream.read((char*)signature.data(), 4);
    if (signature != Bhd5Signature)
        throw std::runtime_error("Invalid signature");

    int bhdVersion = 0; // 511
    int unknown = 0;    // 1
    int size = 0;       // excluding sizeof(signature)
    int bucketDirectoryEntryCount = 0;
    int bucketDirectoryOffset = 0;
    int saltLength = 0;
    FireFlame::IO::read_type
    (
        inputStream, bhdVersion, unknown, size,
        bucketDirectoryEntryCount, bucketDirectoryOffset,
        saltLength
    );

    std::string salt(saltLength, '\0');
    inputStream.read((char*)salt.data(), saltLength);
    for (int i = 0; i < bucketDirectoryEntryCount; i++)
    {
        result->_buckets.emplace_back(Bhd5Bucket::Read(inputStream, version));
    }
    return result;
}
}