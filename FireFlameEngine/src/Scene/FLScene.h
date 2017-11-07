#pragma once
#include <unordered_map>
#include <functional>
#include "Primitive\FLD3DPrimitive.h"
#include "Vertex\FLVertex.h"
#include "..\FLTypeDefs.h"
#include "..\ShaderWrapper\FLD3DShaderWrapper.h"

namespace FireFlame {
class D3DRenderer;
class StopWatch;
class Scene {
public:
	Scene(std::shared_ptr<D3DRenderer>& renderer);

    // some scene management
    int  GetReady();

	void Update(const StopWatch& gt);
	void Render(const StopWatch& gt);

    void AddShader(const stShaderDescription& shaderDesc);
    void AddPrimitive(const stRawMesh& mesh);
	void AddPrimitive(const stRawMesh& mesh, const std::string& shaderName);
	void PrimitiveAddSubMesh(const std::string& name, const stRawMesh::stSubMesh& subMesh);

    void PrimitiveUseShader(const std::string& primitive, const std::string& shader);

    template <typename T>
    void UpdateShaderCBData(std::string shaderName, unsigned int index, const T& data) {
        auto it = mShaders.find(shaderName);
        if (it == mShaders.end()) return;
        auto& shader = it->second;
        shader->UpdateShaderCBData(index, data);
    }

    // register callbacks
    void RegisterUpdateFunc(std::function<void(float)> func) { mUpdateFunc = func; }

private:
    std::shared_ptr<D3DRenderer> mRenderer;

    void PreRender();
    void Draw(ID3D12GraphicsCommandList* cmdList);

    // callbacks
    std::function<void(float)> mUpdateFunc = [](float) {};

	// todo : scene manage
	std::unordered_map<std::string, std::unique_ptr<D3DPrimitive>>     mPrimitives; 
    std::unordered_map<std::string, std::shared_ptr<D3DShaderWrapper>> mShaders;
};
}