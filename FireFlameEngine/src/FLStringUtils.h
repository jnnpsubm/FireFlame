#pragma once
#include <string>

namespace FireFlame {
namespace StringUtils {

inline
void replace(std::string& str, char src, char dst)
{
    for (auto& c : str)
        if (c == src)
            c = dst;
}

inline
void replace(std::string& str, const std::string& src, char dst)
{
    for (auto& c : str)
        if (src.find(c) != std::string::npos)
            c = dst;
}

} // end StringUtils
} // end FireFlame