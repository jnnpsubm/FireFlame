#pragma once
#include "Matrix\FLMatrix4X4.h"
#include <array>
#include <vector>
#include <string>

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

enum class FilterType
{
    Blur = 0,
    Sobel,
    HDR,

    Max
};

struct FilterParam
{
    FilterParam(FilterType type) :type(type) {}
    FilterType type;
    union 
    {
        struct
        {
            int blurCount;
            float sigma;
        };
    };
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
        throw std::exception("unknown IndexFormatByteLength");
	}
}
 
const unsigned long VERTEX_FORMAT_FLOAT1         = 1UL << 0;
const unsigned long VERTEX_FORMAT_FLOAT2         = 1UL << 1;
const unsigned long VERTEX_FORMAT_FLOAT3         = 1UL << 2;
const unsigned long VERTEX_FORMAT_FLOAT4         = 1UL << 3;
const unsigned long VERTEX_FORMAT_A8B8G8R8_UNORM = 1UL << 4;
const unsigned long VERTEX_FORMAT_A8R8G8B8_UNORM = 1UL << 8;

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
    PointList,
    LineStrip,
    LineList,
    TriangleStrip,
    TriangleList
};
enum class Primitive_Topology_Type {
    Point,
    Line,
    Triangle,
    Patch
};
enum class Cull_Mode {
    None = 1,
    Front,
    Back
};
enum class Fill_Mode {
    Wireframe = 2,
    Solid = 3
};

inline Primitive_Topology_Type FLTopology2FLTopologyType(Primitive_Topology top)
{
    switch (top)
    {
    case FireFlame::Primitive_Topology::PointList:
        return Primitive_Topology_Type::Point;
    case FireFlame::Primitive_Topology::LineStrip:
        return Primitive_Topology_Type::Line;
    case FireFlame::Primitive_Topology::LineList:
        return Primitive_Topology_Type::Line;
    case FireFlame::Primitive_Topology::TriangleStrip:
        return Primitive_Topology_Type::Triangle;
    case FireFlame::Primitive_Topology::TriangleList:
        return Primitive_Topology_Type::Triangle;
    default:
        throw std::exception("unknown primitive topology in FLTopology2FLTopologyType");
    }
}

enum class COLOR_WRITE_ENABLE :std::uint8_t
{
    RED = 1,
    GREEN = 2,
    BLUE = 4,
    ALPHA = 8,
    ALL = (((RED | GREEN) | BLUE) | ALPHA)
};

enum class STENCIL_OP:std::uint8_t
{
    KEEP = 1,
    ZERO = 2,
    REPLACE = 3,
    INCR_SAT = 4,
    DECR_SAT = 5,
    INVERT = 6,
    INCR = 7,
    DECR = 8
};

enum class COMPARISON_FUNC
{
    NEVER = 1,
    LESS = 2,
    EQUAL = 3,
    LESS_EQUAL = 4,
    GREATER = 5,
    NOT_EQUAL = 6,
    GREATER_EQUAL = 7,
    ALWAYS = 8
};

enum class BLEND_OP:std::uint8_t
{
    ADD = 1,
    SUBTRACT = 2,
    REV_SUBTRACT = 3,
    MIN = 4,
    MAX = 5
};

enum class BLEND:std::uint8_t
{
    ZERO = 1,
    ONE = 2,
    SRC_COLOR = 3,
    INV_SRC_COLOR = 4,
    SRC_ALPHA = 5,
    INV_SRC_ALPHA = 6,
    DEST_ALPHA = 7,
    INV_DEST_ALPHA = 8,
    DEST_COLOR = 9,
    INV_DEST_COLOR = 10,
    SRC_ALPHA_SAT = 11,
    BLEND_FACTOR = 14,
    INV_BLEND_FACTOR = 15,
    SRC1_COLOR = 16,
    INV_SRC1_COLOR = 17,
    SRC1_ALPHA = 18,
    INV_SRC1_ALPHA = 19
};

