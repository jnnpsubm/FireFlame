/*
From PBRT V3
*/
#pragma once
#include "..\MathHelper\FLVector.h"

namespace FireFlame
{
class Noise
{
public:
    typedef float Float;
    //typedef double Float;

public:
    static void Permutate(unsigned seed);

    static Float EvaluatePoint(Vector3f p) { return Evaluate(p.x, p.y, p.z); }
    static Float Evaluate(Float x, Float y, Float z);
    static Float EvaluatePointAbs(Vector3f p)
    {
        return std::abs(EvaluatePoint(p));
    }
    static Float EvaluateAbs(Float x, Float y, Float z)
    {
        return std::abs(Evaluate(x, y, z));
    }
    static Float FBm
    (
        const Vector3f &p, 
        const Vector3f &dpdx, const Vector3f &dpdy,
        Float omega, int maxOctaves
    );
    static Float FBm
    (
        const Vector3f &p,
        Float omega, int maxOctaves
    );

    static Float Turbulence
    (
        const Vector3f &p,
        const Vector3f &dpdx, const Vector3f &dpdy,
        Float omega, int maxOctaves
    );
    static Float Turbulence
    (
        const Vector3f &p,
        Float omega, int maxOctaves
    );
};
}
