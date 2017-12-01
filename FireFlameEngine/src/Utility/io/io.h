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
void skip(std::istream& is)
{
    T val;
    is.read((char*)&val, sizeof(T));
}

template<typename T>
T read_type(std::istream& is, T& val)
{
    is.read((char*)&val, sizeof(T));
    return val;
}

template<typename T1, typename T2>
void read_type(std::istream& is, T1& val1, T2& val2)
{
    is.read((char*)&val1, sizeof(T1));
    is.read((char*)&val2, sizeof(T2));
}

template<typename T1, typename T2, typename T3>
void read_type(std::istream& is, T1& val1, T2& val2, T3& val3)
{
    is.read((char*)&val1, sizeof(T1));
    is.read((char*)&val2, sizeof(T2));
    is.read((char*)&val3, sizeof(T3));
}

template<typename T1, typename T2, typename T3, typename T4>
void read_type(std::istream& is, T1& val1, T2& val2, T3& val3, T4& val4)
{
    is.read((char*)&val1, sizeof(T1));
    is.read((char*)&val2, sizeof(T2));
    is.read((char*)&val3, sizeof(T3));
    is.read((char*)&val4, sizeof(T4));
}

template<typename T1, typename T2, typename T3, typename T4, typename T5>
void read_type(std::istream& is, T1& val1, T2& val2, T3& val3, T4& val4, T5& val5)
{
    is.read((char*)&val1, sizeof(T1));
    is.read((char*)&val2, sizeof(T2));
    is.read((char*)&val3, sizeof(T3));
    is.read((char*)&val4, sizeof(T4));
    is.read((char*)&val5, sizeof(T5));
}

template<typename T1, typename T2, typename T3, typename T4, typename T5, typename T6>
void read_type(std::istream& is, T1& val1, T2& val2, T3& val3, T4& val4, T5& val5, T6& val6)
{
    is.read((char*)&val1, sizeof(T1));
    is.read((char*)&val2, sizeof(T2));
    is.read((char*)&val3, sizeof(T3));
    is.read((char*)&val4, sizeof(T4));
    is.read((char*)&val5, sizeof(T5));
    is.read((char*)&val6, sizeof(T6));
}

template<typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7>
void read_type(std::istream& is, T1& val1, T2& val2, T3& val3, T4& val4, T5& val5, T6& val6, T7& val7)
{
    is.read((char*)&val1, sizeof(T1));
    is.read((char*)&val2, sizeof(T2));
    is.read((char*)&val3, sizeof(T3));
    is.read((char*)&val4, sizeof(T4));
    is.read((char*)&val5, sizeof(T5));
    is.read((char*)&val6, sizeof(T6));
    is.read((char*)&val7, sizeof(T7));
}

} // end IO
} // end FireFlame