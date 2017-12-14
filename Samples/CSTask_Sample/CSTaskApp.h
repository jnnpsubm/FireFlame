#pragma once
#include "..\Common\FLEngineApp3.h"
#include "..\Common\Waves.h"
#include <stack>

class CSTaskApp : public FLEngineApp3 {
public:
    CSTaskApp(FireFlame::Engine& e) : FLEngineApp3(e)
    { 
        
    }
    void PreInitialize() override;
    void Initialize() override;

    void Update(float time_elapsed) override {};
    void UpdateMainPassCB(float time_elapsed) override {};
    void OnKeyUp(WPARAM wParam, LPARAM lParam) override;

private:
    void AddShaders();
    void AddShaderVectorLen();

    void AddPSOs();
};