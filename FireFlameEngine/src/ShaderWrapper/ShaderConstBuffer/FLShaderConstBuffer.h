#pragma once
#include "..\..\Matrix\FLMatrix4X4.h"

namespace FireFlame {

struct ObjectConstBuffer
{

};

struct MultiObjectConstBuffer
{
    MultiObjectConstBuffer() = default;
    explicit MultiObjectConstBuffer(const std::string& name, const std::string& shaderName, unsigned int CBIndex = 0)
        :name(name), shaderName(shaderName), CBIndex(CBIndex) {}
    std::string name;
    std::string shaderName;
    unsigned int CBIndex = -1;
};

struct PassConstBuffer
{
    PassConstBuffer() = default;
    explicit PassConstBuffer(const std::string& name, const std::string& shaderName, unsigned int CBIndex = 0)
        :name(name), shaderName(shaderName), CBIndex(CBIndex) {}
    std::string name;
    std::string shaderName;
    unsigned int CBIndex = -1;
};
} // end namespace