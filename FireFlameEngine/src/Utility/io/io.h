#pragma once
#include <iostream>
#ifdef WIN32
#include <io.h>
#include <direct.h> 
#else
#include <unistd.h>
#include <sys/stat.h>
#endif

#define MAX_PATH_LEN 512

#ifdef WIN32
#define ACCESS(fileName,accessMode) _access(fileName,accessMode)
#define MKDIR(path) _mkdir(path)
#else
#define ACCESS(fileName,accessMode) access(fileName,accessMode)
#define MKDIR(path) mkdir(path,S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH)
#endif

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

template <typename T, typename... Args>
void read_type(std::istream& is, T& val, Args&... rest)
{
    read_type(is, val);
    read_type(is, rest...);
}

static int32_t create_directory(const std::string &directoryPathIn)
{
    std::string directoryPath = directoryPathIn;
    if (directoryPath[directoryPath.size()-1] != '\\' || directoryPath[directoryPath.size() - 1] != '/')
    {
        directoryPath.push_back('\\');
    }
    uint32_t dirPathLen = (uint32_t)directoryPath.length();
    if (dirPathLen > MAX_PATH_LEN)
    {
        return -1;
    }
    char tmpDirPath[MAX_PATH_LEN] = { 0 };
    for (uint32_t i = 0; i < dirPathLen; ++i)
    {
        tmpDirPath[i] = directoryPath[i];
        if (tmpDirPath[i] == '\\' || tmpDirPath[i] == '/')
        {
            if (ACCESS(tmpDirPath, 0) != 0)
            {
                int32_t ret = MKDIR(tmpDirPath);
                if (ret != 0)
                {
                    return ret;
                }
            }
        }
    }
    return 0;
}

} // end IO
} // end FireFlame