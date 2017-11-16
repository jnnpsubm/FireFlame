#pragma once
#include "FLVector.h"

namespace FireFlame 
{
struct FLVertex
{
    Vector3f Pos;
    Vector4f Color;
    FLVertex() = default;
    FLVertex(float x, float y, float z) :
        Pos(x, y, z)
    {}
    FLVertex(float x, float y, float z, float r, float g, float b, float a) : 
        Pos(x, y, z),
        Color(r, g, b, a) 
    {}
};
struct FLVertexNormal
{
    Vector3f Pos;
    Vector3f Normal;
    FLVertexNormal() = default;
    FLVertexNormal(float x, float y, float z, float u, float v, float w) :
        Pos(x, y, z),
        Normal(u, v, w)
    {}
};
}
