#include "FLHKXLoader.h"
#include <fstream>
#include "..\..\3rd_utils\spdlog\spdlog.h"
#include "..\io\io.h"

namespace FireFlame {

void HKXLoader::load(const std::string& filename)
{
    spdlog::get("console")->info("Loading HKX file : {0}", filename);

    std::ifstream file(filename, std::ios::in | std::ios::binary);

    long boneCount;
    file.seekg(0x2a0, std::ios::beg);
    file.read((char*)&boneCount, sizeof(long));
    spdlog::get("console")->info("Bone Count : {0:d}", boneCount);

    std::vector<short> boneParaents;
    file.seekg(0x7c, std::ios::cur);
    for (long i = 0; i < boneCount; ++i)
    {
        short parent;
        file.read((char*)&parent, sizeof(short));
        boneParaents.push_back(parent);
        spdlog::get("console")->info("Bone Parent : {0:d}", parent);
    }
    spdlog::get("console")->info("Bone Parent Count : {0:d}", boneParaents.size());
    spdlog::get("console")->info("Bone Parent End @ 0x{0:x}", file.tellg());

    // Padding
    for (long i = 0; i < boneCount; ++i)
    {
        file.seekg(16, std::ios::cur);
    }
    spdlog::get("console")->info("Padding End @ 0x{0:x}", file.tellg());

    // Skip something
    char checkNull;
    file.read(&checkNull, 1);
    while (checkNull == 0)
    {
        auto pos = file.tellg();
        char chk;
        file.read(&chk, 1);
        if (chk == 0)
            file.seekg(0x0, std::ios::cur);
        else
            file.seekg(pos, std::ios::beg);
        file.read(&checkNull, 1);
    }
    std::uint64_t pos = file.tellg();
    file.seekg(pos-1, std::ios::beg);
    spdlog::get("console")->info("Names Start @ 0x{0:x}", file.tellg());

    // Bone names
    std::vector<std::string> boneNames;
    std::vector<std::int64_t> boneNamePos;
    for (long i = 0; i < boneCount; ++i)
    {
        std::string name;
        IO::read_string(file, name);
        spdlog::get("console")->info("Bone Name : {0}", name);
        boneNames.emplace_back(std::move(name));
        if (i == boneCount-1) break;
        // Skip
        char c;
        do file.read(&c, 1);
        while (c == '\0');
        file.putback(c);
    }
    spdlog::get("console")->info("Bone Name Count : {0:d}", boneNames.size());

    // Bone Datas
    size_t stride = 0;
    if (boneNames.back().size() < 16)
        stride = 16 - boneNames.back().size();
    else if (boneNames.back().size() < 32)
        stride = 32 - boneNames.back().size();
    else if (boneNames.back().size() < 64)
        stride = 64 - boneNames.back().size();
    else if (boneNames.back().size() < 128)
        stride = 128 - boneNames.back().size();
    file.seekg(stride-1, std::ios::cur);
    spdlog::get("console")->info("Bones Data Start @ 0x{0:x}", file.tellg());
}

} // end FireFlame