#pragma once
#include "DSBhd5BucketEntry.h"

namespace DSFS {
class Bhd5Bucket{
public:
    std::vector<Bhd5BucketEntry>& GetEntries(){
        return _entries;
    }

    Bhd5Bucket(std::istream& reader, GameVersion version){
        int bucketEntryCount = 0;
        int bucketOffset = 0;
        reader.read((char*)&bucketEntryCount, sizeof(int));
        reader.read((char*)&bucketOffset, sizeof(int));

        auto currentPosition = reader.tellg();
        reader.seekg(bucketOffset, std::ios::beg);
        for (int i = 0; i < bucketEntryCount; i++){
            _entries.emplace_back(reader, version);
        }
        reader.seekg(currentPosition, std::ios::beg);
    }

private:
    std::vector<Bhd5BucketEntry> _entries;
};
}
