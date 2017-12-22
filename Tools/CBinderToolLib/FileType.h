#pragma once
#include <string>

namespace CBinderToolLib {
enum class FileType
{
    Unknown,
    Regulation,
    Dcx,
    EncryptedBdt,
    EncryptedBhd,
    Bdt,
    Bhd,
    Bnd,
    Savegame,
    Tpf,
    Param,
    Fmg
};
inline std::string GetFileTypeStr(FileType type)
{
    switch (type)
    {
    case CBinderToolLib::FileType::Unknown:
        return "Unknown";
    case CBinderToolLib::FileType::Regulation:
        return "Regulation";
    case CBinderToolLib::FileType::Dcx:
        return "Dcx";
    case CBinderToolLib::FileType::EncryptedBdt:
        return "EncryptedBdt";
    case CBinderToolLib::FileType::EncryptedBhd:
        return "EncryptedBhd";
    case CBinderToolLib::FileType::Bdt:
        return "Bdt";
    case CBinderToolLib::FileType::Bhd:
        return "Bhd";
    case CBinderToolLib::FileType::Bnd:
        return "Bnd";
    case CBinderToolLib::FileType::Savegame:
        return "Savegame";
    case CBinderToolLib::FileType::Tpf:
        return "Tpf";
    case CBinderToolLib::FileType::Param:
        return "Param";
    case CBinderToolLib::FileType::Fmg:
        return "Fmg";
    default:
        return "Unknown";
    }
}
}

