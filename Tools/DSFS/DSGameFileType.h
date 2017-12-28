#pragma once
#include "DSFileType.h"
#include "DSGameVersion.h"

namespace DSFS {
    struct GameFileType {
        FileType fileType = FileType::Unknown;
        GameVersion gameVersion = GameVersion::Common;
    };

    extern GameFileType GetGameFileType(const std::string& fileName);
    extern std::string GetGameFileTypeStr(const GameFileType& type);
}