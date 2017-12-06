#pragma once
#include <string>
#include "..\MathHelper\FLVector.h"
#include "..\Matrix\FLMatrix4X4.h"

namespace FireFlame
{
struct Material
{
    Material(const std::string& name, int framesDirty) : Name(name), NumFramesDirty(framesDirty) {}
    ~Material()
    {
        delete[] data;
        data = nullptr;
        dataLen = 0;
    }

    // Unique material name for lookup.
    std::string Name;

    std::string ShaderName;

    // Index into constant buffer corresponding to this material.
    int MatCBIndex = -1;

    // Index into SRV heap for diffuse texture.
    int DiffuseSrvHeapIndex = -1;

    // Index into SRV heap for normal texture.
    int NormalSrvHeapIndex = -1;

    // Dirty flag indicating the material has changed and we need to update the constant buffer.
    // Because we have a material constant buffer for each FrameResource, we have to apply the
    // update to each FrameResource.  Thus, when we modify a material we should set 
    // NumFramesDirty = gNumFrameResources so that each frame resource gets the update.
    int NumFramesDirty = 0;

    // Material constant buffer data used for shading.
    size_t dataLen = 0;
    void* data = nullptr;
};
}