#pragma once
#include <string>
#include <functional>

namespace FireFlame {
class CSTask
{
public:
    CSTask
    (
        const std::string& shaderName,
        const std::string& PSOName,
        unsigned X,
        unsigned Y,
        unsigned Z
    ):  shaderName(shaderName),
        PSOName(PSOName),
        GroupSize{ X,Y,Z },
        status(Initial), fence(0)
    {}
    virtual ~CSTask() = default;

    virtual bool needCopyback() const = 0;

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
};


template <typename CALL_BACK>
class CSTaskCB : public CSTask
{
public:
    CSTaskCB
    (
        const std::string& shaderName,
        const std::string& PSOName,
        unsigned X,
        unsigned Y,
        unsigned Z,
        CALL_BACK callback
    ) : CSTask(shaderName, PSOName, X, Y, Z), callback(callback)
    {}

    bool needCopyback() const override { return callback != nullptr; }
    CALL_BACK callback;
};
} // end FireFlame