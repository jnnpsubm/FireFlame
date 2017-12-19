#include "PCH.h"
#include "FLScene.h"
#include "RenderItem\FLD3DRenderItem.h"
#include "..\Renderer\FLD3DRenderer.h"
#include "..\ShaderWrapper\FLD3DShaderWrapper.h"
#include "..\ShaderWrapper\FLD3DComputeShaderWrapper.h"
#include "..\Timer\FLStopWatch.h"
#include "..\Engine\FLEngine.h"
#include "..\PSOManager\FLD3DPSOManager.h"
#include "..\PSOManager\FLD3DPSOManager2.h"
#include "Pass\FLPass.h"
#include "..\Material\FLMaterial.h"
#include "..\Texture\FLD3DTexture.h"
#include "..\Utility\chrono\FLchrono.h"
#include "..\3rd_utils\spdlog\spdlog.h"
#ifdef USE_MS_DDS_LOADER
#include "..\3rd_utils\DDSTextureLoader12.h"
#else
#include "..\3rd_utils\DDSTextureLoaderLuna.h"
#endif
#include "..\ShaderWrapper\ShaderConstBuffer\FLShaderConstBuffer.h"
#include "CSTask\FLD3DCSTask.h"
#include "..\Texture\DynamicTexture\FLD3DTextureWaves.h"

namespace FireFlame {
Scene::Scene(std::shared_ptr<D3DRenderer>& renderer) : mRenderer(renderer){}

void Scene::Update(const StopWatch& gt) {
    Engine::GetEngine()->GetRenderer()->WaitForGPUFrame();
    mUpdateFunc(gt.DeltaTime());
    UpdateObjectCBs(gt);
    UpdateMaterialCBs(gt);
}
void Scene::UpdateObjectCBs(const StopWatch& gt){
    //auto currObjectCB = Engine::GetEngine()->GetRenderer()->GetCurrFrameResource()->ObjectCB.get();
    auto& shaderRes = Engine::GetEngine()->GetRenderer()->GetCurrFrameResource()->ShaderResources;
    for (auto& e : mRenderItems){
        // Only update the cbuffer data if the constants have changed.  
        // This needs to be tracked per frame resource.
        auto& item = e.second;
        if (item->NumFramesDirty > 0){
            auto shaderName = item->Shader;
            auto shader = mShaders[shaderName];
            auto currObjectCB = shaderRes[shaderName].ObjectCB.get();
            //currObjectCB->CopyData(e->ObjCBIndex, objConstants);
            shader->UpdateObjCBData(item->ObjCBIndex, item->DataLen, item->Data);
            //Matrix4X4* m = (Matrix4X4*)item->Data;
            // Next FrameResource need to be updated too.
            item->NumFramesDirty--;
        }
    }
}

void Scene::UpdateMaterialCBs(const StopWatch& gt)
{
    //auto currMaterialCB = Engine::GetEngine()->GetRenderer()->GetCurrFrameResource()->MaterialCB.get();
    auto& shaderRes = Engine::GetEngine()->GetRenderer()->GetCurrFrameResource()->ShaderResources;
    for (auto& e : mMaterials)
    {
        // Only update the cbuffer data if the constants have changed.  If the cbuffer
        // data changes, it needs to be updated for each FrameResource.
        Material* mat = e.second.get();
        if (mat->NumFramesDirty > 0)
        {
            auto currMaterialCB = shaderRes[mat->ShaderName].MaterialCB.get();
            currMaterialCB->CopyData(mat->MatCBIndex, mat->dataLen, mat->data);
            mat->NumFramesDirty--;
        }
    }
}

void Scene::Quit()
{
    mQuit = true;
    if (mCompute)
    {
        if (mCompute->wait_for(std::chrono::seconds(60)) == std::future_status::ready)
        {
            spdlog::get("console")->info("Scene quited......");
        }
        else
        {
            throw std::runtime_error("Can not quit compute thread......");
        }
    }
}

void Scene::Render(const StopWatch& gt) {
	mRenderer->Render(gt);
    
    if (mPrintScene)
    {
        mPrintScene = false;
        std::wstring filename(L"ScreenShot_");
        filename += AnsiToWString(NowAsFileName());
        filename += L".bmp";
        mRenderer->GrabScreen(filename);
    }
}
void Scene::PreRender() {
    mRenderer->SetCurrentPSO(nullptr);
}
void Scene::Draw(ID3D12GraphicsCommandList* cmdList) 
{
    for (auto& tex : mTextures)
    {
        if(tex.second->needUpdate) tex.second->Update(cmdList);
    }
    for (auto& pass : mPasses)
    {
        DrawPass(cmdList, pass.second.get());
    }
}
void Scene::DrawPass(ID3D12GraphicsCommandList* cmdList, const Pass* pass)
{
    for (auto& namedPrimitive : mPrimitives) 
    {
        auto& primitive = namedPrimitive.second;
        auto mesh = primitive->GetMesh();
        if (!mesh->ResidentOnGPU()) 
        {
            mesh->MakeResident2GPU(mRenderer->GetDevice(), mRenderer->GetCommandList());
        }
    }

    auto cmp = [](const RItemsWithPriority& rhs, const RItemsWithPriority& lhs) {
        return lhs.first < rhs.first;
    };
    std::priority_queue<RItemsWithPriority, std::vector<RItemsWithPriority>, decltype(cmp)>
        renderItemQueue(cmp);
    for (auto& pairPriorityAndOpacityMapped : mPriorityMappedRItems)
    {
        int priority = pairPriorityAndOpacityMapped.first;
        renderItemQueue.emplace(priority, &pairPriorityAndOpacityMapped.second);
    }
    while (!renderItemQueue.empty()) {
        auto& items = renderItemQueue.top();
        DrawRenderItems(cmdList, pass, items.first, true);
        DrawRenderItems(cmdList, pass, items.first, false);
        renderItemQueue.pop();
    }
}

void Scene::DrawRenderItems
(
    ID3D12GraphicsCommandList* cmdList,
    const Pass* pass,
    int priority,
    bool opaque
)
{
    auto renderer = Engine::GetEngine()->GetRenderer();
    auto& shaderMapped = GetShaderMappedRItem(priority, opaque);
    for (auto& itSameShader : shaderMapped)
    {
        D3DShaderWrapper* Shader = mShaders[itSameShader.first].get();

        auto CBVHeap = Shader->GetCBVHeap();
        ID3D12DescriptorHeap* descriptorHeaps[] = { CBVHeap };
        cmdList->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);

        cmdList->SetGraphicsRootSignature(Shader->GetRootSignature());

        if (Shader->GetPassParamIndex() != (UINT)-1 && Shader->GetPassCBVIndex() != (UINT)-1)
        {
            UINT passCbvIndex = Shader->GetPassCBVIndex();
            passCbvIndex += renderer->GetCurrFrameResIndex() * Shader->GetPassCBVMaxCount();
            auto passCbvHandle = CD3DX12_GPU_DESCRIPTOR_HANDLE(CBVHeap->GetGPUDescriptorHandleForHeapStart());
            passCbvHandle.Offset(passCbvIndex, renderer->GetCbvSrvUavDescriptorSize());
            cmdList->SetGraphicsRootDescriptorTable(Shader->GetPassParamIndex(), passCbvHandle);
        }
        for (auto& itSamePSO : itSameShader.second)
        {
            auto pso = Engine::GetEngine()->GetPSOManager2()->GetPSO
            (
                itSamePSO.first,
                renderer->GetMSAAMode(),
                renderer->GetFillMode()
            );
            cmdList->SetPipelineState(pso);
            for (auto& renderItem : itSamePSO.second)
            {
                if (!renderItem->Primitive->Visible()) continue;
                if (!renderItem->Visible) continue;
                renderItem->Render(Shader);
            }
        }
    }
}

