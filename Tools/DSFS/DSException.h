#pragma once
#include <stdarg.h>

namespace DSFS {
    // enable exception with formatted string message 
    template<typename E>
    void Throwf(const char* fmt, ...) 
    {
        static const int DEFAULT_BUFSIZE = 512;
        char buf[DEFAULT_BUFSIZE];
        va_list ap;
        va_start(ap, fmt);
        vsnprintf(buf, sizeof buf, fmt, ap);
        va_end(ap);
        throw E(std::string(buf).c_str());
    }


#define THROW_FMT(EXCEPTION, fmt, ...) \
   Throwf<EXCEPTION>("[Error] %s:%d:%s:\n" fmt "\n", __FILE__, __LINE__, __func__, ##__VA_ARGS__)
}