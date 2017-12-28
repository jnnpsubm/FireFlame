#pragma once
#include <string>

namespace DSFS {
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
        case DSFS::FileType::Unknown:
            return "Unknown";
        case DSFS::FileType::Regulation:
            return "Regulation";
        case DSFS::FileType::Dcx:
            return "Dcx";
        case DSFS::FileType::EncryptedBdt:
            return "EncryptedBdt";
        case DSFS::FileType::EncryptedBhd:
            return "EncryptedBhd";
        case DSFS::FileType::Bdt:
            return "Bdt";
        case DSFS::FileType::Bhd:
            return "Bhd";
        case DSFS::FileType::Bnd:
            return "Bnd";
        case DSFS::FileType::Savegame:
            return "Savegame";
        case DSFS::FileType::Tpf:
            return "Tpf";
        case DSFS::FileType::Param:
            return "Param";
        case DSFS::FileType::Fmg:
            return "Fmg";
        default:
            return "Unknown";
        }
    }
}