int Scene::GetReady() {
    mRenderer->RegisterPreRenderFunc(std::bind(&Scene::PreRender, this));
    mRenderer->RegisterDrawFunc(std::bind(&Scene::Draw, this, std::placeholders::_1));

    // if no pass, add a default pass
    if (mPasses.empty() && !mShaders.empty() && !mRenderItems.empty())
    {
        AddPass("DefaultPass");
    }

    mRenderer->ResetCommandList();
    // make mesh resident to GPU memory
    for (auto& namedPrimitive : mPrimitives){
        auto& primitive = namedPrimitive.second;
        primitive->GetMesh()->MakeResident2GPU(mRenderer->GetDevice(), mRenderer->GetCommandList());
    }
    mRenderer->ExecuteCommand();
    // Wait until initialization is complete.
    mRenderer->WaitForGPU();
    for (auto& namedPrimitive : mPrimitives) {
        auto& primitive = namedPrimitive.second;
        primitive->GetMesh()->DisposeUploaders();
    }

    PrintAllPasses();
    PrintAllShaders();
    PrintAllPSOs();
    PrintAllPassCBs();
    PrintAllMultiObjCBs();
    PrintAllPrimitives();
    PrintAllTextures();
    PrintAllMaterials();
    PrintAllRenderItems();
    return 0;
}

