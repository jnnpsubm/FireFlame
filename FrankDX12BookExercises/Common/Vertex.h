#pragma once
#include <stdint.h>

struct Vertex {
    float x, y, z;
    Vertex() { x = y = z = 1.0f; }
    Vertex(float _x, float _y, float _z) : x(_x), y(_y), z(_z) {}
    Vertex& operator+=(const Vertex& offset) {
        x += offset.x;
        y += offset.y;
        z += offset.z;
        return *this;
    }
};

struct VertexColored : Vertex {
    float r, g, b, a;
    VertexColored() { r = g = b = a = 1.0f; }
    VertexColored(float _x, float _y, float _z, float _r, float _g, float _b, float _a)
        : Vertex(_x, _y, _z), r(_r), g(_g), b(_b), a(_a) 
    {}
};

struct VertexPackedColored : Vertex {
    uint32_t color;
    VertexPackedColored() { color = 0; }
    VertexPackedColored(float _x, float _y, float _z, uint32_t color)
        : Vertex(_x, _y, _z), color(color)
    {}
};