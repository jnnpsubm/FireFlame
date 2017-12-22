#pragma once
#include "GameVersion.h"
#include "FileType.h"
#include <utility>
#include <string>

namespace CBinderToolLib {
class Utils
{
public:
    static std::pair<FileType, GameVersion> GetFileType(const std::string& fileName);
    static std::string GetFileTypeDescription(const std::pair<FileType, GameVersion>& fileType);
};

}

