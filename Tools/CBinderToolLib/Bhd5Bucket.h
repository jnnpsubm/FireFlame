#pragma once
#include "Bhd5BucketEntry.h"

namespace CBinderToolLib{
class Bhd5Bucket
{
private:
    std::vector<Bhd5BucketEntry> _entries;

public:
    Bhd5Bucket() = default;

    std::vector<Bhd5BucketEntry>& GetEntries()
    {
        return _entries;
    }

    static Bhd5Bucket Read(std::istream& reader, GameVersion version)
    {
        Bhd5Bucket result;

        int bucketEntryCount = 0;
        int bucketOffset = 0;
        reader.read((char*)&bucketEntryCount, sizeof(int));
        reader.read((char*)&bucketOffset, sizeof(int));

        auto currentPosition = reader.tellg();
        reader.seekg(bucketOffset, std::ios::beg);

        for (int i = 0; i < bucketEntryCount; i++)
        {
            result._entries.push_back(Bhd5BucketEntry::Read(reader, version));
        }
        reader.seekg(currentPosition, std::ios::beg);
        return result;
    }
};
}
