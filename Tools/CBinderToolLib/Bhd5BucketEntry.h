#pragma once
#include "Bhd5AesKey.h"
#include "Bhd5SaltedShaHash.h"
#include "GameVersion.h"
#include <memory>

namespace CBinderToolLib{
class Bhd5BucketEntry
{
public:
    unsigned FileNameHash = 0;
    unsigned GetFileNameHash() const { return FileNameHash; }
private:
    void SetFileNameHash(unsigned hash) { FileNameHash = hash; }

public:
    std::int64_t FileSize = 0;
    std::int64_t GetFileSize() const { return FileSize; }
    void SetFileSize(std::int64_t size) { FileSize = size; }

    long PaddedFileSize = 0;
    long GetPaddedFileSize() const { return PaddedFileSize; }
    void SetPaddedFileSize(long size) { PaddedFileSize = size; }

public:
    std::int64_t FileOffset = 0;
    std::int64_t GetFileOffset() const { return FileOffset; }
private:
    void SetFileOffset(std::int64_t offset) { FileOffset = offset; }

public:
    std::unique_ptr<Bhd5AesKey>        AesKey = nullptr;
    std::unique_ptr<Bhd5SaltedShaHash> ShaHash = nullptr;

    bool IsEncrypted() const { return AesKey != nullptr; }

    static Bhd5BucketEntry Read(std::istream& reader, GameVersion version);
};
}

