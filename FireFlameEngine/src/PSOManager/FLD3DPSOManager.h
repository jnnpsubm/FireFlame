#ifndef _FL_D3D_PSO_MANAGER_H_
#define _FL_D3D_PSO_MANAGER_H_

#include "FLPSOManager.h"
#include <vector>
#include <map>
#include <tuple>
#include <d3d12.h>
#include <wrl.h>

namespace FireFlame {
class D3DPSOManager : public PSOManager {
private:
    typedef std::tuple
    <
        UINT, 
        D3D12_PRIMITIVE_TOPOLOGY_TYPE, 
        D3D12_CULL_MODE,
        D3D12_FILL_MODE
    > PSO_TRAIT;
    typedef Microsoft::WRL::ComPtr<ID3D12PipelineState> PSO_ComPtr;

public:
    ID3D12PipelineState*  GetPSO
    (
        UINT MSAAMode, 
        D3D12_PRIMITIVE_TOPOLOGY_TYPE ptype,
        D3D12_CULL_MODE cull = D3D12_CULL_MODE_BACK,
        D3D12_FILL_MODE fill = D3D12_FILL_MODE_SOLID
    ) const
    {
        auto it = mPSOs.find({ MSAAMode, ptype,cull,fill });
        if (it != mPSOs.end()) return it->second.Get();
        return nullptr;
    }
    bool AddPSO
    (
        UINT          MSAAMode,
        const D3D12_GRAPHICS_PIPELINE_STATE_DESC& desc
    );

private:
    std::map<PSO_TRAIT, PSO_ComPtr> mPSOs;
};
}


#endif // !_FL_D3D_PSO_MANAGER_H_
