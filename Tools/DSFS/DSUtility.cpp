#include "DSUtility.h"

namespace DSFS {
std::unordered_multimap<std::string, std::string> DSUtility::_fileExtensions =
{
    { "BND4",".bnd" },
    { "BHF4",".bhd" },
    { "BDF4",".bdt" },
    { { 'D','C','X','\0' },".dcx" },
    { "DDS ",".dds" },
    { "TAE ",".tae" },
    { "FSB5",".fsb" },
    { "fsSL",".esd" },
    { "fSSL",".esd" },
    { { 'T','P','F','\0' },".tpf" },
    { "PFBB",".pfbbin" },
    { "OBJB",".breakobj" },
    { "filt",".fltparam" }, // DS II //extension = ".gparam"; // DS III
    { "VSDF",".vsd" },
    { "NVG2",".ngp" },
    { "#BOM",".txt" },
    { "\x1BLua",".lua" }, // or .hks
    { "RIFF",".fev" },
    { "GFX\v",".gfx" },
    { { 'S','M','D','\0' },".metaparam" },
    { "SMDD",".metadebug" },
    { "CLM2",".clm2" },
    { "FLVE",".flver" },
    { "F2TR",".flver2tri" },
    { "FRTR",".tri" },
    { { 'F','X','R','\0' },".fxr" },
    { "ITLIMITER_INFO",".itl" },
    { "EVD\0",".emevd" },
    { "ENFL",".entryfilelist" },
    { "NVMA",".nvma" }, // ?
    { "MSB ",".msb" }, // ?
    { "BJBO",".bjbo" }, // ?
    { "ONAV",".onav" } // ?
};

bool DSUtility::TryGetFileExtension(const std::string& signature, std::string& extension)
{
    auto it = _fileExtensions.find(signature);
    if (it == _fileExtensions.end())
    {
        //std::cerr << "unknown file extension" << std::endl;
        extension = ".bin";
        return false;
    }
    extension = it->second;
    return true;
}
}