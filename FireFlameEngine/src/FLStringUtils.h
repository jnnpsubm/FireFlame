#pragma once
#include <string>

namespace FireFlame {
namespace StringUtils {

inline
std::string wstring2string(const std::wstring &wstr)
{
    std::string str(wstr.length(), ' ');
#pragma warning(push)
#pragma warning(disable : 4244)
    //std::copy(wstr.begin(), wstr.end(), str.begin());
#pragma warning(pop)
    for (size_t i = 0; i < str.length(); i++)
    {
        str[i] = (char)wstr[i];
    }
    return str;
}

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