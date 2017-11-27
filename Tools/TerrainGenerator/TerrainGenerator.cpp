#include "TerrainGenerator.h"

float eva(float, float, float)
{
    return 100.f;
}

class evator
{
public:
    static float eva(float, float, float)
    {
        return 100.f;
    }
    static float eva(float)
    {
        return 100.f;
    }
};

TerrainGenerator::TerrainGenerator
(
    std::uint32_t width,
    std::uint32_t height,
    std::float_t  noise_len,
    const std::string& output, 
    const std::string& geoType, 
    std::uint8_t subDivideLevel,
    bool abs
) : mWidth(width),
    mHeight(height),
    mNoiseLen(noise_len),
    mFileName(output), 
    mGeoType(geoType), 
    mSubDivideLevel(subDivideLevel)
{
    if (mNoiseLen == 0.f)
    {
        mNoiseLen = (std::float_t)(std::max)(mWidth, mHeight) / 128.f;
    }
    mFileName += "_w" + std::to_string(mWidth) + "_h" + std::to_string(mHeight) +
        +"_n" + std::to_string((int)mNoiseLen);
    if (abs)
        mFileName += "_abs";
    mFileName += ".ply";

    if (abs)
        mNoiseEvaFunc = &FireFlame::Noise::EvaluateAbs;
    else
        mNoiseEvaFunc = &FireFlame::Noise::Evaluate;
}

void TerrainGenerator::Go()
{
    GenerateHeightMap();

    switch (ToTerrainType(mGeoType))
    {
    case TerrainType::Grid:
        GenerateGrid();
        break;
    case TerrainType::GeoSphere:
        GenerateGeoSphere();
        break;
    default:
        throw std::runtime_error("unknown terrain type......");
    }

    Save2PlyFile();
}

void TerrainGenerator::Save2PlyFile()
{
    auto report = [](float percent) { printf("Save Ply Data:%.2lf%%\r", percent); };
    FireFlame::PLYLoader::Save(mFileName, mVertices, mIndices, report);
    std::cout << "Save To " << mFileName << " Completed!" << std::endl;
}

void TerrainGenerator::GenerateHeightMap()
{
    const float stepw = mNoiseLen / (mWidth-1);
    const float steph = mNoiseLen / (mHeight-1);
    mNoiseData = std::make_unique<float[]>(mWidth*mHeight);

    std::cout << "noise len:" << mNoiseLen << std::endl;

    for (size_t i = 0; i < mHeight; ++i)
    {
        for (size_t j = 0; j < mWidth; ++j)
        {
            mNoiseData[i*mWidth + j] = mNoiseEvaFunc(i*steph, j*stepw, 0.5f) / 4.f;
        }
        if (i % 200 == 0)
            printf("Noise Data:%.2lf%%\r", (float)i / mHeight * 100.f);
    }
    std::cout << "Noise Data Generation Complete, size : " 
        << mWidth*mHeight*sizeof(std::float_t)/1024/1024 << "MB" << std::endl;
}

void TerrainGenerator::GenerateGrid()
{
    using namespace FireFlame;

    auto report = [](float percent) { printf("Geometry Data:%.2lf%%\r", percent); };
    GeometryGenerator geoGen;
    GeometryGenerator::MeshData grid = geoGen.CreateGrid(1.f, 1.f, mWidth, mHeight, report);

    auto vertexSize = grid.Vertices.size() * sizeof(GeometryGenerator::Vertex) / 1024 / 1024;
    auto indexSize = grid.Indices32.size() * sizeof(std::uint32_t) / 1024 / 1024;
    std::cout << "Geometry Data Generation Complete, vertex count : "
        << grid.Vertices.size() << std::endl;
    std::cout << "Geometry Data Generation Complete, vertex size : "
        << vertexSize << "MB" << std::endl;
    std::cout << "Geometry Data Generation Complete, index size : "
        << indexSize << "MB" << std::endl;

    mVertices.resize(grid.Vertices.size());
    for (size_t i = 0; i < grid.Vertices.size(); ++i)
    {
        mVertices[i].Pos = grid.Vertices[i].Position;
        mVertices[i].Normal = { 0.f,0.f,0.f };
        mVertices[i].Tex = grid.Vertices[i].TexC;
        if (i % 500 == 0)
            printf("Geometry Data Copied:%.2lf%%\r", (float)i / grid.Vertices.size()*100.f);
    }
    grid.Vertices.clear();
    std::cout << "Geometry Data Copied,size:"
        << mVertices.size() * sizeof(FLVertexNormalTex) / 1024 / 1024 << "MB" << std::endl;
    for (size_t i = 0; i < mHeight; ++i)
    {
        for (size_t j = 0; j < mWidth; ++j)
        {
            mVertices[i*mWidth+j].Pos.y = SampleHeightMap(j, i);
        }
        if (i % 400 == 0)
            printf("Height Map Sampled:%.2lf%%\r", (float)i / mHeight*100.f);
    }
    
    std::cout << "Height Map Sampling Complete!" << std::endl;
    mIndices = std::move(grid.Indices32);
    std::cout << "Face Count:" << mIndices.size() / 3 << std::endl;

    // begin normals

    // end normals
}

void TerrainGenerator::GenerateGeoSphere()
{

}

float TerrainGenerator::SampleHeightMap(size_t x, size_t y)
{
    size_t index = y*mWidth + x;
    return mNoiseData[index];
}

float TerrainGenerator::SampleHeightMap(float x, float y)
{
    //std::cout << "row:" << y*(mHeight-1) << " column:" << x*(mWidth-1) << std::endl;
    float hf = y*(mHeight - 1), wf = x*(mWidth - 1);
    size_t h = (size_t)(y*(mHeight-1)+0.5f), w = (size_t)(x*(mWidth-1)+0.5f);
    if (std::abs(hf - (float)h) >= 0.001f)
        std::cerr << "not point to point......" << std::endl;
    size_t index = h*mWidth + w;
    return mNoiseData[index];
}