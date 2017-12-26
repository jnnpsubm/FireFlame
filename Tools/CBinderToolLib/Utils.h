#pragma once
#include "GameVersion.h"
#include "FileType.h"
#include "Bhd5BucketEntry.h"
#include "Bdt5FileStream.h"
#include <utility>
#include <string>
#include <memory>

namespace CBinderToolLib {
class Utils
{
public:
    static bool TryReadFileSize(Bhd5BucketEntry& entry, Bdt5FileStream& bdtStream, long& fileSize);

    static std::pair<FileType, GameVersion> GetFileType(const std::string& fileName);
    static std::string GetFileTypeDescription(const std::pair<FileType, GameVersion>& fileType);

    static std::istream* DecryptBhdFile(const std::string& filePath, GameVersion version);
};

}

