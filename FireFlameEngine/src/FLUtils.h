#pragma once
#include <string>

namespace FireFlame
{

inline std::string ShaderMacros2String
(
    const std::string& vsMacro,
    const std::string& psMacro,
    const std::string& tsMacro = "",
    const std::string& gsMacro = ""
) 
{
    return vsMacro + psMacro + tsMacro + gsMacro;
}

} // end FireFlame