struct PSODesc
{
    PSODesc
    (
        const std::string& shaderName
    ) : shaderName(shaderName)
    {
        default();
    }
    PSODesc
    (
        const std::string& shaderName,
        const std::string& shaderMacroVS,
        const std::string& shaderMacroPS,
        bool opaque = true,
        Primitive_Topology topology = Primitive_Topology::TriangleList,
        Cull_Mode cullMode = Cull_Mode::Back
    ) : shaderName(shaderName),
        shaderMacroVS(shaderMacroVS),
        shaderMacroPS(shaderMacroPS),
        opaque(opaque),
        topology(topology),
        cullMode(cullMode)
    {}
    void default() {
        shaderMacroVS = "";
        shaderMacroPS = "";

        topology = Primitive_Topology::TriangleList;
        cullMode = Cull_Mode::Back;
        frontCounterClockwise = false;

        opaque = true;
        alpha2Coverage = false;
        blendOp = BLEND_OP::ADD;
        blendOpAlpha = BLEND_OP::ADD;
        srcBlend = BLEND::SRC_ALPHA;
        destBlend = BLEND::INV_SRC_ALPHA;
        srcBlendAlpha = BLEND::ONE;
        destBlendAlpha = BLEND::ZERO;
        
        depthEnable = true;
        stencilEnable = false;
        stencilReadMask = 0xff;
        stencilWriteMask = 0xff;
        depthWriteMask = 1;

        stencilFailOp = STENCIL_OP::KEEP;
        stencilDepthFailOp = STENCIL_OP::KEEP;
        stencilPassOp = STENCIL_OP::KEEP;
        stencilFunc = COMPARISON_FUNC::ALWAYS;

        colorWriteEnable =
        {
            (std::uint8_t)COLOR_WRITE_ENABLE::ALL,
            (std::uint8_t)COLOR_WRITE_ENABLE::ALL,
            (std::uint8_t)COLOR_WRITE_ENABLE::ALL,
            (std::uint8_t)COLOR_WRITE_ENABLE::ALL,
            (std::uint8_t)COLOR_WRITE_ENABLE::ALL,
            (std::uint8_t)COLOR_WRITE_ENABLE::ALL,
            (std::uint8_t)COLOR_WRITE_ENABLE::ALL,
            (std::uint8_t)COLOR_WRITE_ENABLE::ALL
        };
    }
    std::string shaderName;
    std::string shaderMacroVS;
    std::string shaderMacroGS;
    std::string shaderMacroPS;
    
    Primitive_Topology topology = Primitive_Topology::TriangleList;
    Cull_Mode cullMode = Cull_Mode::Back;
    bool frontCounterClockwise = false;

    bool opaque = true;
    bool alpha2Coverage = false;
    BLEND_OP blendOp = BLEND_OP::ADD;
    BLEND_OP blendOpAlpha = BLEND_OP::ADD;
    BLEND srcBlend = BLEND::SRC_ALPHA;
    BLEND destBlend = BLEND::INV_SRC_ALPHA;
    BLEND srcBlendAlpha = BLEND::ONE;
    BLEND destBlendAlpha = BLEND::ZERO;

    bool depthEnable = true;
    bool stencilEnable = false;
    std::uint8_t stencilReadMask = 0xff;
    std::uint8_t stencilWriteMask = 0xff;

    STENCIL_OP      stencilFailOp = STENCIL_OP::KEEP;
    STENCIL_OP      stencilDepthFailOp = STENCIL_OP::KEEP;
    STENCIL_OP      stencilPassOp = STENCIL_OP::KEEP;
    COMPARISON_FUNC stencilFunc = COMPARISON_FUNC::ALWAYS;

    std::uint8_t depthWriteMask = 1;
    std::array<std::uint8_t,8> colorWriteEnable =
    //std::uint8_t colorWriteEnable[8] =
    {
        (std::uint8_t)COLOR_WRITE_ENABLE::ALL,
        (std::uint8_t)COLOR_WRITE_ENABLE::ALL,
        (std::uint8_t)COLOR_WRITE_ENABLE::ALL,
        (std::uint8_t)COLOR_WRITE_ENABLE::ALL,
        (std::uint8_t)COLOR_WRITE_ENABLE::ALL,
        (std::uint8_t)COLOR_WRITE_ENABLE::ALL,
        (std::uint8_t)COLOR_WRITE_ENABLE::ALL,
        (std::uint8_t)COLOR_WRITE_ENABLE::ALL
    };
};

struct stRawMesh {
    stRawMesh() = default;
    explicit stRawMesh(const std::string& _name) 
        : name(_name), 
          primitiveTopology(Primitive_Topology::TriangleList)
    {/*===============================================================*/}
	std::string name;
    bool                      vertexInFrameRes = false;
    std::vector<void*>        vertexData;
	std::vector<unsigned int> vertexDataSize;
    std::vector<unsigned int> vertexDataCount;

