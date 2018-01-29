#pragma once
#include <string>

namespace FireFlame
{
struct Pass
{
    Pass() = default;
    explicit Pass(const std::string& name)
        :name(name){}
    std::string name;

    Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> CbvHeap = nullptr;
    int                                          CbvIndex = -1;
    int                                          MSAAMode = -1;
};
}