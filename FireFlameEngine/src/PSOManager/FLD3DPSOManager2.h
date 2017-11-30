#ifndef _FL_D3D_PSO_MANAGER2_H_
#define _FL_D3D_PSO_MANAGER2_H_

#include "FLPSOManager.h"
#include <vector>
#include <string>
#include <map>
#include <set>
#include <tuple>
#include <d3d12.h>
#include <wrl.h>
#include "..\FLTypeDefs.h"

namespace FireFlame {
class D3DRenderer;
class D3DPSOManager2 : public PSOManager {
private:
    typedef std::tuple
    <
        std::string,       // PSO name
        UINT,              // MSAAMode
        D3D12_FILL_MODE
    > PSO_TRAIT;
    typedef Microsoft::WRL::ComPtr<ID3D12PipelineState> PSO_ComPtr;

public:
    void AddPSO(const std::string& name, const PSODesc& desc);
    ID3D12PipelineState*  GetPSO
    (
        const std::string& name,
        UINT MSAAMode,
        D3D12_FILL_MODE fill = D3D12_FILL_MODE_SOLID
    ) const
    {
        auto it = mPSOs.find({ name, MSAAMode,fill });
        if (it != mPSOs.end()) return it->second.Get();
        return nullptr;
    }
    
    bool NameExist(const std::string& name) const { return mNames.find(name) != mNames.end(); }

private:
    std::set<std::string>           mNames;
    std::map<PSO_TRAIT, PSO_ComPtr> mPSOs;
};
}


#endif // !_FL_D3D_PSO_MANAGER2_H_
