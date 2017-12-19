#pragma once
#include <string>
#include <wrl\client.h>
#include <d3d12.h>

namespace FireFlame
{
struct D3DTextureBase
{
    D3DTextureBase(const std::string& name) : name(name) {}
    D3DTextureBase(const std::string& name, const std::wstring& filename) :
        name(name),
        filenameTex(filename)
    {}

    // Unique material name for lookup.
    std::string name;
    std::wstring filenameTex;

    virtual ID3D12Resource* Resource() = 0;

    bool needUpdate = false;
    virtual void Update(ID3D12GraphicsCommandList* cmdList) {}
};

struct D3DTexture : D3DTextureBase
{
    D3DTexture(const std::string& name) : D3DTextureBase(name) {}
    D3DTexture(const std::string& name, const std::wstring& filename) : D3DTextureBase(name,filename) {}

    ID3D12Resource* Resource() override { return resource.Get(); }
    Microsoft::WRL::ComPtr<ID3D12Resource> resource = nullptr;
    Microsoft::WRL::ComPtr<ID3D12Resource> uploadHeap = nullptr;
};
}