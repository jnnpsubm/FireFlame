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
};
}