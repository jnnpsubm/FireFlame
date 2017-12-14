#pragma once
#include <string>
#include <functional>

namespace FireFlame {
struct CSTask
{
    CSTask
    (
        const std::string& shaderName, 
        const std::string& PSOName,
        unsigned X,
        unsigned Y,
        unsigned Z,
        const std::function<void(void*)>& callback1
    )
        : shaderName(shaderName),
        PSOName(PSOName),
        GroupSize{ X,Y,Z },
        callback1(callback1), 
        status(Initial), fence(0)
    {}
    CSTask
    (
        const std::string& shaderName,
        const std::string& PSOName,
        unsigned X,
        unsigned Y,
        unsigned Z,
        const std::function<void(void*)>& callback1,
        const std::function<void(void*, void*)>& callback2
    )
        : shaderName(shaderName),
        PSOName(PSOName),
        GroupSize{ X,Y,Z },
        callback1(callback1), callback2(callback2), 
        status(Initial), fence(0)
    {}
    std::string shaderName;
    std::string PSOName;

    struct {
        unsigned X;
        unsigned Y;
        unsigned Z;
    } GroupSize;

    std::function<void(void*)>        callback1;
    std::function<void(void*, void*)> callback2;

    enum {
        Initial = 0,
        Dispatched,
        Done
    } status;
    std::uint64_t fence;
};
} // end FireFlame