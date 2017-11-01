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

struct stRawMesh {
	std::string name;
	unsigned int vertexSize;
	unsigned int vertexCount;
	unsigned int vertexFormat;
	void* vertices;

	Index_Format indexFormat;
	unsigned int indexCount;
	void* indices;
	Matrix4X4 LocalToWorld;

	struct stSubMesh {
		std::string  name;
		unsigned int indexCount;
		unsigned int startIndexLocation;			
		int          baseVertexLocation;
	};
};

struct stShaderDescription {
    stShaderDescription(const std::string& _name, const std::wstring& file, unsigned int _numConstBuffer,
        const std::vector<std::string>& _entry, const std::vector<std::string>& _target, 
        const std::vector<std::string>& _semanticNames, UINT shaderType)
        : name(_name), shaderFile(file), numConstBuffer(_numConstBuffer),
          entryPoint(_entry), target(_target), semanticNames(_semanticNames), shaderType(shaderType)
    {/*===============================================================================================*/}
    std::string               name;

    std::wstring              shaderFile;
    std::vector<std::string>  entryPoint;  // order by vs tcs tes gs ps
    std::vector<std::string>  target;      // order by vs tcs tes gs ps
    UINT                      shaderType;
    std::vector<std::string>  semanticNames;
    unsigned int              numConstBuffer = 0;

    bool HaveVS() const  { return shaderType & Shader_VS; }
    bool HaveTCS() const { return shaderType & Shader_TCS; }
    bool HaveTES() const { return shaderType & Shader_TES; }
    bool HaveGS() const  { return shaderType & Shader_GS; }
    bool HavePS() const  { return shaderType & Shader_PS; }
    static const UINT Shader_VS  = 1U << 0;
    static const UINT Shader_TCS = 1U << 1;
    static const UINT Shader_TES = 1U << 2;
    static const UINT Shader_GS  = 1U << 3;
    static const UINT Shader_PS  = 1U << 4;
};
} // end namespace