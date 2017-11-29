#pragma once
#include "FireFlameHeader.h"

class TerrainGenerator
{
public:
    TerrainGenerator
    (
        std::uint32_t width,
        std::uint32_t height,
        std::float_t  noise_len,
        const std::string& output, 
        const std::string& geoType, 
        std::uint8_t subDivideLevel,
        std::float_t noise_scale,
        bool abs
    );
    void Go();

private:
    std::function<float(float, float, float)> mNoiseEvaFunc = [](float, float, float) {return 0.f; };

    void GenerateHeightMap();
    void GenerateGrid();
    void GenerateGeoSphere();

    void Save2PlyFile();

    float SampleHeightMap(float x, float y);
    float SampleHeightMap(size_t x, size_t y);

    enum class TerrainType:std::uint8_t
    {
        Grid = 0,
        GeoSphere,

        Unknown
    };
    TerrainType ToTerrainType(const std::string& type)
    {
        if (type == "grid") return TerrainType::Grid;
        else if (type == "geosphere") return TerrainType::GeoSphere;
        else return TerrainType::Unknown;
    }

    std::vector<std::uint32_t> mIndices;
    std::vector<FireFlame::FLVertexNormalTex> mVertices;

    std::float_t  mNoiseScale = 4.f;
    std::float_t  mNoiseLen = 2.f;
    std::unique_ptr<std::float_t[]> mNoiseData;

    std::uint32_t mWidth;
    std::uint32_t mHeight;

    std::string  mFileName;
    std::string  mGeoType;
    std::uint8_t mSubDivideLevel = 8;
};