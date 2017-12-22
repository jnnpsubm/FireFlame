#include "Options.h"
#include "FireFlameHeader.h"
#include <io.h>
#include <regex>

namespace CBinderTool {
Options::Options()
{
}


Options::~Options()
{
}

std::unique_ptr<Options> Options::Parse(int argc, char* argv[])
{
    auto options = std::make_unique<Options>();
    if (argc <= 1)
    {
        throw std::exception("Missing arguments");
    }

    options->mInputPath = argv[1];
    if ((_access(options->mInputPath.c_str(), 0)) == -1)
    {
        throw std::exception("Input file not found");
    }

    auto fileType = GetFileType(FireFlame::StringUtils::file_name(options->mInputPath));
    /*options.InputType = fileType.type;
    options.InputGameVersion = fileType.version;

    if (options.InputType == FileType.Unknown)
    {
        throw new FormatException("Unsupported input file format");
    }

    if (args.Length >= 2)
    {
        options.OutputPath = args[1];
    }
    else
    {
        options.OutputPath = Path.Combine(
            Path.GetDirectoryName(options.InputPath),
            Path.GetFileNameWithoutExtension(options.InputPath));
        switch (options.InputType)
        {
        case FileType.EncryptedBhd:
            options.OutputPath += "_decrypted.bhd";
            break;
        case FileType.Dcx:
        case FileType.Fmg:
            break;
        }
    }*/

    return options;
}

std::pair<FileType, GameVersion> Options::GetFileType(const std::string& fileName)
{
    if (fileName.empty())
    {
        throw std::runtime_error("empty file name......");
    }

    if (fileName == "Data0.bdt")
    {
        return { FileType::Regulation, GameVersion::DarkSouls3 };
    }

    if (fileName == "enc_regulation.bnd.dcx")
    {
        return { FileType::Regulation, GameVersion::DarkSouls2 };
    }

    // file.dcx file.bnd.dcx
    if (FireFlame::StringUtils::ends_with(fileName, ".dcx", true))
    {
        return { FileType::Dcx, GameVersion::Common };
    }

    // .anibnd .chrbnd .chrtpfbhd .mtdbnd .shaderbnd .objbnd .partsbnd .rumblebnd .hkxbhd .tpfbhd
    // .shaderbdle .shaderbdledebug
    if (FireFlame::StringUtils::ends_with(fileName, "bnd", true) ||
        FireFlame::StringUtils::ends_with(fileName, "bdle", true) ||
        FireFlame::StringUtils::ends_with(fileName, "bdledebug", true))
    {
        return { FileType::Bnd, GameVersion::Common };
    }

    // DS30000.sl2
    if (std::regex_match(fileName, std::regex("^DS3\\d+.*\\.sl2",std::regex::icase)))
    {
        return { FileType::Savegame, GameVersion::DarkSouls3 };
    }

    // DARKSII0000.sl2
    if (std::regex_match(fileName, std::regex("^DARKSII\\d+.*\\.sl2",std::regex::icase)))
    {
        return { FileType::Savegame, GameVersion::DarkSouls2 };
    }

    if (std::regex_match(fileName, std::regex("^(?:Data|DLC)\\d\\.bdt$", std::regex::icase)))
    {
        return { FileType::EncryptedBdt, GameVersion::DarkSouls3 };
    }

    if (std::regex_match(fileName, std::regex("^[^\\W_]+Ebl\\.bdt$", std::regex::icase)))
    {
        return { FileType::EncryptedBdt, GameVersion::DarkSouls2 };
    }

    if (std::regex_match(fileName, std::regex("^(?:Data|DLC|)\\d\\.bhd$", std::regex::icase)))
    {
        return { FileType::EncryptedBhd, GameVersion::DarkSouls3 };
    }

    if (std::regex_match(fileName, std::regex("^[^\\W_]+Ebl\\.bhd$", std::regex::icase)))
    {
        return { FileType::EncryptedBhd, GameVersion::DarkSouls2 };
    }

    // file.bdt file.hkxbdt file.tpfbdt
    if (FireFlame::StringUtils::ends_with(fileName, "bdt", true))
    {
        return { FileType::Bdt, GameVersion::Common };
    }

    // file.bhd file.hkxbhd file.tpfbhd
    if (FireFlame::StringUtils::ends_with(fileName, "bhd", true))
    {
        return { FileType::Bhd, GameVersion::Common };
    }

    if (FireFlame::StringUtils::ends_with(fileName, ".tpf", true))
    {
        return { FileType::Tpf, GameVersion::Common };
    }

    if (FireFlame::StringUtils::ends_with(fileName, ".param", true))
    {
        return { FileType::Param, GameVersion::Common };
    }

    if (FireFlame::StringUtils::ends_with(fileName, ".fmg", true))
    {
        return { FileType::Fmg, GameVersion::Common };
    }

    return { FileType::Unknown, GameVersion::Common };
}
}


