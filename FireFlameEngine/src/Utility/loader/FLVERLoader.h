#pragma once
#include <string>
#include <vector>

namespace FireFlame {
class FLVERLoader
{
public:
    struct Vertex
    {
        Vertex(float x, float y, float z) :x(x), y(y), z(z) {}
        float x;
        float y;
        float z;
    };
    struct UV
    {
        UV(float u, float v) :u(u), v(v) {}
        float u;
        float v;
    };
    struct FaceInfo
    {
        FaceInfo(long faceCount, long faceOffset, long faceSize)
            :faceCount(faceCount), faceOffset(faceOffset), faceSize(faceSize)
        {}
        long faceCount;
        long faceOffset;
        long faceSize;
    };
    struct VertexInfo
    {
        VertexInfo(long vertexType, long vertexSize, long vertexCount, long vertexOffset) :
            vertexType(vertexType), vertexSize(vertexSize),
            vertexCount(vertexCount), vertexOffset(vertexOffset)
        {}
        long vertexType;
        long vertexSize;
        long vertexCount;
        long vertexOffset;
    };

    struct UVInfo
    {
        UVInfo(long UVType, long UVSize, long UVCount, long UVOffset):
            UVType(UVType), UVSize(UVSize), UVCount(UVCount), UVOffset(UVOffset)
        {}
        long UVType;
        long UVSize;
        long UVCount;
        long UVOffset;
    };

public:
    void load(const std::string& filename);

    size_t get_part_count() const { return mFaces.size(); }

    std::vector<std::vector<std::uint16_t>>& get_indices() { return mFaces; }
    std::vector<std::vector<Vertex>>&        get_vertices(){ return mVertices; }
    std::vector<std::vector<UV>>&            get_uvs()     { return mUVs; }
            
private:
    std::vector<std::vector<std::uint16_t>> mFaces;
    std::vector<std::vector<Vertex>>        mVertices;
    std::vector<std::vector<UV>>            mUVs;

    std::vector<FaceInfo>   mFaceInfo;
    std::vector<VertexInfo> mVertexInfo;
    std::vector<UVInfo>     mUVInfo;
};
} // end FireFlame