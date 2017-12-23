#include "FileNameDictionary.h"

namespace CBinderToolLib {
const std::array<std::string, 2> FileNameDictionary::VirtualRoots =
{
    "N:\\SPRJ\\data\\",
    "N:\\FDP\\data\\"
};

const std::vector<std::string> FileNameDictionary::PhysicalRootsDs3 = 
{
    "capture",
    "data1",
    "data2",
    "data3",
    "data4",
    "data5",
    "system",
    "temp",
    "config",
    "debug",
    "debugdata",
    "dbgai",
    "parampatch",

    "chrhkx",
    "chrflver",
    "tpfbnd",
    "hkxbnd",
};

std::unordered_map<std::string, std::string> FileNameDictionary::SubstitutionMapDs2;
std::unordered_map<std::string, std::string> FileNameDictionary::SubstitutionMapDs3;

FileNameDictionary::FileNameDictionary()
{
    if (SubstitutionMapDs2.empty())
    {
        SubstitutionMapDs2.emplace("chr", "gamedata:");
        SubstitutionMapDs2.emplace("chrhq", "hqchr:");
        SubstitutionMapDs2.emplace("dlc_data", "gamedata:");
        SubstitutionMapDs2.emplace("dlc_menu", "gamedata:");
        SubstitutionMapDs2.emplace("eventmaker", "gamedata:");
        SubstitutionMapDs2.emplace("ezstate", "gamedata:");
        SubstitutionMapDs2.emplace("gamedata", "gamedata:");
        SubstitutionMapDs2.emplace("gamedata_patch", "gamedata:");
        SubstitutionMapDs2.emplace("icon", "gamedata:");
        SubstitutionMapDs2.emplace("map", "gamedata:");
        SubstitutionMapDs2.emplace("maphq", "hqmap:");
        SubstitutionMapDs2.emplace("menu", "gamedata:");
        SubstitutionMapDs2.emplace("obj", "gamedata:");
        SubstitutionMapDs2.emplace("objhq", "hqobj:");
        SubstitutionMapDs2.emplace("parts", "gamedata:");
        SubstitutionMapDs2.emplace("partshq", "hqparts:");
        SubstitutionMapDs2.emplace("text", "gamedata:");
    }
    if (SubstitutionMapDs3.empty())
    {
        SubstitutionMapDs3.emplace("cap_breakobj", "capture:/breakobj");
        SubstitutionMapDs3.emplace("cap_dbgsaveload", "capture:/dbgsaveload");
        SubstitutionMapDs3.emplace("cap_debugmenu", "capture:/debugmenu");
        SubstitutionMapDs3.emplace("cap_entryfilelist", "capture:/entryfilelist");
        SubstitutionMapDs3.emplace("cap_envmap", "capture:/envmap");
        SubstitutionMapDs3.emplace("cap_report", "capture:/fdp_report");
        SubstitutionMapDs3.emplace("cap_gparam", "capture:/gparam");
        SubstitutionMapDs3.emplace("cap_havok", "capture:/havok");
        SubstitutionMapDs3.emplace("cap_log", "capture:/log");
        SubstitutionMapDs3.emplace("cap_mapstudio", "capture:/mapstudio");
        SubstitutionMapDs3.emplace("cap_memdump", "capture:/memdump");
        SubstitutionMapDs3.emplace("cap_param", "capture:/param");
        SubstitutionMapDs3.emplace("cap_screenshot", "capture:/screenshot");

        SubstitutionMapDs3.emplace("title", "data1:/");
        SubstitutionMapDs3.emplace("event", "data1:/event");
        SubstitutionMapDs3.emplace("facegen", "data1:/facegen");
        SubstitutionMapDs3.emplace("font", "data1:/font");
        SubstitutionMapDs3.emplace("menu", "data1:/menu");
        SubstitutionMapDs3.emplace("menuesd_dlc", "data1:/menu");
        SubstitutionMapDs3.emplace("menutexture", "data1:/menu");
        SubstitutionMapDs3.emplace("movie", "data1:/movie");
        SubstitutionMapDs3.emplace("msg", "data1:/msg");
        SubstitutionMapDs3.emplace("mtd", "data1:/mtd");
        SubstitutionMapDs3.emplace("other", "data1:/other");
        SubstitutionMapDs3.emplace("param", "data1:/param");
        SubstitutionMapDs3.emplace("gparam", "data1:/param/drawparam");
        SubstitutionMapDs3.emplace("regulation", "data1:/param/regulation");
        SubstitutionMapDs3.emplace("paramdef", "data1:/paramdef");
        SubstitutionMapDs3.emplace("remo", "data1:/remo");
        SubstitutionMapDs3.emplace("aiscript", "data1:/script");
        SubstitutionMapDs3.emplace("luascriptpatch", "data1:/script");
        SubstitutionMapDs3.emplace("script", "data1:/script");
        SubstitutionMapDs3.emplace("talkscript", "data1:/script/talk");
        SubstitutionMapDs3.emplace("patch_sfxbnd", "data1:/sfx");
        SubstitutionMapDs3.emplace("sfx", "data1:/sfx");
        SubstitutionMapDs3.emplace("sfxbnd", "data1:/sfx");
        SubstitutionMapDs3.emplace("shader", "data1:/shader");
        SubstitutionMapDs3.emplace("fmod", "data1:/sound");
        SubstitutionMapDs3.emplace("sndchr", "data1:/sound");
        SubstitutionMapDs3.emplace("sound", "data1:/sound");
        SubstitutionMapDs3.emplace("stayparamdef", "data1:/stayparamdef");
        SubstitutionMapDs3.emplace("testdata", "data1:/testdata");

        SubstitutionMapDs3.emplace("parts", "data2:/parts");

        SubstitutionMapDs3.emplace("action", "data3:/action");
        SubstitutionMapDs3.emplace("actscript", "data3:/action/script");
        SubstitutionMapDs3.emplace("chr", "data3:/chr");
        SubstitutionMapDs3.emplace("chranibnd", "data3:/chr");
        SubstitutionMapDs3.emplace("chranibnd_dlc", "data3:/chr");
        SubstitutionMapDs3.emplace("chrbnd", "data3:/chr");
        SubstitutionMapDs3.emplace("chresd", "data3:/chr");
        SubstitutionMapDs3.emplace("chresdpatch", "data3:/chr");
        SubstitutionMapDs3.emplace("chrtpf", "data3:/chr");

        SubstitutionMapDs3.emplace("obj", "data4:/obj");
        SubstitutionMapDs3.emplace("objbnd", "data4:/obj");

        SubstitutionMapDs3.emplace("map", "data5:/map");
        SubstitutionMapDs3.emplace("maphkx", "data5:/map");
        SubstitutionMapDs3.emplace("maptpf", "data5:/map");
        SubstitutionMapDs3.emplace("patch_maptpf", "data5:/map");
        SubstitutionMapDs3.emplace("breakobj", "data5:/map/breakobj");
        SubstitutionMapDs3.emplace("entryfilelist", "data5:/map/entryfilelist");
        SubstitutionMapDs3.emplace("mapstudio", "data5:/map/mapstudio");
        SubstitutionMapDs3.emplace("onav", "data5:/map/onav");
        SubstitutionMapDs3.emplace("sndmap", "data5:/sound");
        SubstitutionMapDs3.emplace("sndremo", "data5:/sound");

        SubstitutionMapDs3.emplace("adhoc", "debugdata:/adhoc");
    }
}
}

