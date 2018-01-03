#pragma once
#include "DSFileType.h"
#include "DSGameVersion.h"

namespace DSFS {
    struct GameFileType {
        GameFileType() = default;
        GameFileType(FileType fileType, GameVersion gameVersion)
            : fileType(fileType), gameVersion(gameVersion) {}
        FileType fileType = FileType::Unknown;
        GameVersion gameVersion = GameVersion::Common;
    };

    extern GameFileType GetGameFileType(const std::string& fileName);
    extern std::string GetGameFileTypeStr(const GameFileType& type);
}