	Index_Format indexFormat;
	unsigned int indexCount;
	void* indices;

    Primitive_Topology primitiveTopology = Primitive_Topology::TriangleList;

	struct stSubMesh {
        stSubMesh() = default;
        explicit stSubMesh(const std::string& _name):name(_name){}
        stSubMesh(const std::string& _name,unsigned int indexCount,
                  unsigned int startIndexLocation = 0,
                  int baseVertexLocation = 0)
            : name(_name), indexCount(indexCount), startIndexLocation(startIndexLocation)
            , baseVertexLocation(baseVertexLocation)
        {}
		std::string  name;
		unsigned int indexCount = 0;
		unsigned int startIndexLocation = 0;			
		int          baseVertexLocation = 0;
	};
    std::vector<stSubMesh> subMeshs;
};

struct stRenderItemDesc {
    stRenderItemDesc() = default;
    explicit stRenderItemDesc(const std::string& name, Primitive_Topology topology = Primitive_Topology::TriangleList)
        :name(name),
         topology(topology)
    {}
    stRenderItemDesc
    (
        const std::string& _name, 
        const stRawMesh::stSubMesh& subMesh,
        Primitive_Topology topology = Primitive_Topology::TriangleList
    )
        : name(_name),
          subMesh(subMesh), topology(topology)
    {}
    std::string  name;

    stRawMesh::stSubMesh subMesh;
    Primitive_Topology topology = Primitive_Topology::TriangleList;

    bool opaque = true;
    Cull_Mode cullMode = Cull_Mode::Back;

    size_t dataLen = 0;
    void*  data = nullptr;

    std::string mat;

    std::uint32_t stencilRef = -1;

    std::string AsPSOName() const
    {
        return std::to_string(opaque) +
               std::to_string(int(FLTopology2FLTopologyType(topology))) +
               std::to_string(int(cullMode));
    }
};

enum class Shader_Type {
    VS = 0,
    TCS,
    TES,
    GS,
    PS,
    CS,
};
struct stShaderStage {
    stShaderStage(const std::wstring& _file, Shader_Type _type, 
                  const std::string& _entry, const std::string& _target)
        : file(_file),type(_type),entry(_entry),target(_target)
    {/*=======================================================================================*/}
    stShaderStage(const std::wstring& _file, Shader_Type _type,
                  const std::string& _entry, const std::string& _target, 
                  std::vector<std::pair<std::string, std::string>>& macros)
        : file(_file), type(_type), entry(_entry), target(_target), Macros(macros)
    {/*=======================================================================================*/
    }
    stShaderStage(const std::string& _data, Shader_Type _type,
            const std::string& _entry, const std::string& _target)
        : data(_data), type(_type), entry(_entry), target(_target)
    {/*=======================================================================================*/}
    std::wstring file;
    std::string  data;
    Shader_Type  type;
    std::string  entry;
    std::string  target;

    std::vector<std::pair<std::string, std::string>> Macros;
    void AddMacro(const std::string& name, const std::string& def) {
        Macros.push_back(std::make_pair(name, def));
    }
    std::string Macros2String() const {
        std::string ret;
        for (const auto& macro : Macros)
        {
            ret += macro.first;
            ret += '_';
            ret += macro.second;
            ret += '_';
        }
        return std::move(ret);
    }
};

enum class ROOT_PARAMETER_TYPE :std::uint8_t
{
    DESCRIPTOR_TABLE = 0,
    _32BIT_CONSTANTS = (DESCRIPTOR_TABLE + 1),
    CBV = (_32BIT_CONSTANTS + 1),
    SRV = (CBV + 1),
    UAV = (SRV + 1)
};

enum class DESCRIPTOR_RANGE_TYPE
{
    SRV = 0,
    UAV = (SRV + 1),
    CBV = (UAV + 1),
    SAMPLER = (CBV + 1)
};

enum class SHADER_VISIBILITY:std::uint8_t
{
    VISIBILITY_ALL = 0,
    VISIBILITY_VERTEX = 1,
    VISIBILITY_HULL = 2,
    VISIBILITY_DOMAIN = 3,
    VISIBILITY_GEOMETRY = 4,
    VISIBILITY_PIXEL = 5
};

