#pragma once
#include <chrono>
#include <ctime>
#include <string>
#include "..\FLStringUtils.h"

namespace FireFlame {

inline std::string NowAsFileName()
{
    std::chrono::system_clock::time_point tp;
    tp = std::chrono::system_clock::now();
    std::time_t t = std::chrono::system_clock::to_time_t(tp);
#ifdef _MSC_VER
    #pragma warning(suppress : 4996)
    std::string ts = (std::ctime)(&t);
#else
    std::string ts = (std::ctime)(&t);
#endif
    StringUtils::replace(ts, " :", '_');
    ts.resize(ts.size() - 1);
    return ts;
}

} // end FireFlame