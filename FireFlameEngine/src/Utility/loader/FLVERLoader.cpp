#include "FLVERLoader.h"
#include "..\..\3rd_utils\spdlog\spdlog.h"
#include "..\IO\io.h"
#include "..\..\FLStringUtils.h"
#include "..\..\MathHelper\FLMathHelper.h"
#include <fstream>

namespace FireFlame {

inline std::string get_endian(wchar_t c)
{
    switch (c)
    {
    case L'L':return "LittleEndian";
    case L'B':return "BigEndian";
    default: return "UnknownEndian";
    }
}

void FLVERLoader::load(const std::string& filename)
{
    spdlog::get("console")->info("Loading FLVER file : {0}", filename);

    std::ifstream file(filename, std::ios::in | std::ios::binary);

    std::string nameext;
    IO::read_string(file, nameext);
    spdlog::get("console")->info("File Ext Name : {0}", nameext);

    wchar_t endian = 0;
    IO::read_type(file, endian);
    if (endian == L'B')
    {
        spdlog::get("console")->error("BigEndian FLVER file parser not implemented......");
        return;
    }
    spdlog::get("console")->info("Endian : {0}", get_endian(endian));

    // Version
    short verMajor, verMinor;
    IO::read_type(file, verMajor);
    IO::read_type(file, verMinor);
    spdlog::get("console")->info("Major Version : {0}, Minor Version : {1}", verMajor, verMinor);
    
    // data offset
    long dataOffset = 0;
    IO::read_type(file, dataOffset);
    spdlog::get("console")->info("Data Offset : {0:d}", dataOffset);

    long dataSize = 0;
    IO::read_type(file, dataSize);
    spdlog::get("console")->info("Data Size : {0:d}", dataSize);

    long count = 0;
    IO::read_type(file, count);
    spdlog::get("console")->info("Count : {0:d}", count);

    long numMat = 0;
    IO::read_type(file, numMat);
    spdlog::get("console")->info("Material Count : {0:d}", numMat);

    long boneCount = 0;
    IO::read_type(file, boneCount);
    spdlog::get("console")->info("Bone Count : {0:d}", boneCount);

    long numParts = 0;
    IO::read_type(file, numParts);
    spdlog::get("console")->info("Part Count : {0:d}", numParts);

    long numMesh = 0;
    IO::read_type(file, numMesh);
    spdlog::get("console")->info("Mesh Count : {0:d}", numMesh);

    file.seekg(0x50, std::ios::beg);
    long boneMapCount = 0;
    IO::read_type(file, boneMapCount);
    spdlog::get("console")->info("Bone Map Count : {0:d}", boneMapCount);
    long skipCount1 = 0;
    IO::read_type(file, skipCount1);
    spdlog::get("console")->info("Skip Count 1 : {0:d}", skipCount1);
    long skipCount2 = 0;
    IO::read_type(file, skipCount2);
    spdlog::get("console")->info("Skip Count 2 : {0:d}", skipCount2);

    file.seekg(0x80, std::ios::beg);
    for (long i = 0; i < count; i++)
    {
        file.seekg(64, std::ios::cur);
    }
    for (long i = 0; i < numMat; i++)
    {
        file.seekg(32, std::ios::cur);
    }
    spdlog::get("console")->info("Bone Start @ 0x{0:x}", file.tellg());

    std::vector<std::wstring> boneNames;
    auto bonePos = file.tellg();  
    for (long i = 0; i < boneCount; i++)
    {
        file.seekg(0xc, std::ios::cur);
        auto bPos = file.tellg();

        long boneNameOff = 0;
        IO::read_type(file, boneNameOff);
        spdlog::get("console")->info("Bone Name Offset : 0x{0:x}", boneNameOff);

        file.seekg(boneNameOff, std::ios::beg);
        std::wstring boneName;
        IO::read_wstring(file, boneName);
        boneNames.push_back(boneName);
        spdlog::get("console")->info("Bone Name : {0}", StringUtils::wstring2string(boneName));
        //std::wcout << "Bone Name:" << boneNames.back() << std::endl;

        file.seekg(bPos,std::ios::beg);
        file.seekg(0x74, std::ios::cur);
    }
    spdlog::get("console")->info("Bone Name Size : {0:d}", boneNames.size());

    file.seekg(bonePos, std::ios::beg);
    for (long i = 0; i < boneCount; i++)
    {
        auto getPos = file.tellg();
        getPos += (std::streampos)128;

        float c11, c12, c13;
        IO::read_type(file, c11); IO::read_type(file, c12); IO::read_type(file, c13);

        long boneNameOff = 0;
        IO::read_type(file, boneNameOff);

        float c21, c22, c23;
        IO::read_type(file, c21); IO::read_type(file, c22); IO::read_type(file, c23);
        c21 = MathHelper::ToRadius(c21); c22 = MathHelper::ToRadius(c22); c23 = MathHelper::ToRadius(c23);

        short boneParent, boneChild;
        IO::read_type(file, boneParent); IO::read_type(file, boneChild);
        spdlog::get("console")->info("Bone Parent : {0:d}, Child : {1:d}", boneParent, boneChild);

        float c31, c32, c33;
        IO::read_type(file, c31); IO::read_type(file, c32); IO::read_type(file, c33);
        c31 = MathHelper::ToRadius(c31); c32 = MathHelper::ToRadius(c32); c33 = MathHelper::ToRadius(c33);

        short boneUnk, boneUnk2;
        IO::read_type(file, boneUnk); IO::read_type(file, boneUnk2);

        float c41, c42, c43;
        IO::read_type(file, c41); IO::read_type(file, c42); IO::read_type(file, c43);
        c41 = MathHelper::ToRadius(c41); c42 = MathHelper::ToRadius(c42); c43 = MathHelper::ToRadius(c43);
        float c51, c52, c53;
        IO::read_type(file, c51); IO::read_type(file, c52); IO::read_type(file, c53);
        c51 = MathHelper::ToRadius(c51); c52 = MathHelper::ToRadius(c52); c53 = MathHelper::ToRadius(c53);

        file.seekg(getPos, std::ios::beg);

        // Todo : Construct Bones
    }

    spdlog::get("console")->info("Bone End @ 0x{0:x}", file.tellg());
    std::vector<long> boneMapOffs;
    std::vector<long> lodCounts;
    std::vector<long> maxBoneIDs;
    for (long i = 0; i < numParts; i++)
    {
        auto getPos = file.tellg() + (std::streampos)48;

        long Unk;
        IO::read_type(file, Unk);

        long meshID;
        IO::read_type(file, meshID);
        spdlog::get("console")->info("Mesh ID : {0:d}", meshID);

        file.seekg(0x8, std::ios::cur);

        long maxBoneID;
        IO::read_type(file, maxBoneID);
        spdlog::get("console")->info("Max Bone ID : {0:d}", maxBoneID);

        file.seekg(0x8, std::ios::cur);

        long boneMapOff;
        IO::read_type(file, boneMapOff);

        long lodCount;
        IO::read_type(file, lodCount);
        spdlog::get("console")->info("LOD Count : {0:d}", lodCount);

        file.seekg(0x10, std::ios::cur);

        file.seekg(getPos, std::ios::beg);

        boneMapOffs.push_back(boneMapOff);
        lodCounts.push_back(lodCount);
        maxBoneIDs.push_back(maxBoneID);
    }

    spdlog::get("console")->info("Face Info Start @ 0x{0:x}", file.tellg());
}

} // end FireFlame