struct ROOT_PARAMETER
{
    ROOT_PARAMETER
    (
        const std::string& name, 
        unsigned datasize, 
        unsigned maxDescriptor, 
        DESCRIPTOR_RANGE_TYPE vtype = DESCRIPTOR_RANGE_TYPE::CBV,
        unsigned baseRegister = 0,
        unsigned registerSpace = 0,
        ROOT_PARAMETER_TYPE ptype = ROOT_PARAMETER_TYPE::DESCRIPTOR_TABLE,
        unsigned tablesize = 1,
        SHADER_VISIBILITY visibility = SHADER_VISIBILITY::VISIBILITY_ALL
    ):
        name(name),
        ptype(ptype),
        tablesize(tablesize),
        vtype(vtype),
        visibility(visibility),
        baseRegister(baseRegister),
        registerSpace(registerSpace),
        datasize(datasize),
        maxDescriptor(maxDescriptor)
    {}
    std::string name;
    ROOT_PARAMETER_TYPE ptype = ROOT_PARAMETER_TYPE::DESCRIPTOR_TABLE;
    unsigned tablesize = 1;
    DESCRIPTOR_RANGE_TYPE vtype = DESCRIPTOR_RANGE_TYPE::CBV;
    SHADER_VISIBILITY visibility = SHADER_VISIBILITY::VISIBILITY_ALL;
    unsigned baseRegister = 0;
    unsigned registerSpace = 0;

    unsigned datasize;
    unsigned maxDescriptor;
};

struct ComputeShaderDescription {
    ComputeShaderDescription() = default;
    ComputeShaderDescription(const std::string& name) :name(name) {}

    std::string                 name;

    std::vector<stShaderStage>  shaderStage;

    bool                        addDefaultSamplers = false;
    std::vector<ROOT_PARAMETER> rootParameters;

    const stShaderStage& AddShaderStage
    (
        const std::wstring& file, Shader_Type type,
        const std::string& entry, const std::string& target
    )
    {
        shaderStage.emplace_back(file, type, entry, target);
        return shaderStage.back();
    }
    const stShaderStage& AddShaderStage
    (
        const std::wstring& file, Shader_Type type,
        const std::string& entry, const std::string& target,
        std::vector<std::pair<std::string, std::string>>& macros
    )
    {
        shaderStage.emplace_back(file, type, entry, target, macros);
        return shaderStage.back();
    }
    const stShaderStage& AddShaderStage
    (
        const std::string& data, Shader_Type type,
        const std::string& entry, const std::string& target
    )
    {
        shaderStage.emplace_back(data, type, entry, target);
        return shaderStage.back();
    }
    void AddRootParameter
    (
        const std::string& name,
        unsigned datasize,
        unsigned maxDescriptor,
        DESCRIPTOR_RANGE_TYPE vtype = DESCRIPTOR_RANGE_TYPE::CBV,
        unsigned baseRegister = 0,
        unsigned registerSpace = 0,
        ROOT_PARAMETER_TYPE ptype = ROOT_PARAMETER_TYPE::DESCRIPTOR_TABLE,
        unsigned tablesize = 1,
        SHADER_VISIBILITY visibility = SHADER_VISIBILITY::VISIBILITY_ALL
    )
    {
        rootParameters.emplace_back
        (
            name, datasize, maxDescriptor,
            vtype, baseRegister, registerSpace, ptype, tablesize, visibility
        );
    }
};

struct ShaderDescription {
    ShaderDescription() = default;
    ShaderDescription(const std::string& name) :name(name) {}
    ShaderDescription(const std::string& _name,
                        const std::vector<unsigned long>& _vertexFormats,
                        const std::vector<stSemanticName>& _semanticNames,
                        unsigned int                objCBSize,
                        unsigned int                passCBSize)
        : name(_name), vertexFormats(_vertexFormats),semanticNames(_semanticNames),
        objCBSize(objCBSize), passCBSize(passCBSize)
    {
        inputSlots.resize(vertexFormats.size(), 0);
    }
    std::string                 name;

    std::vector<stShaderStage>  shaderStage;

    std::vector<unsigned long>  vertexFormats;
    std::vector<unsigned int>   inputSlots;
    std::vector<stSemanticName> semanticNames;   // order must match vertexFormats

    bool                        useRootParamDescription = false;

    bool                        addDefaultSamplers = true;
    std::vector<ROOT_PARAMETER> rootParameters;

