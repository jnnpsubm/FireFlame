#pragma once
#include "Matrix\FLMatrix4X4.h"
#include <vector>

namespace FireFlame {
enum class ControllerInputMode {
    Poll,
    Feed
};

enum class API_Feature {
	API_DX12_1 = 0,
    API_DX12_0,
	API_DX11_1_On12,
    API_DX11_0_ON12,
	API_OpenGL42,
	API_OpenGLES,
	API_Vulkan,
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
        throw std::exception("unkown IndexFormatByteLength");
	}
}
 
const unsigned long VERTEX_FORMAT_FLOAT1         = 1UL << 0;
const unsigned long VERTEX_FORMAT_FLOAT2         = 1UL << 1;
const unsigned long VERTEX_FORMAT_FLOAT3         = 1UL << 2;
const unsigned long VERTEX_FORMAT_FLOAT4         = 1UL << 3;
const unsigned long VERTEX_FORMAT_A8B8G8R8_UNORM = 1UL << 4;

struct stSemanticName {
    stSemanticName(const std::string& _name,
                   unsigned int _index = 0)
        : name(_name), index(_index)
    {}
    std::string  name;
    unsigned int index;
};
struct stVertexInputDes {
    stVertexInputDes(const std::vector<unsigned long>& _format,
                     const std::vector<stSemanticName>& _semanticNames)
        : format(_format), semanticNames(_semanticNames)
    {}
    std::vector<unsigned long>    format;
    std::vector<stSemanticName>   semanticNames;
    unsigned long toCombinedFormat() const {
        unsigned long combined = 0;
        for (auto f : format) {
            combined |= f;
        }
        return combined;
    }
};

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
                        const std::vector<unsigned long>& _vertexFormats,
                        const std::vector<stSemanticName>& _semanticNames,
                        const std::vector<unsigned int>& _constBufferSize)
        : name(_name), vertexFormats(_vertexFormats),semanticNames(_semanticNames),
          constBufferSize(_constBufferSize)
    {/*==================================================================================*/}
    std::string                name;

    std::vector<stShaderStage>  shaderStage;
    std::vector<unsigned long>  vertexFormats;
    std::vector<stSemanticName> semanticNames;   // order mush match vertexFormat
    std::vector<unsigned int>   constBufferSize;

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
typedef const std::vector<stMSAADesc>& CRef_MSAADesc_Vec;
} // end namespace