#pragma once
#include <iostream>

namespace FireFlame {
namespace IO {

inline void read_string(std::istream& is, std::string& str) {
    char c;
    is.read(&c, 1);
    while (c != '\0')
    {
        str.push_back(c);
        is.read(&c, 1);
    }
}

inline void read_wstring(std::istream& is, std::wstring& str) {
    wchar_t c;
    is.read((char*)&c, sizeof(wchar_t));
    while (c != L'\0')
    {
        str.push_back(c);
        is.read((char*)&c, sizeof(wchar_t));
    }
}

template<typename T>
T read_type(std::istream& is, T& val)
{
    is.read((char*)&val, sizeof(T));
    return val;
}

} // end IO
} // end FireFlame