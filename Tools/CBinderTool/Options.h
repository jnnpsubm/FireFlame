#pragma once
#include <memory>
#include <string>
#include "..\CBinderToolLib\FileType.h"
#include "..\CBinderToolLib\GameVersion.h"

namespace CBinderTool {
class Options
{
public:
    Options();
    ~Options();

    CBinderToolLib::GameVersion GetInputGameVersion() const { return mInputGameVersion; }
    CBinderToolLib::FileType    GetInputType()        const { return mInputType;        }
    std::string                 GetOutputPath()       const { return mOutputPath;       }

    static std::unique_ptr<Options> Parse(int argc, char* argv[]);

private:
    std::string mInputPath;
    std::string mOutputPath;

    CBinderToolLib::FileType mInputType = CBinderToolLib::FileType::Unknown;
    CBinderToolLib::GameVersion mInputGameVersion = CBinderToolLib::GameVersion::Common;
};
}


