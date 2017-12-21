#include <iostream>
#include <fstream>
#include <string>
#include "FireFlameHeader.h"
#include "WOWChar.h"

unsigned MakeFourCC(const char* fourCC)
{
    return fourCC[3] << 24 | fourCC[2] << 16 | fourCC[1] << 8 | fourCC[0];
}

std::uint32_t MakeVersion(std::uint16_t Major, std::uint16_t Minor)
{
    return Major << 16 | Minor;
}

void main(int argc, char* argv[])
{
    using namespace FireFlame;

    cmdline::parser cmdParser;
    cmdParser.add<std::string>("input", 'i', "input M2i file name", true);
    cmdParser.parse_check(argc, argv);

    std::string filename = cmdParser.get<std::string>("input");
    std::cout << "M2i:" << filename << std::endl;

    std::ifstream in(filename, std::ios_base::in | std::ios_base::binary);
    if (!in)
    {
        std::cerr << "can not open file " << filename << std::endl;
        return;
    }

    WOWChar wowCharT1("test1");

    // signature
    unsigned signature;
    IO::read_type(in, signature);
    if (signature != MakeFourCC("M2I0"))
    {
        std::cerr << "not a M2i file, exit......\n";
        return;
    }

    // version
    std::uint16_t versionMajor, versionMinor;
    IO::read_type(in, versionMajor);
    IO::read_type(in, versionMinor);
    std::uint32_t version = MakeVersion(versionMajor, versionMinor);
    std::cout << "Version Major:" << versionMajor << " Version Minor:" << versionMinor << std::endl;
    if (versionMajor != 4 || versionMinor < 5 || versionMinor > 9)
    {
        std::cerr << "Unsupported M2i version (" << versionMajor << "." << versionMinor << "), exit...\n";
    }

    // meshes
    std::uint32_t meshCount;
    IO::read_type(in, meshCount);
    std::cout << "mesh count:" << meshCount << std::endl;
    for (std::uint32_t i = 0; i < meshCount; i++)
    {
        WOWChar::Mesh mesh;
        std::uint16_t ID;
        IO::read_type(in, ID);
        mesh.ID = ID;
        if (version >= MakeVersion(4,6))
        {
            IO::read_string(in, mesh.description);
        }
        if (version >= MakeVersion(4, 7))
        {
            IO::read_type(in, mesh.materialOverride);
            if (version >= MakeVersion(4, 9))
            {
                IO::read_type(in, mesh.hasCustomTexture);
                IO::read_string(in, mesh.customTexture);
                IO::read_type(in, mesh.textureStyle);
                IO::read_type(in, mesh.hasGloss);
                IO::read_string(in, mesh.glossTexture);
            }
        }
        if (!mesh.customTexture.empty())
        {
            std::cout << "custom texture:" << mesh.customTexture << std::endl;
        }
        if (!mesh.glossTexture.empty())
        {
            std::cout << "gloss texture:" << mesh.glossTexture << std::endl;
        }
        IO::read_type(in, mesh.level);
        IO::read_type(in, mesh.vertexCount);
        std::cout << "mesh " << ID << mesh.description << " vertex count:" << mesh.vertexCount << std::endl;
        for (std::uint32_t i = 0; i < mesh.vertexCount; i++)
        {
            mesh.vertices.emplace_back();
            IO::read_type(in, mesh.vertices.back().Pos);
            IO::read_type(in, mesh.vertices.back().boneWeights);
            IO::read_type(in, mesh.vertices.back().boneIndices);
            IO::read_type(in, mesh.vertices.back().Normal);
            IO::read_type(in, mesh.vertices.back().Tex);
        }

        std::uint32_t triangleCount;
        IO::read_type(in, triangleCount);
        mesh.indices.resize(triangleCount * 3);
        for (std::uint32_t i = 0; i < triangleCount*3; i++)
        {
            IO::read_type(in, mesh.indices[i]);
            if (mesh.indices[i] >= mesh.vertices.size())
            {
                std::cerr << "error index:" << mesh.indices[i] << std::endl;
            }else if (mesh.indices[i] == mesh.vertices.size()-1)
            {
                //std::cout << "index:" << mesh.indices[i] << std::endl;
            }
        }
        wowCharT1.AppendMesh(mesh);
    }

    // bone list
    std::uint32_t boneCount;
    IO::read_type(in, boneCount);
    std::cout << "bone count:" << boneCount << std::endl;
    for (std::uint32_t i = 0; i < boneCount; i++)
    {
        WOWChar::Bone bone;
        IO::read_type(in, bone.index);
        IO::read_type(in, bone.parent);
        std::cout << "index:" << bone.index << " parent:" << bone.parent << std::endl;
        IO::read_type(in, bone.pos);
        if (version >= MakeVersion(4, 8))
        {
            IO::read_type(in, bone.hasData);
            IO::read_type(in, bone.flags);
            IO::read_type(in, bone.submeshId);
            IO::read_type(in, bone.unknown1);
            IO::read_type(in, bone.unknown2);
        }
        wowCharT1.AppendBone(bone);
    }

    // load attachments
    std::uint32_t attachmentCount;
    IO::read_type(in, attachmentCount);
    std::cout << "attachment count:" << attachmentCount << std::endl;
    for (std::uint32_t i = 0; i < attachmentCount; i++)
    {
        WOWChar::Attachment attachment;
        IO::read_type(in, attachment.ID);
        IO::read_type(in, attachment.parent);
        std::cout << "attachment ID:" << attachment.ID << " parent:" << attachment.parent << std::endl;
        IO::read_type(in, attachment.pos);
        IO::read_type(in, attachment.scale);
        wowCharT1.AppendAttachment(attachment);
    }

    system("pause");
}