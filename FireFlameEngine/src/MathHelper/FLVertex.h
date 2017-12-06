#pragma once
#include "FLVector.h"

namespace FireFlame 
{
struct FLVertexPos
{
    Vector3f Pos;
    FLVertexPos() = default;
    FLVertexPos(float x, float y, float z) :
        Pos(x, y, z)
    {}
};
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
struct FLVertexTex
{
    Vector3f Pos;
    Vector2f Tex;
    FLVertexTex() = default;
    FLVertexTex(float x, float y, float z, float s, float t) :
        Pos(x, y, z),
        Tex(s, t)
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
struct FLVertexNormalTex
{
    Vector3f Pos;
    Vector3f Normal;
    Vector2f Tex;
    FLVertexNormalTex() = default;
    FLVertexNormalTex(float x, float y, float z, float u, float v, float w, float s, float t) :
        Pos(x, y, z),
        Normal(u, v, w),
        Tex(s, t)
    {}
};
}
