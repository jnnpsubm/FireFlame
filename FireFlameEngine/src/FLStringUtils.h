#pragma once
#include <string>
#include <algorithm>
#include <cctype>

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
void tolower(std::string& str)
{
    std::for_each(str.begin(), str.end(), [](char& c) {c = std::tolower(c); });
}

inline
bool starts_with(const std::string& str, const std::string& starts, bool bIgnoreCase)
{
    if (str.size() < starts.size()) return false;

    auto str1 = str.substr(0, starts.size());
    for (size_t i = 0; i < starts.size(); i++)
    {
        auto c1 = bIgnoreCase ? std::tolower(str1[i]) : str1[i];
        auto c2 = bIgnoreCase ? std::tolower(starts[i]) : starts[i];
        if (c1 != c2) return false;
    }
    return true;
}

inline
bool ends_with(const std::string& str, const std::string& ends, bool bIgnoreCase)
{
    if (str.size() < ends.size()) return false;

    auto str1 = str.substr(str.size() - ends.size());
    for (size_t i = 0; i < ends.size(); i++)
    {
        auto c1 = bIgnoreCase ? std::tolower(str1[i]) : str1[i];
        auto c2 = bIgnoreCase ? std::tolower(ends[i]) : ends[i];
        if (c1 != c2) return false;
    }
    return true;
}

inline
std::string combine_path(const std::string& pathParent, const std::string& pathChild)
{
    return pathParent + "\\" + pathChild;
}

inline 
std::string dir_name(const std::string& path)
{
    auto pos1 = path.rfind('\\');
    auto pos2 = path.rfind('/');
    if (pos1 == std::string::npos && pos2 == std::string::npos) return "";
    return path.substr(0, (std::min)(pos1, pos2));
}

inline
std::string change_extension(const std::string& path, const std::string& extension)
{
    std::string pathOut = path;
    auto posDot = pathOut.rfind('.');
    size_t len = path.size() - posDot - 1;
    return pathOut.replace(posDot+1, len, extension);
}

inline
std::string file_name(const std::string& path)
{
    auto pos1 = path.rfind('\\');
    auto pos2 = path.rfind('/');
    if (pos1 == std::string::npos && pos2 == std::string::npos) return "";
    return path.substr((std::min)(pos1, pos2)+1);
}

inline
std::string file_extension(const std::string& path)
{
    auto pos = path.rfind('.');
    if (pos == std::string::npos) return "";
    return path.substr(pos);
}

inline
std::string file_name_noext(const std::string& path)
{
    auto posDot = path.rfind('.');
    auto posSplit = (std::min)(path.rfind('\\'), path.rfind('/'));
    //if (posSplit == std::string::npos) posSplit = 0;
    //if (posSplit > posDot) return "";
    auto count = posDot - posSplit - 1;
    return path.substr(posSplit + 1, (std::max)((decltype(count))0, count));
}

inline
void trim_start(std::string& str, const char start)
{
    size_t pos = 0;
    for (size_t i = 0; i < str.size(); i++)
    {
        if (str[i] == start)
            pos = i;
        else
            break;
    }
    str = str.substr(pos);
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

inline
void replace(std::string& str, const std::string& src, const std::string& dst)
{
    std::string::size_type pos = 0;
    std::string::size_type srcLen = src.size();
    std::string::size_type desLen = dst.size();
    pos = str.find(src, pos);
    while ((pos != std::string::npos))
    {
        str.replace(pos, srcLen, dst);
        pos = str.find(src, (pos + desLen));
    }
}

} // end StringUtils
} // end FireFlame