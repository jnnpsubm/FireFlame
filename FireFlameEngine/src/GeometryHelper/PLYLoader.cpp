#include "FLPLYLoader.h"
#include <iostream>
#include <fstream>
#include "..\3rd_utils\tinyply.h"

namespace FireFlame
{
//const std::string PLYLoader::PLY_HEADER("ply");
//
//const std::string PLYLoader::PLY_FORMAT_ASCII("ascii");
//const std::string PLYLoader::PLY_FORMAT_BIN_LENDIAN("binary_little_endian");
//const std::string PLYLoader::PLY_FORMAT_BIN_BENDIAN("binary_big_endian");
//const std::string PLYLoader::PLY_FORMAT_VERSION_1_0("1.0");
//
//const std::string PLYLoader::PLY_HEADER_END("end_header");

bool PLYLoader::Load
(
    const std::string& filename,
    std::vector<FLVertexNormalTex>& verticesOut,
    std::vector<std::uint32_t>&  indicesOut
)
{
    using namespace tinyply;
    try
    {
        // Read the file and create a std::istringstream suitable
        // for the lib -- tinyply does not perform any file i/o.
        std::ifstream ss(filename, std::ios::binary);

        if (ss.fail())
        {
            throw std::runtime_error("failed to open " + filename);
        }

        PlyFile file;
        file.parse_header(ss);

        std::shared_ptr<PlyData> vertices, vnormals, fnormals, face_indices_num, faces, texcoords;

        // The header information can be used to programmatically extract properties on elements
        // known to exist in the file header prior to reading the data. For brevity of this sample, properties 
        // like vertex position are hard-coded: 
        try { vertices = file.request_properties_from_element("vertex", { "x", "y", "z" }); }
        catch (const std::exception & e) { std::cerr << "tinyply exception: " << e.what() << std::endl; }

        try { vnormals = file.request_properties_from_element("vertex", { "nx", "ny", "nz" }); }
        catch (const std::exception & e) { std::cerr << "tinyply exception: " << e.what() << std::endl; }

        try { texcoords = file.request_properties_from_element("vertex", { "u", "v" }); }
        catch (const std::exception & e) { std::cerr << "tinyply exception: " << e.what() << std::endl; }

        //try { colors = file.request_properties_from_element("vertex", { "red", "green", "blue", "alpha" }); }
        //catch (const std::exception & e) { std::cerr << "tinyply exception: " << e.what() << std::endl; }

        try { faces = file.request_properties_from_element("face", { "vertex_indices" }); }
        catch (const std::exception & e) { std::cerr << "tinyply exception: " << e.what() << std::endl; }

        try { face_indices_num = file.request_properties_from_element("face", { "list" }); }
        catch (const std::exception & e) { std::cerr << "tinyply exception: " << e.what() << std::endl; }

        try { fnormals = file.request_properties_from_element("face", { "nx", "ny", "nz" }); }
        catch (const std::exception & e) { std::cerr << "tinyply exception: " << e.what() << std::endl; }

        file.read(ss);

        if (vertices)
        {
            verticesOut.resize(vertices->count);
            if (vertices->t != Type::FLOAT32)
            {
                throw std::exception("only support float32 now");
            }
            std::uint8_t* buffer = vertices->buffer.get();
            size_t sizeEle = vertices->buffer.size_bytes() / vertices->count;
            for (size_t i = 0; i < verticesOut.size(); ++i)
            {
                memcpy(&verticesOut[i].Pos, buffer, sizeEle);
                buffer += sizeEle;
            }
            if (vnormals)
            {
                if (vnormals->t != Type::FLOAT32)
                {
                    throw std::exception("vnormals only support float32 now");
                }
                buffer = vnormals->buffer.get();
                sizeEle = vnormals->buffer.size_bytes() / vnormals->count;
                for (size_t i = 0; i < verticesOut.size(); ++i)
                {
                    memcpy(&verticesOut[i].Normal, buffer, sizeEle);
                    buffer += sizeEle;
                }
            }
            if (texcoords)
            {
                if (texcoords->t != Type::FLOAT32)
                {
                    throw std::exception("texcoords only support float32 now");
                }
                buffer = texcoords->buffer.get();
                sizeEle = texcoords->buffer.size_bytes() / texcoords->count;
                for (size_t i = 0; i < verticesOut.size(); ++i)
                {
                    memcpy(&verticesOut[i].Tex, buffer, sizeEle);
                    buffer += sizeEle;
                }
            }
        }
        if (faces)
        {
            std::uint8_t* buffer = faces->buffer.get();
            indicesOut.resize(faces->count * 3);
            size_t stride = PropertyTable[faces->t].stride;
            size_t indicesPerFace = faces->buffer.size_bytes() / stride / faces->count;
            std::cout << "index per face:" << indicesPerFace << std::endl;
            if (faces->t == Type::INT32)
            {
                for (size_t i = 0; i < indicesOut.size(); ++i)
                {
                    indicesOut[i] = *((std::int32_t*)buffer);
                    buffer += sizeof(std::int32_t);
                }
            }
            else if (faces->t == Type::UINT32)
            {
                for (size_t i = 0; i < indicesOut.size(); ++i)
                {
                    indicesOut[i] = *((std::uint32_t*)buffer);
                    buffer += sizeof(std::uint32_t);
                }
            }
            else if (faces->t == Type::INT16)
            {
                for (size_t i = 0; i < indicesOut.size(); ++i)
                {
                    indicesOut[i] = *((std::int16_t*)buffer);
                    buffer += sizeof(std::int16_t);
                }
            }
            else if (faces->t == Type::UINT16)
            {
                for (size_t i = 0; i < indicesOut.size(); ++i)
                {
                    indicesOut[i] = *((std::uint16_t*)buffer);
                    buffer += sizeof(std::uint16_t);
                }
            }
            else
            {
                throw std::exception("unknown index type......");
            }
        }
        if (!vnormals && fnormals)
        {
            std::cout << "no vertex normals,use face normals to calculate vertex normals..." << std::endl;
            if (fnormals->t != Type::FLOAT32)
            {
                throw std::exception("only support float32 now");
            }
            float* buffer = (float*)fnormals->buffer.get();
            for (size_t i = 0; i < faces->count; ++i)
            {
                Vector3f fn(*(buffer), *(buffer + 1), *(buffer + 2));
                verticesOut[indicesOut[3 * i + 0]].Normal += fn;
                verticesOut[indicesOut[3 * i + 1]].Normal += fn;
                verticesOut[indicesOut[3 * i + 2]].Normal += fn;
                buffer += 3;
            }
            for (size_t i = 0; i < verticesOut.size(); ++i)
            {
                verticesOut[i].Normal.Normalize();
            }
            std::cout << "vertex normals generated..." << std::endl;
        }
        if (!vnormals && !fnormals && faces)
        {
            for (size_t i = 0; i < faces->count; ++i)
            {

            }
        }
    }
    catch (const std::exception & e)
    {
        std::cerr << "Caught tinyply exception: " << e.what() << std::endl;
        return false;
    }
    return true;
}

bool PLYLoader::Load
(
    const std::string& filename, 
    std::vector<FLVertexNormal>& verticesOut,
    std::vector<std::uint32_t>&  indicesOut
)
{
    using namespace tinyply;
    try
    {
        // Read the file and create a std::istringstream suitable
        // for the lib -- tinyply does not perform any file i/o.
        std::ifstream ss(filename, std::ios::binary);

        if (ss.fail())
        {
            throw std::runtime_error("failed to open " + filename);
        }

        PlyFile file;
        file.parse_header(ss);

        std::shared_ptr<PlyData> vertices, vnormals, fnormals, face_indices_num, faces, texcoords;

        // The header information can be used to programmatically extract properties on elements
        // known to exist in the file header prior to reading the data. For brevity of this sample, properties 
        // like vertex position are hard-coded: 
        try { vertices = file.request_properties_from_element("vertex", { "x", "y", "z" }); }
        catch (const std::exception & e) { std::cerr << "tinyply exception: " << e.what() << std::endl; }

        try { vnormals = file.request_properties_from_element("vertex", { "nx", "ny", "nz" }); }
        catch (const std::exception & e) { std::cerr << "tinyply exception: " << e.what() << std::endl; }

        try { texcoords = file.request_properties_from_element("vertex", { "u", "v" }); }
        catch (const std::exception & e) { std::cerr << "tinyply exception: " << e.what() << std::endl; }

        //try { colors = file.request_properties_from_element("vertex", { "red", "green", "blue", "alpha" }); }
        //catch (const std::exception & e) { std::cerr << "tinyply exception: " << e.what() << std::endl; }

        try { faces = file.request_properties_from_element("face", { "vertex_indices" }); }
        catch (const std::exception & e) { std::cerr << "tinyply exception: " << e.what() << std::endl; }

        try { face_indices_num = file.request_properties_from_element("face", { "list" }); }
        catch (const std::exception & e) { std::cerr << "tinyply exception: " << e.what() << std::endl; }

        try { fnormals = file.request_properties_from_element("face", { "nx", "ny", "nz" }); }
        catch (const std::exception & e) { std::cerr << "tinyply exception: " << e.what() << std::endl; }

        file.read(ss);
        
        if (vertices)
        {
            verticesOut.resize(vertices->count);
            if (vertices->t != Type::FLOAT32)
            {
                throw std::exception("only support float32 now");
            }
            std::uint8_t* buffer = vertices->buffer.get();
            size_t sizeEle = vertices->buffer.size_bytes() / vertices->count;
            for (size_t i = 0; i < verticesOut.size(); ++i)
            {
                memcpy(&verticesOut[i].Pos, buffer, sizeEle);
                buffer += sizeEle;
            }
            if (vnormals)
            {
                if (vnormals->t != Type::FLOAT32)
                {
                    throw std::exception("only support float32 now");
                }
                buffer = vnormals->buffer.get();
                sizeEle = vnormals->buffer.size_bytes() / vnormals->count;
                for (size_t i = 0; i < verticesOut.size(); ++i)
                {
                    memcpy(&verticesOut[i].Normal, buffer, sizeEle);
                    buffer += sizeEle;
                }
            }
        }
        if (faces)
        {
            std::uint8_t* buffer = faces->buffer.get();
            indicesOut.resize(faces->count * 3);
            size_t stride = PropertyTable[faces->t].stride;
            size_t indicesPerFace = faces->buffer.size_bytes() / stride / faces->count;
            std::cout << "index per face:" << indicesPerFace << std::endl;
            if (faces->t == Type::INT32)
            {
                for (size_t i = 0; i < indicesOut.size(); ++i)
                {
                    indicesOut[i] = *((std::int32_t*)buffer);
                    buffer += sizeof(std::int32_t);
                }
            }
            else if (faces->t == Type::UINT32)
            {
                for (size_t i = 0; i < indicesOut.size(); ++i)
                {
                    indicesOut[i] = *((std::uint32_t*)buffer);
                    buffer += sizeof(std::uint32_t);
                }
            }
            else if (faces->t == Type::INT16)
            {
                for (size_t i = 0; i < indicesOut.size(); ++i)
                {
                    indicesOut[i] = *((std::int16_t*)buffer);
                    buffer += sizeof(std::int16_t);
                }
            }
            else if (faces->t == Type::UINT16)
            {
                for (size_t i = 0; i < indicesOut.size(); ++i)
                {
                    indicesOut[i] = *((std::uint16_t*)buffer);
                    buffer += sizeof(std::uint16_t);
                }
            }
            else
            {
                throw std::exception("unknown index type......");
            }
        }
        if (!vnormals && fnormals)
        {
            std::cout << "no vertex normals,use face normals to calculate vertex normals..." << std::endl;
            if (fnormals->t != Type::FLOAT32)
            {
                throw std::exception("only support float32 now");
            }
            float* buffer = (float*)fnormals->buffer.get();
            for (size_t i = 0; i < faces->count; ++i)
            {
                Vector3f fn(*(buffer), *(buffer + 1), *(buffer + 2));
                verticesOut[indicesOut[3 * i + 0]].Normal += fn;
                verticesOut[indicesOut[3 * i + 1]].Normal += fn;
                verticesOut[indicesOut[3 * i + 2]].Normal += fn;
                buffer += 3;
            }
            for (size_t i = 0; i < verticesOut.size(); ++i)
            {
                verticesOut[i].Normal.Normalize();
            }
            std::cout << "vertex normals generated..." << std::endl;
        }
        if (!vnormals && !fnormals && faces)
        {
            for (size_t i = 0; i < faces->count; ++i)
            {

            }
        }
    }
    catch (const std::exception & e)
    {
        std::cerr << "Caught tinyply exception: " << e.what() << std::endl;
        return false;
    }
    return true;
}

void PLYLoader::Save
(
    const std::string& filename, 
    const std::vector<FLVertexNormalTex>& vertices, 
    const std::vector<std::uint32_t>& indices,
    std::function<void(float)> report
)
{
    using namespace tinyply;

    // Tinyply does not perform any file i/o internally
    std::filebuf fb;
    fb.open(filename, std::ios::out | std::ios::binary);
    std::ostream outputStream(&fb);

    PlyFile outFile;

    outFile.add_properties_to_element
    (
        "vertex", { "x", "y", "z", "nx", "ny", "nz", "u", "v" }, 
        Type::FLOAT32, vertices.size()*8, 
        (uint8_t*)(vertices.data()),
        Type::INVALID, 0
    );
    outFile.add_properties_to_element
    (
        "face", { "vertex_indices" }, 
        Type::UINT32, indices.size(), 
        (uint8_t*)(indices.data()),
        Type::UINT32, 3
    );

    outFile.get_comments().push_back("generated by tinyply");
    outFile.write(outputStream, true, report);
    fb.close();
}

void PLYLoader::Save
(
    const std::string& filename, 
    const std::vector<FLVertexTex>& vertices, 
    const std::vector<std::uint32_t>& indices, 
    std::function<void(float)> report
)
{
    using namespace tinyply;

    // Tinyply does not perform any file i/o internally
    std::filebuf fb;
    fb.open(filename, std::ios::out | std::ios::binary);
    std::ostream outputStream(&fb);

    PlyFile outFile;

    outFile.add_properties_to_element
    (
        "vertex", { "x", "y", "z", "u", "v" },
        Type::FLOAT32, vertices.size() * 5,
        (uint8_t*)(vertices.data()),
        Type::INVALID, 0
    );
    outFile.add_properties_to_element
    (
        "face", { "vertex_indices" },
        Type::UINT32, indices.size(),
        (uint8_t*)(indices.data()),
        Type::UINT32, 3
    );

    outFile.get_comments().push_back("generated by tinyply");
    outFile.write(outputStream, true, report);
    fb.close();
}

void PLYLoader::Save
(
    const std::string& filename,
    const std::vector<FLVertexTex>& vertices,
    const std::vector<std::uint16_t>& indices,
    std::function<void(float)> report
)
{
    using namespace tinyply;

    // Tinyply does not perform any file i/o internally
    std::filebuf fb;
    fb.open(filename, std::ios::out | std::ios::binary);
    std::ostream outputStream(&fb);

    PlyFile outFile;

    outFile.add_properties_to_element
    (
        "vertex", { "x", "y", "z", "u", "v" },
        Type::FLOAT32, vertices.size() * 5,
        (uint8_t*)(vertices.data()),
        Type::INVALID, 0
    );
    outFile.add_properties_to_element
    (
        "face", { "vertex_indices" },
        Type::UINT16, indices.size(),
        (uint8_t*)(indices.data()),
        Type::UINT16, 3
    );

    outFile.get_comments().push_back("generated by tinyply");
    outFile.write(outputStream, true, report);
    fb.close();
}

//bool PLYLoader::Load(const std::string& filename)
//{
//    using std::string;
//
//    std::ifstream file(path);
//    string strLine;
//    string nouse;
//
//    std::getline(file, strLine);
//    if (strLine != PLY_HEADER)
//    {
//        throw std::exception("PLYLoader::Load Not ply file......");
//    }
//
//    string strFormat;
//    string strVersion;
//    file >> nouse >> strFormat >> strVersion;
//    std::getline(file, nouse); // comment line
//
//    while (std::getline(file, strLine) && strLine != PLY_HEADER_END)
//    {
//
//    }
//
//    if (strFormat == PLY_FORMAT_VERSION_1_0)
//    {
//        if (strFormat == PLY_FORMAT_BIN_LENDIAN)
//        {
//            return LoadBinaryLE_V1_0(path);
//        }
//        else if (strFormat == PLY_FORMAT_ASCII)
//        {
//            throw std::exception("PLYLoader::Load todo : ascii format not implemented...");
//        }
//        else if (strFormat == PLY_FORMAT_BIN_BENDIAN)
//        {
//            throw std::exception("PLYLoader::Load todo : binary big endian format not implemented...");
//        }
//    }
//    else
//    {
//        throw std::exception("PLYLoader::Load unknown ply version...");
//    }
//
//    return true;
//}

//bool PLYLoader::LoadBinaryLE_V1_0(const std::string& path)
//{
//    
//    return true;
//}
}