int Scene::Compute(std::shared_ptr<D3DRenderer> renderer)
{
    auto engine = Engine::GetEngine();
    auto device = renderer->GetDevice();
    auto cmdList = renderer->GetComputeCmdList();
    while (!mQuit && !mCSTasks.empty())
    {
        std::lock_guard<std::mutex> lock(mComputeMutex);
        //std::this_thread::yield();
        for (auto& itTask : mCSTasks)
        {
            auto task = itTask.second.get();
            if (task->status == CSTask::Initial)
            {
                auto cmdAlloc = mCSTaskCmdAllocs[itTask.first];
                ThrowIfFailed(cmdAlloc->Reset());
                ThrowIfFailed(cmdList->Reset(cmdAlloc.Get(), nullptr));
                auto shader = mComputeShaders[task->shaderName].get();
                shader->Dispatch(device, cmdList, *task);
                renderer->ExecuteCommand(cmdList);
                task->fence = renderer->SetFence();
                task->status = CSTask::Dispatched;
                spdlog::get("console")->info("task:{0} dispatched at {1:f}", itTask.first, engine->TotalTime());
            }
        }
        for (auto& itTask : mCSTasks)
        {
            auto task = itTask.second.get();
            if (task->status == CSTask::Dispatched && renderer->FenceReached(task->fence))
            {
                spdlog::get("console")->info("task:{0} finished at {1:f}", itTask.first, engine->TotalTime());
                if (task->needCopyback())
                {
                    spdlog::get("console")->info("task:{0} start copyback at {1:f}", itTask.first, engine->TotalTime());
                    auto cmdAlloc = mCSTaskCmdAllocs[itTask.first];
                    ThrowIfFailed(cmdAlloc->Reset());
                    ThrowIfFailed(cmdList->Reset(cmdAlloc.Get(), nullptr));
                    auto shader = mComputeShaders[task->shaderName].get();
                    shader->Copyback(device, cmdList, *task);
                    renderer->ExecuteCommand(cmdList);
                    task->fence = renderer->SetFence();
                    task->status = CSTask::Copyback;
                }
                else
                {
                    task->status = CSTask::Done;
                }
            }
        }
        for (auto& itTask : mCSTasks)
        {
            auto task = itTask.second.get();
            if (task->status == CSTask::Copyback && renderer->FenceReached(task->fence))
            {
                spdlog::get("console")->info("task:{0} copyback finished at {1:f}", itTask.first, engine->TotalTime());
                // call back
                auto shader = mComputeShaders[task->shaderName].get();
                shader->TaskCallback(*task);
                task->status = CSTask::Done;
            }
        }
        for (auto itTask = mCSTasks.begin(); itTask != mCSTasks.end();)
        {
            auto task = itTask->second.get();
            if (task->status == CSTask::Done)
            {
                auto shader = mComputeShaders[task->shaderName].get();
                shader->ClearTaskRes(task->name);
                mCSTaskCmdAllocs.erase(task->name);
                itTask = mCSTasks.erase(itTask);
            }
            else ++itTask;
        }
    }
    return 0;
}

void Scene::AddShader(const ShaderDescription& shaderDesc) 
{
    std::shared_ptr<D3DShaderWrapper> shader = nullptr;
    auto it = mShaders.find(shaderDesc.name);
    if (it == mShaders.end()) 
    {
        shader = std::make_shared<D3DShaderWrapper>(shaderDesc.name);
        mShaders[shaderDesc.name] = shader;
    }
    else 
    {
        spdlog::get("console")->warn("Shader {0} already exist......", shaderDesc.name);
        return;
    }

    if (shaderDesc.useRootParamDescription)
    {
        shader->BuildRootInputResources(shaderDesc);
        shader->BuildRootSignature(mRenderer->GetDevice(), shaderDesc);
    }
    else
    {
        shader->SetParamIndex
        (
            shaderDesc.texParamIndex, shaderDesc.objParamIndex, shaderDesc.multiObjParamIndex,
            shaderDesc.matParamIndex, shaderDesc.passParamIndex
        );
        shader->BuildRootInputResources
        (
            shaderDesc.objCBSize, shaderDesc.maxObjCBDescriptor,
            shaderDesc.passCBSize, 3,
            shaderDesc.materialCBSize, shaderDesc.materialCBSize ? 100 : 0,
            shaderDesc.texSRVDescriptorTableSize, shaderDesc.maxTexSRVDescriptor,
            shaderDesc.multiObjCBSize, shaderDesc.multiObjCBSize ? 5 : 0
        );
        shader->BuildRootSignature(mRenderer->GetDevice());
    }
    
    shader->BuildShadersAndInputLayout(shaderDesc);

    // normally one pass const buffer for one shader,
    // but if that is not the case,use multiObj const buffer or add pass const buffer manually maybe
    std::string passName = shader->GetDefaultPassCBName();
    AddPassCB(shaderDesc.name, passName);
    SetShaderPassCB(shaderDesc.name, passName);
}

void Scene::AddComputeShader(const ComputeShaderDescription& desc)
{
    std::shared_ptr<D3DComputeShaderWrapper> shader = nullptr;
    auto it = mComputeShaders.find(desc.name);
    if (it == mComputeShaders.end())
    {
        shader = std::make_shared<D3DComputeShaderWrapper>(desc.name);
        mComputeShaders[desc.name] = shader;
    }
    else
    {
        spdlog::get("console")->warn("ComputeShader {0} already exist......", desc.name);
        return;
    }
    shader->BuildRootSignature(mRenderer->GetDevice(), desc);
    shader->BuildShader(desc);
}

