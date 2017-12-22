#include "Options.h"
#include "FireFlameHeader.h"
#include "..\CBinderToolLib\Utils.h"
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

    auto fileType = CBinderToolLib::Utils::GetFileType(FireFlame::StringUtils::file_name(options->mInputPath));
    options->mInputType = fileType.first;
    options->mInputGameVersion = fileType.second;

    if (options->mInputType == CBinderToolLib::FileType::Unknown)
    {
        throw std::exception("Unsupported input file format");
    }

    if (argc >= 3)
    {
        options->mOutputPath = argv[2];
    }
    else
    {
        options->mOutputPath = FireFlame::StringUtils::combine_path
        (
            FireFlame::StringUtils::dir_name(options->mInputPath),
            FireFlame::StringUtils::file_name_noext(options->mInputPath)
        );
        switch (options->mInputType)
        {
        case CBinderToolLib::FileType::EncryptedBhd:
            options->mOutputPath += "_decrypted.bhd";
            break;
        case CBinderToolLib::FileType::Dcx:
        case CBinderToolLib::FileType::Fmg:
            break;
        }
    }

    return options;
}
}