    unsigned int                texSRVDescriptorTableSize = 1;
    unsigned int                objCBSize = 1;
    unsigned int                multiObjCBSize = 0;
    unsigned int                materialCBSize = 0;
    unsigned int                passCBSize = 1;
    unsigned int                maxObjCBDescriptor = 100;
    unsigned int                maxTexSRVDescriptor = 64;

    // param0:register t0~tn
    // param1:register b0
    // param2:register b1
    // param3:register b2
    // param4:register b3
    unsigned int                texParamIndex = -1;
    unsigned int                objParamIndex = 1;
    unsigned int                multiObjParamIndex = -1;
    unsigned int                matParamIndex = -1;
    unsigned int                passParamIndex = 2;
    void ParamDefault()
    {
        texParamIndex = 0;
        objParamIndex = 1;
        matParamIndex = 2;
        passParamIndex = 3;
    }
    void ParamDefault2()
    {
        texParamIndex = 0;
        objParamIndex = 1;
        matParamIndex = 2;
        passParamIndex = 3;
        multiObjParamIndex = 4;
    }

    const stShaderStage& AddShaderStage
    (
        const std::wstring& file, Shader_Type type, 
        const std::string& entry, const std::string& target
    ) 
    {
        shaderStage.emplace_back(file, type, entry, target);
        return shaderStage.back();
    }
    const stShaderStage& AddShaderStage
    (
        const std::wstring& file, Shader_Type type,
        const std::string& entry, const std::string& target,
        std::vector<std::pair<std::string, std::string>>& macros
    ) 
    {
        shaderStage.emplace_back(file, type, entry, target, macros);
        return shaderStage.back();
    }
    const stShaderStage& AddShaderStage
    (
        const std::string& data, Shader_Type type,
        const std::string& entry, const std::string& target
    ) 
    {
        shaderStage.emplace_back(data, type, entry, target);
        return shaderStage.back();
    }
    void AddVertexInput
    (
        const std::string& semanticName, unsigned long format, 
        unsigned int slot = 0, unsigned int semanticIndex = 0
    ) 
    {
        semanticNames.emplace_back(semanticName, semanticIndex);
        inputSlots.emplace_back(slot);
        vertexFormats.emplace_back(format);
    }
    void AddRootParameter
    (
        const std::string& name,
        unsigned datasize,
        unsigned maxDescriptor,
        DESCRIPTOR_RANGE_TYPE vtype = DESCRIPTOR_RANGE_TYPE::CBV,
        unsigned baseRegister = 0,
        unsigned registerSpace = 0,
        ROOT_PARAMETER_TYPE ptype = ROOT_PARAMETER_TYPE::DESCRIPTOR_TABLE,
        unsigned tablesize = 1,
        SHADER_VISIBILITY visibility = SHADER_VISIBILITY::VISIBILITY_ALL
    )
    {
        rootParameters.emplace_back
        (
            name, datasize, maxDescriptor,
            vtype, baseRegister, registerSpace, ptype, tablesize, visibility
        );
    }
};

struct stViewport {
    float x;
    float y;
    float w;
    float h;
    float z0;
    float z1;
};

enum class SRV_DIMENSION :std::uint8_t
{
    UNKNOWN = 0,
    BUFFER = 1,
    TEXTURE1D = 2,
    TEXTURE1DARRAY = 3,
    TEXTURE2D = 4,
    TEXTURE2DARRAY = 5,
    TEXTURE2DMS = 6,
    TEXTURE2DMSARRAY = 7,
    TEXTURE3D = 8,
    TEXTURECUBE = 9,
    TEXTURECUBEARRAY = 10
};

struct stMaterialDesc
{
    struct TEX
    {
        explicit TEX(const std::string& name) :name(name) {}
        TEX(const std::string& name, SRV_DIMENSION dim) :name(name), viewDimension(dim) {}
        std::string name;
        SRV_DIMENSION viewDimension = SRV_DIMENSION::TEXTURE2D;
    };
    std::string name;
    std::string shaderName;
    std::vector<TEX> textures;
    size_t dataLen;
    const void* data;
};

struct stMSAADesc {
    stMSAADesc(UINT s, UINT q) :sampleCount(s), qualityLevels(q) {}
    UINT sampleCount;
    UINT qualityLevels;
};
typedef const std::vector<stMSAADesc>& CRef_MSAADesc_Vec;
} // end namespace