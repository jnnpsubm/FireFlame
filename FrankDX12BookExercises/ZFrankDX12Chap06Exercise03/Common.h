#pragma once

struct Vertex {
    float x, y, z;
    Vertex() { x = y = z = 1.0f; }
    Vertex(float _x, float _y, float _z) : x(_x), y(_y), z(_z) {}
};

struct VertexColored : Vertex {
    float r, g, b, a;
    VertexColored() { r = g = b = a = 1.0f; }
    VertexColored(float _x, float _y, float _z, float _r, float _g, float _b, float _a)
        : Vertex(_x, _y, _z), r(_r), g(_g), b(_b), a(_a) 
    {}
};