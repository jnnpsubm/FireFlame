#pragma once
#include "Matrix\FLMatrix4X4.h"
#include <vector>

namespace FireFlame {
enum class API_Feature {
	API_DX12 = 0,
	API_DX11On12,
	API_OpenGL42,
	API_OpenGLES,
	API_VULKAN,
	API_Metal,

	API_MAX
};

enum class Index_Format {
	UINT16 = 0,
	UINT32
};
inline unsigned int IndexFormatByteLength(Index_Format format) {
	switch (format){
	case Index_Format::UINT16:
		return 2;
	case Index_Format::UINT32:
		return 4;
	default:
		return 2;
	}
}

const unsigned long VERTEX_FORMAT_POS_FLOAT_3   = 1UL << 0;
const unsigned long VERTEX_FORMAT_COLOR_FLOAT_4 = 1UL << 1;

enum class Primitive_Topology {
    TrangleList,
};
struct stRawMesh {
    stRawMesh(const std::string& _name) 
        : name(_name), 
          primitiveTopology(Primitive_Topology::TrangleList)
    {/*===============================================================*/}
	std::string name;
	unsigned int vertexSize;
	unsigned int vertexCount;
	unsigned int vertexFormat;
	void* vertices;

	Index_Format indexFormat;
	unsigned int indexCount;
	void* indices;
	Matrix4X4 LocalToWorld;

    Primitive_Topology primitiveTopology;

	struct stSubMesh {
        stSubMesh(const std::string& _name):name(_name){}
		std::string  name;
		unsigned int indexCount;
		unsigned int startIndexLocation;			
		int          baseVertexLocation;
	};
};

enum class Shader_Type {
    VS = 0,
    TCS,
    TES,
    GS,
    PS,
};
struct stShaderStage {
    stShaderStage(const std::wstring& _file, Shader_Type _type, 
                  const std::string& _entry, const std::string& _target)
        : file(_file),type(_type),entry(_entry),target(_target)
    {/*=======================================================================================*/}
    std::wstring file;
    Shader_Type  type;
    std::string  entry;
    std::string  target;
};

struct stShaderDescription {
    stShaderDescription(const std::string& _name,
                        const std::vector<unsigned int>& _vertexFormats,
                        const std::vector<std::string>& _semanticNames,
                        const std::vector<unsigned int>& _constBufferSize)
        : name(_name), vertexFormats(_vertexFormats),semanticNames(_semanticNames),
          constBufferSize(_constBufferSize)
    {/*==================================================================================*/}
    std::string                name;

    std::vector<stShaderStage> shaderStage;
    std::vector<unsigned int>  vertexFormats;
    std::vector<std::string>   semanticNames;   // order mush match vertexFormat
    std::vector<unsigned int>  constBufferSize;

    void AddShaderStage(const std::wstring& file, Shader_Type type, 
                        const std::string& entry, const std::string& target) {
        shaderStage.emplace_back(file, type, entry, target);
    }
};

struct stMSAADesc {
    stMSAADesc(UINT s, UINT q) :sampleCount(s), qualityLevels(q) {}
    UINT sampleCount;
    UINT qualityLevels;
};
} // end namespace