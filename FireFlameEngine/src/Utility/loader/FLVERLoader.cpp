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
    spdlog::get("console")->info("Data Offset : 0x{0:x}", dataOffset);

    long dataSize = 0;
    IO::read_type(file, dataSize);
    spdlog::get("console")->info("Data Size : {0:d}", dataSize);

    long count2 = 0;
    IO::read_type(file, count2);
    spdlog::get("console")->info("Count : {0:d}", count2);

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
    for (long i = 0; i < count2; i++)
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
    for (long i = 0; i < numParts; i++)
    {
        if (lodCounts[i] > 0)
        {
            long lodID;
            IO::read_type(file, lodID);

            char Unk1, Unk2, Unk3, Unk4;
            IO::read_type(file, Unk1); IO::read_type(file, Unk2); 
            IO::read_type(file, Unk3); IO::read_type(file, Unk4);

            long faceCount, faceOffset, faceSize, faceFlag;
            IO::read_type(file, faceCount);
            IO::read_type(file, faceOffset);
            IO::read_type(file, faceSize);
            IO::skip<long>(file);
            IO::read_type(file, faceFlag);
            IO::skip<long>(file);

            spdlog::get("console")->info
            (
                "Face Count:{0:d},Face Offset:0x{1:x},Face Size:{2:d},Face Flag:{3:d}",
                faceCount,faceOffset,faceSize,faceFlag
            );
            mFaceInfo.emplace_back(faceCount,faceOffset,faceSize);
            // Skip other lod
            for (long i = 0; i < lodCounts[i]-1; i++)
            {
                file.seekg(32, std::ios::cur);
            }
        }
        else
        {
            spdlog::get("console")->critical("No Lod for part : {0:d}?", i);
        }
    }
    spdlog::get("console")->info("Face Info End @ 0x{0:x}", file.tellg());

    for (long i = 0; i < numParts; i++)
    {
        long buffCount, vertexType, vertexSize;
        IO::read_type(file, buffCount, vertexType, vertexSize);

        long vertexCount, Unk1, Unk2, vertexSecSize, vertexOffset;
        IO::read_type(file, vertexCount, Unk1, Unk2, vertexSecSize, vertexOffset);

        //spdlog::get("console")->info("Vertex Size : {0:d}", vertexSize);
        long uvType = 0, uvSize = 0, uvCount = 0, /*Unk1 = 0, Unk2 = 0,*/ uvSecSize = 0, uvOffset = 0;
        if (vertexSize == 20) // Hmmm...
        {
            IO::skip<long>(file);
            IO::read_type(file, uvType, uvSize, uvCount, Unk1, Unk2, uvSecSize, uvOffset);
        }
        spdlog::get("console")->info
        (
            "Vertex Type:{0:d},Vertex Size:{1},Vertex Count:{2},Vertex Offset:{3}", 
            vertexType,vertexSize,vertexCount,vertexOffset
        );
        mVertexInfo.emplace_back(vertexType, vertexSize, vertexCount, vertexOffset);
        mUVInfo.emplace_back(uvType, uvSize, uvCount, uvOffset);
    }
    for (const auto& uv : mUVInfo)
    {
        spdlog::get("console")->info
        (
            "UV Type:{0:d},UV Size:{1},UV Count:{2},UV Offset:{3}",
            uv.UVType, uv.UVSize, uv.UVCount, uv.UVOffset
        );
    }

    spdlog::get("console")->info("Vertex Info End @ 0x{0:x}", file.tellg());

    mFaces.resize(numParts);
    mVertices.resize(numParts);
    mUVs.resize(numParts);
    for (long i = 0; i < numParts; i++)
    {
        long faceOffset = dataOffset + mFaceInfo[i].faceOffset;
        file.seekg(faceOffset, std::ios::beg);
        spdlog::get("console")->info("Face Start @ 0x{0:x}", file.tellg());
        while (file.tellg() < (faceOffset+mFaceInfo[i].faceSize))
        {
            std::uint16_t f1, f2, f3;
            IO::read_type(file, f1, f2, f3);
            //mFaces[i].push_back(f1 + 1); mFaces[i].push_back(f2 + 1); mFaces[i].push_back(f3 + 1);
            mFaces[i].push_back(f1); mFaces[i].push_back(f2); mFaces[i].push_back(f3);
        }
        //spdlog::get("console")->info("0x{0:x}:0x{1:x}", file.tellg(), faceOffset + mFaceInfo[i].faceSize);

        long vertexOffset = dataOffset + mVertexInfo[i].vertexOffset;
        file.seekg(vertexOffset, std::ios::beg);
        spdlog::get("console")->info("Vertex Start @ 0x{0:x}", file.tellg());

        spdlog::get("console")->info
        (
            "Vertex Type:{0:d},Vertex Size:{1:d}",
            mVertexInfo[i].vertexType, mVertexInfo[i].vertexSize
        );
        if (mVertexInfo[i].vertexSize == 48)
        {
            for (long j = 0; j < mVertexInfo[i].vertexCount; j++)
            {
                float vx, vy, vz;
                IO::read_type(file, vx, vy, vz);
                file.seekg(0xc, std::ios::cur);

                unsigned char bone[6],weight[6];
                IO::read_type(file, bone[0], bone[1], bone[2], bone[3], bone[4], bone[5]);
                IO::read_type(file, weight[0], weight[1], weight[2], weight[3], weight[4], weight[5]);
                file.seekg(0x4, std::ios::cur);

                short tu, tv;
                IO::read_type(file, tu, tv);
                tv = -tv; // todo
                file.seekg(0x4, std::ios::cur);

                // todo : handle weight and bone
                {}

                mVertices[i].emplace_back(vx*100.f, vy*100.f, vz*100.f);
                mUVs[i].emplace_back((float)tu/2048.f, (float)tv/2048.f);
            }
        }
        else if (mVertexInfo[i].vertexType == 9 && mVertexInfo[i].vertexSize == 44)
        {
            for (long j = 0; j < mVertexInfo[i].vertexCount; j++)
            {
                auto pos = file.tellg();
                pos += (std::streampos)mVertexInfo[i].vertexSize;

                float vx, vy, vz;
                IO::read_type(file, vx, vy, vz);

                file.seekg(0x8, std::ios::cur);
                std::uint8_t bone1, bone2, bone3, bone4, weight1, weight2, weight3, weight4;
                IO::read_type(file, bone1, bone2, bone3, bone4, weight1, weight2, weight3, weight4);

                file.seekg(0x4, std::ios::cur);
                short tu, tv;
                IO::read_type(file, tu, tv);
                tv = -tv;

                file.seekg(0x8, std::ios::cur);
                // Toto : handle bone and weight
                {}

                file.seekg(pos, std::ios::beg);
                mVertices[i].emplace_back(vx*100.f, vy*100.f, vz*100.f);
                mUVs[i].emplace_back((float)tu / 2048.f, (float)tv / 2048.f);
            }
        }
        else if (mVertexInfo[i].vertexType != 9 && mVertexInfo[i].vertexSize == 44)
        {
            for (long j = 0; j < mVertexInfo[i].vertexCount; j++)
            {
                auto pos = file.tellg() + (std::streampos)mVertexInfo[i].vertexSize;

                float vx, vy, vz;
                IO::read_type(file, vx, vy, vz);

                file.seekg(0xc, std::ios::cur);
                std::uint8_t bone1, bone2, bone3, bone4, weight1, weight2, weight3, weight4;
                IO::read_type(file, bone1, bone2, bone3, bone4, weight1, weight2, weight3, weight4);

                file.seekg(0x4, std::ios::cur);
                short tu, tv;
                IO::read_type(file, tu, tv);
                tv = -tv;

                file.seekg(0x4, std::ios::cur);
                // Todo:bone and weight
                {}
                file.seekg(pos, std::ios::beg);
                mVertices[i].emplace_back(vx*100.f, vy*100.f, vz*100.f);
                mUVs[i].emplace_back((float)tu / 2048.f, (float)tv / 2048.f);
            }
        }
        else if (mVertexInfo[i].vertexSize == 40)
        {
            for (long j = 0; j < mVertexInfo[i].vertexCount; j++)
            {
                float vx, vy, vz;
                IO::read_type(file, vx, vy, vz);

                file.seekg(0x8, std::ios::cur);
                std::uint8_t bone1, bone2, bone3, bone4, weight1, weight2, weight3, weight4;
                IO::read_type(file, bone1, bone2, bone3, bone4, weight1, weight2, weight3, weight4);

                short tu, tv;
                IO::read_type(file, tu, tv);
                tv = -tv;

                file.seekg(0x8, std::ios::cur);
                // Todo:bone and weight
                {}
                
                mVertices[i].emplace_back(vx*100.f, vy*100.f, vz*100.f);
                mUVs[i].emplace_back((float)tu / 2048.f, (float)tv / 2048.f);
            }
        }
        else if (mVertexInfo[i].vertexSize == 36)
        {
            for (long j = 0; j < mVertexInfo[i].vertexCount; j++)
            {
                float vx, vy, vz;
                IO::read_type(file, vx, vy, vz);

                file.seekg(0x8, std::ios::cur);
                std::uint8_t bone1, bone2, bone3, bone4, weight1, weight2, weight3, weight4;
                IO::read_type(file, bone1, bone2, bone3, bone4, weight1, weight2, weight3, weight4);

                file.seekg(0x4, std::ios::cur);
                short tu, tv;
                IO::read_type(file, tu, tv);
                tv = -tv;

                // Todo:bone and weight
                {}
                mVertices[i].emplace_back(vx*100.f, vy*100.f, vz*100.f);
                mUVs[i].emplace_back((float)tu / 2048.f, (float)tv / 2048.f);
            }
        }
        else if (mVertexInfo[i].vertexSize == 32)
        {
            for (long j = 0; j < mVertexInfo[i].vertexCount; j++)
            {
                float vx, vy, vz;
                IO::read_type(file, vx, vy, vz);

                std::uint8_t bone1, bone2, bone3, bone4, weight1, weight2, weight3, weight4;
                IO::read_type(file, bone1, bone2, bone3, bone4, weight1, weight2, weight3, weight4);

                file.seekg(0xc, std::ios::cur);
                short tu, tv;
                IO::read_type(file, tu, tv);
                tv = -tv;

                file.seekg(0x4, std::ios::cur);
                // Todo:bone and weight
                {}
                mVertices[i].emplace_back(vx*100.f, vy*100.f, vz*100.f);
                mUVs[i].emplace_back((float)tu / 2048.f, (float)tv / 2048.f);
            }
        }
        else if (mVertexInfo[i].vertexSize == 20)
        {
            for (long j = 0; j < mVertexInfo[i].vertexCount; j++)
            {
                float vx, vy, vz;
                IO::read_type(file, vx, vy, vz);
                mVertices[i].emplace_back(vx*100.f, vy*100.f, vz*100.f);

                file.seekg(0x8, std::ios::cur);
            }
            spdlog::get("console")->info("Vertex End @ 0x{0:x}", file.tellg());

            std::uint64_t uvOffset = dataOffset + mUVInfo[i].UVOffset;
            file.seekg(uvOffset, std::ios::beg);
            spdlog::get("console")->info("UV Start @ 0x{0:x}", file.tellg());
            if (mUVInfo[i].UVSize == 24)
            {
                for (long j = 0; j < mUVInfo[i].UVCount; j++)
                {
                    auto pos = file.tellg() + (std::streampos)mUVInfo[i].UVSize;

                    file.seekg(0x4, std::ios::cur);
                    std::uint8_t bone1, bone2, bone3, bone4, weight1, weight2, weight3, weight4;
                    IO::read_type(file, bone1, bone2, bone3, bone4, weight1, weight2, weight3, weight4);

                    short tu, tv;
                    IO::read_type(file, tu, tv);
                    tv = -tv;

                    file.seekg(0x4, std::ios::cur);
                    // Todo:bone and weight
                    {}

                    file.seekg(pos, std::ios::beg);
                    mUVs[i].emplace_back((float)tu / 2048.f, (float)tv / 2048.f);
                }
            }
            else if (mUVInfo[i].UVType == 11 && mUVInfo[i].UVSize == 28)
            {
                for (long j = 0; j < mUVInfo[i].UVCount; j++)
                {
                    auto pos = file.tellg() + (std::streampos)mUVInfo[i].UVSize;

                    file.seekg(0x4, std::ios::cur);
                    std::uint8_t bone1, bone2, bone3, bone4, weight1, weight2, weight3, weight4;
                    IO::read_type(file, bone1, bone2, bone3, bone4, weight1, weight2, weight3, weight4);

                    file.seekg(0x4, std::ios::cur);
                    short tu, tv;
                    IO::read_type(file, tu, tv);
                    tv = -tv;

                    // Todo:bone and weight
                    {}

                    file.seekg(pos, std::ios::beg);
                    mUVs[i].emplace_back((float)tu / 2048.f, (float)tv / 2048.f);
                }
            }
            else if (mUVInfo[i].UVType != 11 && mUVInfo[i].UVSize == 28)
            {
                for (long j = 0; j < mUVInfo[i].UVCount; j++)
                {
                    auto pos = file.tellg() + (std::streampos)mUVInfo[i].UVSize;

                    file.seekg(0x8, std::ios::cur);
                    std::uint8_t bone1, bone2, bone3, bone4, weight1, weight2, weight3, weight4;
                    IO::read_type(file, bone1, bone2, bone3, bone4, weight1, weight2, weight3, weight4);

                    file.seekg(0x4, std::ios::cur);
                    short tu, tv;
                    IO::read_type(file, tu, tv);
                    tv = -tv;

                    // Todo:bone and weight
                    {}

                    file.seekg(pos, std::ios::beg);
                    mUVs[i].emplace_back((float)tu / 2048.f, (float)tv / 2048.f);
                }
            }
            else if (mUVInfo[i].UVSize == 32)
            {
                for (long j = 0; j < mUVInfo[i].UVCount; j++)
                {
                    auto pos = file.tellg() + (std::streampos)mUVInfo[i].UVSize;

                    file.seekg(0x8, std::ios::cur);
                    std::uint8_t bone1, bone2, bone3, bone4, weight1, weight2, weight3, weight4;
                    IO::read_type(file, bone1, bone2, bone3, bone4, weight1, weight2, weight3, weight4);

                    file.seekg(0x4, std::ios::cur);
                    short tu, tv;
                    IO::read_type(file, tu, tv);
                    tv = -tv;

                    // Todo:bone and weight
                    {}

                    file.seekg(pos, std::ios::beg);
                    mUVs[i].emplace_back((float)tu / 2048.f, (float)tv / 2048.f);
                }
            }
            else
            {
                spdlog::get("console")->critical("UV Format Unknown......");
            }
            spdlog::get("console")->info("UV End @ 0x{0:x}", file.tellg());
        }
        else
        {
            spdlog::get("console")->critical("Vertex Format Unknown......");
        }
    }

    spdlog::get("console")->info("FLVER file : {0} Loaded!", filename);
}

} // end FireFlame