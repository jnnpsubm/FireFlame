#pragma once
#include "..\MathHelper\FLVector.h"

namespace FireFlame
{
struct Light
{
    Vector3f Strength = { 0.5f, 0.5f, 0.5f };
    float FalloffStart = 1.0f;                          // point/spot light only
    Vector3f Direction = { 0.0f, -1.0f, 0.0f };         // directional/spot light only
    float FalloffEnd = 10.0f;                           // point/spot light only
    Vector3f Position = { 0.0f, 0.0f, 0.0f };           // point/spot light only
    float SpotPower = 64.0f;                            // spot light only

    static const unsigned int MaxLights = 16;
};
}