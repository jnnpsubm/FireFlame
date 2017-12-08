#pragma once
#include "FLVector.h"

namespace FireFlame 
{
struct FLVertex
{
    Vector3f Pos;
    FLVertex() = default;
    FLVertex(float x, float y, float z) :
        Pos(x, y, z)
    {}
};
struct FLVertexSize
{
    Vector3f Pos;
    Vector2f Size;
    FLVertexSize() = default;
    FLVertexSize(float x, float y, float z) :
        Pos(x, y, z)
    {}
    FLVertexSize(float x, float y, float z, float w, float h) :
        Pos(x, y, z), Size(w, h)
    {}
};
struct FLVertexColor
{
    Vector3f Pos;
    Vector4f Color;
    FLVertexColor() = default;
    FLVertexColor(float x, float y, float z) :
        Pos(x, y, z)
    {}
    FLVertexColor(float x, float y, float z, float r, float g, float b, float a) :
        Pos(x, y, z),
        Color(r, g, b, a) 
    {}
};
struct FLVertexColorV3
{
    Vector3f Pos;
    Vector3f Color;
    FLVertexColorV3() = default;
    FLVertexColorV3(float x, float y, float z) :
        Pos(x, y, z)
    {}
    FLVertexColorV3(float x, float y, float z, float r, float g, float b) :
        Pos(x, y, z),
        Color(r, g, b)
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
