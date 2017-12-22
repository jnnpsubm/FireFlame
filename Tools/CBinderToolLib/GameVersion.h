#pragma once
#include <string>

namespace CBinderToolLib {
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
    case CBinderToolLib::GameVersion::Common:
        return "Common";
    case CBinderToolLib::GameVersion::DarkSouls2:
        return "DarkSouls2";
    case CBinderToolLib::GameVersion::DarkSouls3:
        return "DarkSouls3";
    default:
        return "Common";
    }
}
}