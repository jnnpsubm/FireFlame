#pragma once
#include <string>
#include <vector>
#include <fstream>
#include "DSGameVersion.h"
#include "DSBhd5Bucket.h"

namespace DSFS {
class Bhd5File{
public:
    Bhd5File(const std::string& filePath, GameVersion version);

    int Decipher();
    int Parse();
    std::vector<Bhd5Bucket>& GetBuckets(){
        return _buckets;
    }

private:
    std::string mFileStream;
    std::string mFilePath;
    GameVersion mGameVersion;

    std::vector<Bhd5Bucket> _buckets;

    static const std::string Bhd5Signature; // "BHD5";
    static const unsigned char Bhd5UnknownConstant1 = 255;
    static const int Bh5UnknownConstant2 = 1;
};
}
