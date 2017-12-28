#pragma once
#include <string>

namespace DSFS {
    enum class GameVersion
    {
        Common,
        DarkSouls2,
        DarkSouls3
    };

    inline std::string GetGameVersionStr(GameVersion version)
    {
        switch (version)
        {
        case DSFS::GameVersion::Common:
            return "Common";
        case DSFS::GameVersion::DarkSouls2:
            return "DarkSouls2";
        case DSFS::GameVersion::DarkSouls3:
            return "DarkSouls3";
        default:
            return "Common";
        }
    }
}