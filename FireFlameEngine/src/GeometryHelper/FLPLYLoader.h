#pragma once
#include <vector>
#include <string>
#include <functional>
#include "..\MathHelper\FLVertex.h"

namespace FireFlame
{
class PLYLoader
{
public:
    static bool Load
    (
        const std::string& filename, 
        std::vector<FLVertexNormal>& verticesOut,
        std::vector<std::uint32_t>&  indicesOut
    );
    static bool Load
    (
        const std::string& filename,
        std::vector<FLVertexNormalTex>& verticesOut,
        std::vector<std::uint32_t>&  indicesOut
    );

    static void Save
    (
        const std::string& filename,
        const std::vector<FLVertexNormalTex>& vertices,
        const std::vector<std::uint32_t>&  indices, 
        std::function<void(float)> report
    );

    static void Save
    (
        const std::string& filename,
        const std::vector<FLVertexTex>& vertices,
        const std::vector<std::uint32_t>&  indices,
        std::function<void(float)> report
    );

    static void Save
    (
        const std::string& filename,
        const std::vector<FLVertexTex>& vertices,
        const std::vector<std::uint16_t>&  indices,
        std::function<void(float)> report
    );

private:
    /*static bool LoadBinaryLE_V1_0(const std::string& filename);

    static const std::string PLY_HEADER;

    static const std::string PLY_FORMAT_ASCII;
    static const std::string PLY_FORMAT_BIN_LENDIAN;
    static const std::string PLY_FORMAT_BIN_BENDIAN;
    static const std::string PLY_FORMAT_VERSION_1_0;

    static const std::string PLY_HEADER_END;*/
};
}