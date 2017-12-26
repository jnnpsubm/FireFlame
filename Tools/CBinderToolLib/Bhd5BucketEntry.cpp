#include "Bhd5BucketEntry.h"
#include "FireFlameHeader.h"

namespace CBinderToolLib {
Bhd5BucketEntry Bhd5BucketEntry::Read(std::istream& reader, GameVersion version)
{
    Bhd5BucketEntry result;

    std::int64_t saltedHashOffset = 0;
    std::int64_t aesKeyOffset = 0;
    FireFlame::IO::read_type(reader, result.FileNameHash);
    FireFlame::IO::read_type(reader, result.PaddedFileSize);
    FireFlame::IO::read_type(reader, result.FileOffset);
    FireFlame::IO::read_type(reader, saltedHashOffset);
    FireFlame::IO::read_type(reader, aesKeyOffset);

    switch (version)
    {
    case GameVersion::DarkSouls3:
        FireFlame::IO::read_type(reader, result.FileSize);
        break;
    default:
        result.FileSize = result.PaddedFileSize;
        break;
    }

    if (saltedHashOffset != 0)
    {
        auto currentPosition = reader.tellg();
        reader.seekg(saltedHashOffset, std::ios::beg);
        result.ShaHash.reset(Bhd5SaltedShaHash::Read(reader));
        reader.seekg(currentPosition, std::ios::beg);
    }
    if (aesKeyOffset != 0)
    {
        auto currentPosition = reader.tellg();
        reader.seekg(aesKeyOffset, std::ios::beg);
        result.AesKey.reset(Bhd5AesKey::Read(reader));
        reader.seekg(currentPosition, std::ios::beg);
    }

    return result;
}
}