#include "DSBhd5BucketEntry.h"
#include "src\Utility\io\io.h"

namespace DSFS {
Bhd5BucketEntry::Bhd5BucketEntry(std::istream& reader, GameVersion version){
    std::int64_t saltedHashOffset = 0;
    std::int64_t aesKeyOffset = 0;
    FireFlame::IO::read_type(reader, FileNameHash);
    FireFlame::IO::read_type(reader, PaddedFileSize);
    FireFlame::IO::read_type(reader, FileOffset);
    FireFlame::IO::read_type(reader, saltedHashOffset);
    FireFlame::IO::read_type(reader, aesKeyOffset);

    switch (version){
    case GameVersion::DarkSouls3:
        FireFlame::IO::read_type(reader, FileSize);
        break;
    default:
        FileSize = PaddedFileSize;
        break;
    }

    if (saltedHashOffset != 0){
        auto currentPosition = reader.tellg();
        reader.seekg(saltedHashOffset, std::ios::beg);
        ShaHash.reset(new Bhd5SaltedShaHash(reader));
        reader.seekg(currentPosition, std::ios::beg);
    }
    if (aesKeyOffset != 0){
        auto currentPosition = reader.tellg();
        reader.seekg(aesKeyOffset, std::ios::beg);
        Aes.reset(new Bhd5Aes(reader));
        reader.seekg(currentPosition, std::ios::beg);
    }
}
}