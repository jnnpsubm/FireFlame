#include "FLScene.h"
#include "..\Renderer\FLD3DRenderer.h"
#include "..\ShaderWrapper\FLD3DShaderWrapper.h"
#include "..\Timer\FLStopWatch.h"
#include "..\Engine\FLEngine.h"
#include "..\PSOManager\FLD3DPSOManager.h"

namespace FireFlame {
Scene::Scene(std::shared_ptr<D3DRenderer>& renderer) : mRenderer(renderer){}

void Scene::Update(const StopWatch& gt) {
    mUpdateFunc(gt.DeltaTime());

    Engine::GetEngine()->GetRenderer()->WaitForGPUFrame();

    UpdateObjectCBs(gt);
    //UpdateMainPassCB(gt);
}
void Scene::UpdateObjectCBs(const StopWatch& gt){
    auto currObjectCB = Engine::GetEngine()->GetRenderer()->GetCurrFrameResource()->ObjectCB.get();
    for (auto& e : mRenderItems){
        // Only update the cbuffer data if the constants have changed.  
        // This needs to be tracked per frame resource.
        auto& item = e.second;
        if (item->NumFramesDirty > 0){
            auto shaderName = item->Shader;
            auto shader = mShaders[shaderName];
            //currObjectCB->CopyData(e->ObjCBIndex, objConstants);
            shader->UpdateShaderCBData(item->ObjCBIndex, item->DataLen, item->Data);
            // Next FrameResource need to be updated too.
            item->NumFramesDirty--;
        }
    }
}
void Scene::Render(const StopWatch& gt) {
	mRenderer->Render(gt);
}
void Scene::PreRender() {
    mRenderer->SetCurrentPSO(nullptr);
}
void Scene::Draw(ID3D12GraphicsCommandList* cmdList) {
    for (auto& namedPrimitive : mPrimitives) {
        auto& primitive = namedPrimitive.second;
        auto mesh = primitive->GetMesh();
        if (!mesh->ResidentOnGPU()) {
            mesh->MakeResident2GPU(mRenderer->GetDevice(), mRenderer->GetCommandList());
        }
        //primitive->Draw(mRenderer.get());
    }
   
    // todo : some render items share the same pso and shader etc...
    // need to handle outside for loop
    auto renderer = Engine::GetEngine()->GetRenderer();
    for (auto& itemsTopType : mMappedRItems){
        D3D12_PRIMITIVE_TOPOLOGY_TYPE topType = (D3D12_PRIMITIVE_TOPOLOGY_TYPE)itemsTopType.first;
        for (auto& itemsShader : itemsTopType.second) {
            D3DShaderWrapper* Shader = mShaders[itemsShader.first].get();
            auto pso = Engine::GetEngine()->GetPSOManager()->GetPSO
            (
                itemsShader.first,
                renderer->GetMSAAMode(),
                topType,
                renderer->GetCullMode(),
                renderer->GetFillMode()
            );
            cmdList->SetPipelineState(pso);

            ID3D12DescriptorHeap* CBVHeap = Shader->GetCBVHeap();
            ID3D12DescriptorHeap* descriptorHeaps[] = { CBVHeap };
            cmdList->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);

            cmdList->SetGraphicsRootSignature(Shader->GetRootSignature());

            int passCbvIndex = Shader->GetFreePassCBV(renderer->GetCurrFrameResIndex());
            auto passCbvHandle = CD3DX12_GPU_DESCRIPTOR_HANDLE(CBVHeap->GetGPUDescriptorHandleForHeapStart());
            passCbvHandle.Offset(passCbvIndex, renderer->GetCbvSrvUavDescriptorSize());
            cmdList->SetGraphicsRootDescriptorTable(1, CBVHeap->GetGPUDescriptorHandleForHeapStart());

            for (auto& itemsOpaqueStatus : itemsShader.second) {
                bool opaqueStatus = itemsOpaqueStatus.first;
                opaqueStatus;
                for (auto& renderItem : itemsOpaqueStatus.second) {
                    renderItem->Render(Shader);
                }
            }
        }
    }
}
int Scene::GetReady() {
    mRenderer->RegisterPreRenderFunc(std::bind(&Scene::PreRender, this));
    mRenderer->RegisterDrawFunc(std::bind(&Scene::Draw, this, std::placeholders::_1));

    mRenderer->ResetCommandList();
    // make mesh resident to GPU memory
    for (auto& namedPrimitive : mPrimitives){
        auto& primitive = namedPrimitive.second;
        primitive->GetMesh()->MakeResident2GPU(mRenderer->GetDevice(), mRenderer->GetCommandList());
    }
    mRenderer->ExecuteCommand();
    // Wait until initialization is complete.
    mRenderer->WaitForGPU();
    return 0;
}
void Scene::AddShader(const stShaderDescription& shaderDesc) {
    std::shared_ptr<D3DShaderWrapper> shader = nullptr;
    auto it = mShaders.find(shaderDesc.name);
    if (it == mShaders.end()) {
        shader = std::make_shared<D3DShaderWrapper>(shaderDesc.name);
        mShaders[shaderDesc.name] = shader;
    }
    else {
        shader = mShaders[shaderDesc.name];
    }
    //shader->BuildCBVDescriptorHeaps(mRenderer->GetDevice(), 1);
    //shader->BuildConstantBuffers(mRenderer->GetDevice(), shaderDesc.objCBSize);
    shader->BuildFrameCBResources(shaderDesc.objCBSize, shaderDesc.passCBSize, 100, 3);
    shader->BuildRootSignature(mRenderer->GetDevice());
    shader->BuildShadersAndInputLayout(shaderDesc);
    shader->BuildPSO
    (
        mRenderer->GetDevice(),             
        mRenderer->GetBackBufferFormat(),
        mRenderer->GetDepthStencilFormat()
    );
}
void Scene::AddPrimitive(const stRawMesh& mesh) {
    mPrimitives.emplace(mesh.name, std::make_unique<D3DPrimitive>(mesh));
}
void Scene::AddPrimitive(const stRawMesh& mesh, const std::string& shaderName) {
	mPrimitives.emplace(mesh.name, std::make_unique<D3DPrimitive>(mesh));
    auto it = mShaders.find(shaderName);
    if (it == mShaders.end()) throw std::exception("cannot find the shader");
    auto shader = it->second;
    mPrimitives[mesh.name]->SetShader(shader);
}
void Scene::PrimitiveUseShader(const std::string& primitive, const std::string& shader) {
    auto itPrimitive = mPrimitives.find(primitive);
    if (itPrimitive == mPrimitives.end()) throw std::exception("cannot find primitive");
    auto itShader = mShaders.find(shader);
    if (itShader == mShaders.end()) throw std::exception("cannot find shader");
    itPrimitive->second->SetShader(itShader->second);
}
void Scene::RenderItemChangeShader(const std::string& renderItem, const std::string& shader) {
    auto itRItem = mRenderItems.find(renderItem);
    if (itRItem == mRenderItems.end()) 
        throw std::exception("cannot find render item in function(RenderItemChangeShader)");
    auto itShader = mShaders.find(shader);
    if (itShader == mShaders.end()) 
        throw std::exception("cannot find shader in function(RenderItemChangeShader)");
    std::string oldShader = itRItem->second->Shader;
    itRItem->second->SetShader(shader);

    // first remove the render itme
    for (auto& itemsTopType : mMappedRItems) {
        auto& itemsShader = itemsTopType.second;
        auto it = itemsShader.find(oldShader);
        if (it == itemsShader.end()) continue;
        for (auto& itemsOpaqueStatus : it->second) {
            auto& vecRItems = itemsOpaqueStatus.second;
            for (auto it = vecRItems.begin(); it != vecRItems.end(); ++it) {
                if ((*it)->Name == renderItem) {
                    vecRItems.erase(it);
                    break;
                }
            }
        }
    }
    // readd render item
    auto& shaderMappedRItem = mMappedRItems[(UINT)D3DPrimitiveType(itRItem->second->PrimitiveType)];
    auto& psoMappedRItem = shaderMappedRItem[shader];
    auto& vecItems = psoMappedRItem[true];
    vecItems.push_back(itRItem->second.get());
}

