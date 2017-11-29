#pragma once
#include <string>
#include <wrl\client.h>
#include <d3d12.h>

namespace FireFlame
{
struct Texture
{
    Texture(const std::string& name) :
        name(name)
    {}
    Texture(const std::string& name, const std::wstring& filename) : 
        name(name),
        filenameTex(filename)
    {}

    // Unique material name for lookup.
    std::string name;

    std::wstring filenameTex;

    Microsoft::WRL::ComPtr<ID3D12Resource> resource = nullptr;
    Microsoft::WRL::ComPtr<ID3D12Resource> uploadHeap = nullptr;
};
}