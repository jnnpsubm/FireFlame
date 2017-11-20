#pragma once
#include <vector>
#include <string>
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