void Scene::PrimitiveAddSubMesh(const std::string& name, const stRawMesh::stSubMesh& subMesh){
	auto it = mPrimitives.find(name);
	if (it == mPrimitives.end()) return;
    D3DMesh* mesh = mPrimitives[name]->GetMesh();
    mesh->AddSubMesh(subMesh);
}
void Scene::AddRenderItem
(
    const std::string&      primitiveName,
    const std::string&      shaderName,
    const stRenderItemDesc& desc
)
{
    auto itPrimitive = mPrimitives.find(primitiveName);
    if (itPrimitive == mPrimitives.end()) throw std::exception("cannot find primitive");
    auto itShader = mShaders.find(shaderName);
    if (itShader == mShaders.end()) throw std::exception("cannot find shader");
   
    D3DMesh* mesh = itPrimitive->second->GetMesh();
    D3DShaderWrapper* shader = itShader->second.get();

    auto renderItem = std::make_shared<D3DRenderItem>();
    renderItem->Name = desc.name;
    renderItem->NumFramesDirty = Engine::NumFrameResources();
    renderItem->IndexCount = desc.subMesh.indexCount;
    renderItem->StartIndexLocation = desc.subMesh.startIndexLocation;
    renderItem->BaseVertexLocation = desc.subMesh.baseVertexLocation;
    renderItem->ObjCBIndex = shader->GetFreeObjCBV();
    renderItem->Mesh = mesh;
    renderItem->Shader = shaderName;
    renderItem->PrimitiveType = FLPrimitiveTop2D3DPrimitiveTop(desc.topology);

    // All the render items are opaque(true). for now...
    auto& shaderMappedRItem = mMappedRItems[(UINT)D3DPrimitiveType(renderItem->PrimitiveType)];
    auto& psoMappedRItem = shaderMappedRItem[shaderName];
    auto& vecItems = psoMappedRItem[true];
    vecItems.push_back(renderItem.get());
    mRenderItems.emplace(desc.name, renderItem);
}
void Scene::UpdateRenderItemCBData(const std::string& name, size_t size, const void* data) {
    if (mRenderItems.find(name) == mRenderItems.end()) {
        throw std::exception("UpdateRenderItemCBData cannot find renderitem");
    }
    auto& item = mRenderItems[name];
    if (item->Data == nullptr) {
        item->Data = new char[size];
        item->DataLen = size;
    }
    memcpy(item->Data, data, size);
    item->NumFramesDirty = Engine::NumFrameResources();

    //mRenderItems[name]->Shader->UpdateShaderCBData(mRenderItems[name]->ObjCBIndex, size, data);
}
} // end namespace