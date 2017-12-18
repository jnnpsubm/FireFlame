#pragma once
#include <string>
#include <functional>

namespace FireFlame {
struct CSTask
{
    CSTask
    (
        const std::string& name,
        const std::string& shaderName,
        const std::string& PSOName,
        unsigned X,
        unsigned Y,
        unsigned Z,
        std::function<void(const std::string&, void*, unsigned)> callback
    ):  name(name),
        shaderName(shaderName),
        PSOName(PSOName),
        GroupSize{ X,Y,Z },
        status(Initial), fence(0),
        callback(callback)
    {}
    virtual ~CSTask() = default;

    bool needCopyback() const {
        return callback != nullptr;
    }

    std::string name;
    std::string shaderName;
    std::string PSOName;

    struct {
        unsigned X;
        unsigned Y;
        unsigned Z;
    } GroupSize;

    enum {
        Initial = 0,
        Dispatched,
        Copyback,
        Done
    } status;
    std::uint64_t fence;

    std::function<void(const std::string&, void*, unsigned)> callback;
};
} // end FireFlame