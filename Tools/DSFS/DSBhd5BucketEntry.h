#pragma once
#include "DSBhd5Aes.h"
#include "DSBhd5SaltedShaHash.h"
#include "DSGameVersion.h"
#include <memory>

namespace DSFS {
class Bhd5BucketEntry{
public:
    Bhd5BucketEntry(std::istream& reader, GameVersion version);

    unsigned GetFileNameHash() const { return FileNameHash; }
    std::int64_t GetFileOffset() const { return FileOffset; }
    std::int64_t GetFileSize() const { return FileSize; }
    long GetPaddedFileSize() const { return PaddedFileSize; }
        
    bool IsEncrypted() const { return Aes != nullptr; }

private:
    unsigned FileNameHash = 0;
    std::int64_t FileOffset = 0;
    std::int64_t FileSize = 0;
    long PaddedFileSize = 0;

    std::unique_ptr<Bhd5Aes>           Aes     = nullptr;
    std::unique_ptr<Bhd5SaltedShaHash> ShaHash = nullptr;
};
}

