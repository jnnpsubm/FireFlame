#pragma once
#include "FireFlameHeader.h"

class WOWChar
{
public:
    struct Mesh
    {
        std::uint32_t ID = 0;
        std::string description;
        std::int16_t materialOverride = 0;

        std::uint8_t  hasCustomTexture = 0;
        std::string   customTexture;
        std::uint16_t textureStyle = 0;
        std::uint8_t  hasGloss = 0;
        std::string   glossTexture;

        std::uint16_t level = 0;

        std::uint32_t vertexCount = 0;
        std::vector<FireFlame::FLVertexNormalTexBone> vertices;
        std::vector<std::uint16_t> indices;
    };
    struct Bone
    {
        std::uint16_t index = 0;
        std::int16_t  parent = -1;
        FireFlame::Vector3f pos;

        std::uint8_t hasData = 0;
        std::uint32_t flags = 0;
        std::uint16_t submeshId = 0;
        std::uint16_t unknown1 = 0;
        std::uint16_t unknown2 = 0;
    };
    struct Attachment
    {
        std::uint32_t ID = 0;
        std::int16_t parent = -1;
        FireFlame::Vector3f pos;
        float scale = 0.f;
    };

public:
    WOWChar(const std::string& name) :mName(name) {}
    void AppendMesh(const Mesh& mesh) {
        mMeshes.push_back(mesh);
    }
    void AppendBone(const Bone& bone) {
        mBones.push_back(bone);
    }
    void AppendAttachment(const Attachment& attachment) {
        mAttachments.push_back(attachment);
    }

private:
    std::string mName;

    std::vector<Mesh>       mMeshes;
    std::vector<Bone>       mBones;
    std::vector<Attachment> mAttachments;
};

