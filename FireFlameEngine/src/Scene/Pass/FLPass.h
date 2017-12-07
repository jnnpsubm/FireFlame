#pragma once
#include <string>

namespace FireFlame
{
struct Pass
{
    Pass() = default;
    explicit Pass(const std::string& name)
        :name(name){}
    explicit Pass(const std::string& name, const std::string& shaderName, unsigned int CBIndex = 0) 
        :name(name), shaderName(shaderName), CBIndex(CBIndex) {}
    std::string name;
    std::string shaderName;
    unsigned int CBIndex = -1;
};
}