void Scene::AddPSO(const std::string& name, const PSODesc& desc)
{
    auto PSOManager = Engine::GetEngine()->GetPSOManager2();
    if (PSOManager->NameExist(name))
    {
        spdlog::get("console")->error("PSO {0} already exist......", name);
        return;
    }
    PSOManager->AddPSO(name, desc);
}

void Scene::AddComputePSO(const std::string& name, const ComputePSODesc& desc)
{
    auto PSOManager = Engine::GetEngine()->GetPSOManager2();
    if (PSOManager->ComputePSONameExist(name))
    {
        spdlog::get("console")->error("ComputePSO {0} already exist......", name);
        return;
    }
    PSOManager->AddComputePSO(name, desc);
}

void Scene::SetCSRootParamData
(
    const std::string& taskName, const std::string& shaderName, const std::string& paramName, 
    const ResourceDesc& resDesc, size_t dataLen, std::uint8_t* data
)
{
    auto itShader = mComputeShaders.find(shaderName);
    if (itShader == mComputeShaders.end())
    {
        spdlog::get("console")->critical("can not find compute shader {0} in SetCSRootParamData", shaderName);
        throw std::runtime_error("can not find compute shader in SetCSRootParamData");
    }
    std::lock_guard<std::mutex> lock(mComputeMutex);
    itShader->second->SetCSRootParamData(taskName, paramName, resDesc, dataLen, data);
}
void Scene::AddCSTask(const CSTaskDesc& desc)
{
    auto it = mComputeShaders.find(desc.shaderName);
    if (it == mComputeShaders.end())
    {
        spdlog::get("console")->critical("can not find compute shader {0} in AddCSTask", desc.shaderName);
        throw std::runtime_error("can not find compute shader in AddCSTask");
    }
    auto task = std::make_unique<CSTask>
    (
        desc.name, desc.shaderName, desc.PSOName,
        desc.GroupSize.X, desc.GroupSize.Y, desc.GroupSize.Z,
        desc.callback
    );

    std::lock_guard<std::mutex> lock(mComputeMutex);
    auto itTask = mCSTasks.find(desc.name);
    if (itTask != mCSTasks.end())
    {
        spdlog::get("console")->critical("compute task {0} already in process", desc.name);
    }
    mCSTasks.emplace(desc.name, std::move(task));
    ThrowIfFailed
    (
        Engine::GetEngine()->GetRenderer()->GetDevice()->CreateCommandAllocator
        (
            D3D12_COMMAND_LIST_TYPE_DIRECT,
            IID_PPV_ARGS(mCSTaskCmdAllocs[desc.name].ReleaseAndGetAddressOf())
        )
    );
    if (!mCompute || mCompute->wait_for(std::chrono::seconds(0)) == std::future_status::ready)
    {
        mQuit = false;
        mCompute = std::make_unique<std::future<int>>(std::async(std::launch::async, [=]() { return Compute(Engine::GetEngine()->GetRenderer()); }));
    }
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

void Scene::PrimitiveVisible(const std::string& name, bool visible)
{
    auto itPrimitive = mPrimitives.find(name);
    if (itPrimitive == mPrimitives.end())
    {
        spdlog::get("console")->warn("Primitive {0} not found in function(PrimitiveVisible)......", name);
        return;
    }
    itPrimitive->second->SetVisible(visible);
}

void Scene::RenderItemVisible(const std::string& name, bool visible)
{
    auto itRItem = mRenderItems.find(name);
    if (itRItem == mRenderItems.end())
    {
        spdlog::get("console")->warn("Render item {0} not found in function(RenderItemVisible)......", name);
        return;
    }
    itRItem->second->SetVisible(visible);
}

void Scene::RenderItemChangeShader
(
    const std::string& renderItem,
    int                priority,
    const stRenderItemDesc& desc,
    const std::string&      shader,
    const std::string&      shaderMacroVS,
    const std::string&      shaderMacroPS
) 
{
    auto itRItem = mRenderItems.find(renderItem);
    if (itRItem == mRenderItems.end()) 
        throw std::exception("cannot find render item in function(RenderItemChangeShader)");
    auto itShader = mShaders.find(shader);
    if (itShader == mShaders.end()) 
        throw std::exception("cannot find shader in function(RenderItemChangeShader)");
    std::string oldShader = itRItem->second->Shader;

    auto& shaderMapped = GetShaderMappedRItem(priority, itRItem->second->opaque);
    auto itSameShader = shaderMapped.find(oldShader);
    if (itSameShader == shaderMapped.end())
    {
        spdlog::get("console")->error("cannot find old shader {0}", oldShader);
        return;
    }

    // first remove the render item
    bool bFound = false;
    for (auto& itSamePSO : itSameShader->second) {
        auto& vecRItems = itSamePSO.second;
        for (auto it = vecRItems.begin(); it != vecRItems.end(); ++it) {
            if ((*it)->Name == renderItem) {
                vecRItems.erase(it);
                bFound = true;
                break;
            }
        }
    }
    if (!bFound)
    {
        spdlog::get("console")->error("cannot find render item {0}", renderItem);
        return;
    }
    itRItem->second->SetShader(shader);
    auto PSOManager = Engine::GetEngine()->GetPSOManager2();
    auto PSOName = shader + shaderMacroVS + shaderMacroPS + desc.AsPSOName();
    if (!PSOManager->NameExist(PSOName))
    {
        AddPSO(PSOName, { shader,shaderMacroVS,shaderMacroPS,desc.opaque,desc.topology,desc.cullMode });
    }
    // add render item back to new place
    auto& PSOMapped = shaderMapped[shader];
    auto& vecItems = PSOMapped[PSOName];
    vecItems.push_back(itRItem->second.get());
}

void Scene::RenderItemChangeMaterial(const std::string& renderItem, const std::string& matname)
{
    auto itRItem = mRenderItems.find(renderItem);
    if (itRItem == mRenderItems.end())
    {
        spdlog::get("console")->error("cannot find render item in function(RenderItemChangeMaterial)");
        throw std::exception("cannot find render item in function(RenderItemChangeMaterial)");
    } 
    auto itMat = mMaterials.find(matname);
    if (itMat == mMaterials.end())
    {
        spdlog::get("console")->error("cannot find material in function(RenderItemChangeMaterial)");
        throw std::exception("cannot find material in function(RenderItemChangeMaterial)");
    }

    auto renderitem = itRItem->second;
    auto mat = itMat->second;
    renderitem->Mat = mat.get();
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
    const std::string&      shaderMacroVS,
    const std::string&      shaderMacroPS,
    const stRenderItemDesc& desc
)
{
    auto PSOManager = Engine::GetEngine()->GetPSOManager2();
    auto PSOName = shaderName + shaderMacroVS + shaderMacroPS + desc.AsPSOName();
    if (!PSOManager->NameExist(PSOName))
    {
        AddPSO(PSOName,PSODesc(shaderName,shaderMacroVS,shaderMacroPS,desc.opaque,desc.topology,desc.cullMode));
    }
    AddRenderItem(primitiveName, shaderName, PSOName, desc);
}

void Scene::AddRenderItem
(
    const std::string&      primitiveName,
    const std::string&      shaderName,
    const std::string&      PSOName,
    const std::string&      MultiObjCBName,
    int                     priority,
    const stRenderItemDesc& desc
)
{
    auto itPrimitive = mPrimitives.find(primitiveName);
    if (itPrimitive == mPrimitives.end())
    {
        spdlog::get("console")->error("cannot find primitive {0}", primitiveName);
        throw std::exception("cannot find primitive");
    }
    auto itShader = mShaders.find(shaderName);
    if (itShader == mShaders.end())
    {
        spdlog::get("console")->error("cannot find shader {0}", shaderName);
        throw std::exception("cannot find shader");
    }

    auto PSOManager = Engine::GetEngine()->GetPSOManager2();
    if (!PSOManager->NameExist(PSOName))
    {
        spdlog::get("console")->error("cannot find PSO {0}", PSOName);
        throw std::exception("cannot find PSO in AddRenderItem");
    }
    D3DPrimitive* primitive = itPrimitive->second.get();
    D3DShaderWrapper* shader = itShader->second.get();

    auto renderItem = std::make_shared<D3DRenderItem>();
    renderItem->Name = desc.name;
    renderItem->NumFramesDirty = Engine::NumFrameResources();
    renderItem->IndexCount = desc.subMesh.indexCount;
    renderItem->StartIndexLocation = desc.subMesh.startIndexLocation;
    renderItem->BaseVertexLocation = desc.subMesh.baseVertexLocation;
    renderItem->ObjCBIndex = shader->GetFreeObjCBV();
    if (!MultiObjCBName.empty())
    {
        auto itMultiObj = mMultiObjCBs.find(MultiObjCBName);
        if (itMultiObj == mMultiObjCBs.end())
            throw std::exception("cannot find MultiObjCB in AddRenderItem");
        renderItem->MultiObjCBIndex = itMultiObj->second->CBIndex;
    }
    renderItem->Primitive = primitive;
    renderItem->Shader = shaderName;
    renderItem->PrimitiveType = FLPrimitiveTop2D3DPrimitiveTop(desc.topology);
    renderItem->opaque = desc.opaque;
    renderItem->stencilRef = desc.stencilRef;
    if (desc.data && desc.dataLen)
    {
        renderItem->Data = new char[desc.dataLen];
        renderItem->DataLen = desc.dataLen;
        memcpy(renderItem->Data, desc.data, desc.dataLen);
    }
    if (!desc.mat.empty())
    {
        auto itMat = mMaterials.find(desc.mat);
        if (itMat == mMaterials.end())
            throw std::exception("cannot find material in AddRenderItem");
        renderItem->Mat = itMat->second.get();
    }

    auto& shaderMapped = GetShaderMappedRItem(priority, desc.opaque);
    auto& PSOMapped = shaderMapped[shaderName];
    auto& vecItems = PSOMapped[PSOName];
    vecItems.push_back(renderItem.get());

    mRenderItems.emplace(desc.name, renderItem);
}

void Scene::AddTexture(const std::string& name, const std::wstring& filename)
{
    auto tex = std::make_shared<D3DTexture>(name, filename);
    auto renderer = Engine::GetEngine()->GetRenderer();
    renderer->ResetCommandList();
    ThrowIfFailed
    (
        DirectX::CreateDDSTextureFromFile12
        (
            renderer->GetDevice(),
            renderer->GetCommandList(),
            filename.c_str(),
            tex->resource, tex->uploadHeap
        )
    );
    renderer->ExecuteCommand();
    renderer->WaitForGPU();
    tex->uploadHeap = nullptr;
    D3D12_RESOURCE_DESC desc = tex->resource->GetDesc();
    mTextures[tex->name] = std::move(tex);
}

void Scene::AddTextureGroup(const std::string& name, std::vector<std::wstring> filenames)
{

}

void Scene::AddTexture(const std::string& name, std::uint8_t* data, size_t len)
{
    auto tex = std::make_shared<D3DTexture>(name);
    auto renderer = Engine::GetEngine()->GetRenderer();
    renderer->ResetCommandList();
    ThrowIfFailed
    (
        DirectX::CreateDDSTextureFromMemory12
        (
            renderer->GetDevice(),
            renderer->GetCommandList(),
            data,len,
            tex->resource, tex->uploadHeap
        )
    );
    renderer->ExecuteCommand();
    renderer->WaitForGPU();
    tex->uploadHeap = nullptr;
    mTextures[tex->name] = std::move(tex);
}

void Scene::AddTexture2D
(
    const std::string& name,
    const std::uint8_t* data,
    unsigned long format,
    unsigned long width,
    unsigned long height
)
{
    auto tex = std::make_shared<D3DTexture>(name);
    auto renderer = Engine::GetEngine()->GetRenderer();
    renderer->ResetCommandList();
    tex->resource = D3DUtils::CreateDefaultTexture2D
    (
        renderer->GetDevice(),
        renderer->GetCommandList(),
        data, FireFlame::FLVertexFormat2DXGIFormat(format), width, height,
        tex->uploadHeap
    );
    renderer->ExecuteCommand();
    renderer->WaitForGPU();
    tex->uploadHeap = nullptr;
    mTextures[tex->name] = std::move(tex);
}

void Scene::AddTextureWaves
(
    const std::string& name, 
    unsigned width, unsigned height, 
    unsigned disturbCount, 
    float dx, float dt, float speed, float damping
)
{
    auto renderer = Engine::GetEngine()->GetRenderer();
    renderer->ResetCommandList();

    auto tex = std::make_shared<D3DTextureWaves>
    (
        name, renderer->GetDevice(),renderer->GetCommandList(), width, height, 
        dx,dt,speed,damping,disturbCount
    );
    renderer->ExecuteCommand();
    renderer->WaitForGPU();
    tex->ClearUploadBuffer();
    mTextures[tex->name] = std::move(tex);
}

void Scene::AnimateTexture(const std::string& name)
{
    auto itTex = mTextures.find(name);
    if (itTex == mTextures.end())
    {
        spdlog::get("console")->warn("can not find texture {0}", name);
        return;
    }
    itTex->second->needUpdate = true;
}

void Scene::AddMaterial(const stMaterialDesc& matDesc)
{
    auto itShader = mShaders.find(matDesc.shaderName);
    if (itShader == mShaders.end())
        throw std::exception("cannot find shader in AddMaterial");
    auto shader = itShader->second;

    auto mat = std::make_shared<Material>(matDesc.name, Engine::NumFrameResources());
    mat->ShaderName = matDesc.shaderName;
    mat->MatCBIndex = shader->GetFreeMatCBV();
    assert(matDesc.textures.size() <= shader->GetTexSRVDescriptorTableSize());
    if (!matDesc.textures.empty()) mat->DiffuseSrvHeapIndex = shader->CreateTexSRV(matDesc.textures);
    
    if (matDesc.dataLen && matDesc.data)
    {
        mat->data = new char[matDesc.dataLen];
        mat->dataLen = matDesc.dataLen;
        memcpy(mat->data, matDesc.data, matDesc.dataLen);
    }
    mMaterials.emplace(matDesc.name, mat);
}

void Scene::AddMultiObjCB(const std::string& shaderName, const std::string& name)
{
    auto itShader = mShaders.find(shaderName);
    if (itShader == mShaders.end()) throw std::exception("cannot find shader in function call(AddMultiObjCB)");
    auto shader = itShader->second;
    mMultiObjCBs.emplace
    (
        name, 
        std::make_shared<MultiObjectConstBuffer>(name, shaderName, shader->GetFreeMultiObjCBV())
    );
}

void Scene::AddPassCB(const std::string& shaderName, const std::string& passName)
{
    auto itShader = mShaders.find(shaderName);
    if (itShader == mShaders.end())
    {
        throw std::exception("cannot find shader in function call(AddPassCB)");
    }
    auto itPassCB = mPassCBs.find(passName);
    if (itPassCB != mPassCBs.end())
    {
        spdlog::get("console")->warn("passCB name {0} already exist......", passName);
    }
    auto shader = itShader->second;
    auto passCB = std::make_shared<PassConstBuffer>(passName, shaderName, shader->GetFreePassCBV());
    mPassCBs.emplace(passName, passCB);
    shader->SetPassCbvIndex(passCB->CBIndex);
}

void Scene::SetShaderPassCB(const std::string& shaderName, const std::string& passName)
{
    auto itShader = mShaders.find(shaderName);
    if (itShader == mShaders.end())
    {
        throw std::exception("cannot find shader in function call(SetShaderPassCB)");
    }
    auto itPassCB = mPassCBs.find(passName);
    if (itPassCB == mPassCBs.end())
    {
        throw std::exception("cannot find passCB in function call(SetShaderPassCB)");
    }
    auto shader = itShader->second;
    shader->SetPassCbvIndex(itPassCB->second->CBIndex);
}

void Scene::AddPass(const std::string& name)
{
    mPasses.emplace(name, std::make_shared<Pass>(name));
}

void Scene::UpdateRenderItemCBData(const std::string& name, size_t size, const void* data) {
    if (mRenderItems.find(name) == mRenderItems.end()) {
        //throw std::exception("UpdateRenderItemCBData cannot find render item");
        return;
    }
    auto& item = mRenderItems[name];
    if (item->Data == nullptr) {
        item->Data = new char[size];
        item->DataLen = size;
    }
    assert(item->DataLen == size);
    memcpy(item->Data, data, size);
    item->NumFramesDirty = Engine::NumFrameResources();

    //mRenderItems[name]->Shader->UpdateShaderCBData(mRenderItems[name]->ObjCBIndex, size, data);
}

void Scene::UpdateMaterialCBData(const std::string& name, size_t size, const void* data)
{
    auto itMat = mMaterials.find(name);
    if (itMat == mMaterials.end())
        throw std::exception("cannot find material in UpdateMaterialCBData");
    auto mat = itMat->second;
    mat->NumFramesDirty = Engine::NumFrameResources();
    if (mat->data == nullptr) {
        mat->data = new char[size];
        mat->dataLen = size;
    }
    assert(mat->dataLen >= size);
    memcpy(mat->data, data, size);
}

void Scene::UpdateMultiObjCBData(const std::string& name, size_t size, const void* data)
{
    auto itMultiObjCB = mMultiObjCBs.find(name);
    if (itMultiObjCB == mMultiObjCBs.end())
        throw std::exception("cannot find multiObjCB in UpdateMultiObjCBData function");
    auto itShader = mShaders.find(itMultiObjCB->second->shaderName);
    if (itShader == mShaders.end())
        throw std::exception("cannot find shader in UpdateMultiObjCBData function");
    auto shader = itShader->second;

    auto multiObjCbvIndex = itMultiObjCB->second->CBIndex;
    shader->UpdateMultiObjCBData(multiObjCbvIndex, size, data);
}

void Scene::UpdateShaderPassCBData(const std::string& shaderName, size_t size, const void* data) 
{
    auto itShader = mShaders.find(shaderName);
    if (itShader == mShaders.end())
    {
        throw std::exception("cannot find shader in UpdatePassCBData function");
    }
    auto shader = itShader->second;
    std::string passName = shader->GetDefaultPassCBName();
    return UpdateShaderPassCBData(shaderName, passName, size, data);
}

void Scene::UpdateShaderPassCBData(const std::string& shaderName, const std::string& passName, size_t size, const void* data)
{
    auto itPass = mPassCBs.find(passName);
    if (itPass == mPassCBs.end())
    {
        throw std::exception("cannot find pass in UpdatePassCBData function");
    }
    auto itShader = mShaders.find(shaderName);
    if (itShader == mShaders.end())
    {
        throw std::exception("cannot find shader in UpdatePassCBData function");
    }
    auto shader = itShader->second.get();
    auto passCbvIndex = itPass->second->CBIndex;
    return UpdateShaderPassCBData(shader, passCbvIndex, size, data);
}

void Scene::UpdateShaderPassCBData(D3DShaderWrapper* shader, UINT CBIndex, size_t size, const void* data)
{
    return shader->UpdatePassCBData(CBIndex, size, data);
}

void Scene::UpdateMeshCurrVBFrameRes(const std::string& name, int index, size_t size, const void* data)
{
    auto frameRes = Engine::GetEngine()->GetRenderer()->GetCurrFrameResource();
    auto itVBRes = frameRes->VBResources.find(name);
    if (itVBRes == frameRes->VBResources.end())
        throw std::exception("cannot find VB in UpdateMeshCurrVBFrameRes");
    auto VB = itVBRes->second.get();
    VB->CopyData(index, size, data);
}

void Scene::PrintAllRenderItems()
{
    std::cout << "=============================All RenderItems===================================" << std::endl;
    std::cout << "total render items:" << mRenderItems.size() << std::endl;
    for (auto& pairPriorityAndOpacityMapped : mPriorityMappedRItems)
    {
        std::cout << "Priority:" << pairPriorityAndOpacityMapped.first << std::endl;
        for (auto& itOpacityAndShaderMapped : pairPriorityAndOpacityMapped.second)
        {
            std::cout << "   Opacity:" << itOpacityAndShaderMapped.first << std::endl;
            for (auto& itShaderAndPSOMapped : itOpacityAndShaderMapped.second)
            {
                std::cout << "      Shader:" << itShaderAndPSOMapped.first << std::endl;
                for (auto& itPSOAndVecItem : itShaderAndPSOMapped.second)
                {
                    std::cout << "         PSO:" << itPSOAndVecItem.first << std::endl;
                    for (auto& item : itPSOAndVecItem.second)
                    {
                        std::cout << "            item:" << item->Name << std::endl;
                    }
                }
            }
        }
    }
    std::cout << "=============================End All RenderItems===============================" << std::endl;
}

void Scene::PrintAllPasses()
{
    std::cout << "Pass Count:" << mPasses.size() << std::endl;
    for (const auto& pass : mPasses)
    {
        std::cout << "   " << pass.second->name << std::endl;
    }
}

void Scene::PrintAllShaders() 
{
    std::cout << "Shader Count:" << mShaders.size() << std::endl;
    for (const auto& itShader : mShaders)
    {
        std::cout << "   " << itShader.first << std::endl;
    }
    std::cout << "Compute Shader Count:" << mComputeShaders.size() << std::endl;
    for (const auto& itShader : mComputeShaders)
    {
        std::cout << "   " << itShader.first << std::endl;
    }
}

void Scene::PrintAllPassCBs()
{
    std::cout << "PassCB Count:" << mPassCBs.size() << std::endl;
    for (auto itPassCB : mPassCBs)
    {
        std::cout << "   " << itPassCB.first << " shader:" << itPassCB.second->shaderName
            << " CBIndex:" << std::dec << itPassCB.second->CBIndex << std::endl;
    }
}

void Scene::PrintAllPSOs()
{
    Engine::GetEngine()->GetPSOManager2()->PrintAllPSOs();
}

void Scene::PrintAllMultiObjCBs() 
{
    std::cout << "MultiObjCB Count:" << mMultiObjCBs.size() << std::endl;
    for (auto itMultiObjCB : mMultiObjCBs)
    {
        std::cout << "   " << itMultiObjCB.first << " shader:" << itMultiObjCB.second->shaderName
            << " CBIndex:" << itMultiObjCB.second->CBIndex << std::endl;
    }
}

void Scene::PrintAllPrimitives() 
{
    std::cout << "Primitive Count:" << mPrimitives.size() << std::endl;
    for (auto itPrimitive : mPrimitives)
    {
        std::cout << "   " << itPrimitive.first << " Visible:" << itPrimitive.second->Visible() << std::endl;
        for (const auto& itSubMesh : itPrimitive.second->GetMesh()->GetSubMeshs())
        {
            std::cout << "      " << itSubMesh.first << " Index Count:" 
                << itSubMesh.second.indexCount << std::endl;
        }
    }
}

void Scene::PrintAllMaterials() 
{
    std::cout << "Material Count:" << mMaterials.size() << std::endl;
    for (auto itMat : mMaterials)
    {
        std::cout << "   " << itMat.first << " matCBIndex:" << itMat.second->MatCBIndex
            << " srvIndex:" << itMat.second->DiffuseSrvHeapIndex << std::endl;
    }
}

void Scene::PrintAllTextures() 
{
    std::cout << "D3DTexture Count:" << mTextures.size() << std::endl;
    for (auto itTex : mTextures)
    {
        std::cout << "   " << itTex.first << std::endl;
    }
}


} // end namespace