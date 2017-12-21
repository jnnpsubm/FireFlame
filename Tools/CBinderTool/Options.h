#pragma once
#include <memory>
#include <string>
#include "FileType.h"
#include "GameVersion.h"

namespace CBinderTool {
class Options
{
public:
    Options();
    ~Options();

    static std::unique_ptr<Options> Parse(int argc, char* argv[]);

private:
    std::string mInputPath;

    static std::pair<FileType, GameVersion> GetFileType(const std::string& fileName);
};
}


