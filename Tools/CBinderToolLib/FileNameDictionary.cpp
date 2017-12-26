#include "FileNameDictionary.h"
#include <fstream>

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

FileNameDictionary::FileNameDictionary(GameVersion version) :FileNameDictionary()
{
    switch (version)
    {
    case GameVersion::DarkSouls2:
        _substitutionMap = SubstitutionMapDs2;
        break;
    case GameVersion::DarkSouls3:
        _substitutionMap = SubstitutionMapDs3;
        _physicalRoots = PhysicalRootsDs3;
        break;
    default:
        break;
    }
}

void FileNameDictionary::SaveDictionary2File(const std::string& fileName)
{
    std::ofstream out(fileName);
    if (!out) throw std::runtime_error("SaveDictionary2File open fileName failed......");
    for (const auto& itDictionary : _dictionary)
    {
        out << itDictionary.first << std::endl;
        for (const auto& itHash : itDictionary.second)
        {
            out << '\t' << itHash.first << std::endl;
            for (const auto& fileName : itHash.second)
            {
                out << "\t\t" << fileName << std::endl;
            }
        }
    }
}

bool FileNameDictionary::TryGetFileName(unsigned long hash, const std::string& archiveName, std::string& fileName)
{
    fileName.clear();
    auto itDict = _dictionary.find(archiveName);
    if (itDict != _dictionary.end())
    {
        auto itFileNames = itDict->second.find(hash);
        if (itFileNames != itDict->second.end())
        {
            const auto& fileNames = itFileNames->second;
            if (fileNames.size() > 1) return false;
            fileName = fileNames.size() ? fileNames[0] : "";
            NormalizeFileName(fileName);
            return true;
        }
    }
    return false;
}

bool FileNameDictionary::TryGetFileName
(
    unsigned long hash, 
    const std::string& archiveName, const std::string& extension, std::string& fileName
)
{
    fileName.clear();
    auto itDict = _dictionary.find(archiveName);
    if (itDict != _dictionary.end())
    {
        auto itFileNames = itDict->second.find(hash);
        if (itFileNames != itDict->second.end())
        {
            const auto& fileNames = itFileNames->second;
            fileName = fileNames.size() ? fileNames[0] : "";
            auto itFind = std::find_if
            (
                fileNames.cbegin(), fileNames.cend(),
                [&extension](const std::string& str)
            {
                return FireFlame::StringUtils::ends_with(str, extension, true);
            }
            );
            if (itFind != fileNames.cend())
                fileName = *itFind;
            NormalizeFileName(fileName);
            return true;
        }
    }
    return false;
}

bool FileNameDictionary::TrySplitFileName(const std::string& file, std::string& archiveName, std::string& fileName)
{
    archiveName.clear();
    fileName.clear();

    auto pos = file.find_first_of(":/");
    if (pos == std::string::npos)
    {
        return false;
    }

    archiveName = file.substr(0, pos);
    fileName = file.substr(pos + 2, file.size() - pos - 2);
    return true;
}

FileNameDictionary* FileNameDictionary::OpenFromFile(GameVersion version)
{
    std::string dictionaryDirectory;
    std::string dictionaryName = "Dictionary.csv";
    switch (version)
    {
    case GameVersion::DarkSouls2:
        dictionaryName = "DictionaryDS2.csv";
        break;
    case GameVersion::DarkSouls3:
        dictionaryName = "DictionaryDS3.csv";
        break;
    }
    std::string dictionaryPath = dictionaryDirectory + dictionaryName;
    return OpenFromFile(dictionaryPath, version);
}

FileNameDictionary* FileNameDictionary::OpenFromFile(const std::string& dictionaryPath, GameVersion version)
{
    auto dictionary = new FileNameDictionary(version);

    std::fstream fileIn(dictionaryPath);
    if (!fileIn) throw std::runtime_error("can not open dictionary file......");

    std::vector<std::string> lines;
    std::string line;
    while (std::getline(fileIn,line))
    {
        lines.emplace_back(std::move(line));
    }

    for (size_t i = 0; i < lines.size(); i++)
    {
        dictionary->Add(lines[i]);
    }
    return dictionary;
}

void FileNameDictionary::Add(std::string file)
{
    std::string archiveName;
    std::string fileName;
    if (!TrySplitFileName(file, archiveName, fileName))
    {
        std::cout << "file:" << file << " can not split......" << std::endl;
        return;
    }

    std::string substitutionArchiveName;
    auto itSubstitutionMap = _substitutionMap.find(archiveName);
    if (itSubstitutionMap != _substitutionMap.end())
    {
        substitutionArchiveName = itSubstitutionMap->second;
    }
    else
    {
        if (std::find(_physicalRoots.cbegin(), _physicalRoots.cend(), archiveName) == _physicalRoots.cend())
        {
            std::cout << "file:" << file << " no substituon and no physical root......" << std::endl;
            return;
        }
        substitutionArchiveName = archiveName + ":";
    }

    file = substitutionArchiveName + "/" + fileName;
    if (!TrySplitFileName(file, archiveName, fileName))
    {
        std::cout << "file:" << file << " can not split......" << std::endl;
        return;
    }

    std::string hashablePath = "/" + fileName;
    unsigned hash = GetHashCode(hashablePath);
    auto& archiveDictionary = _dictionary[archiveName];
    auto& fileNameList = archiveDictionary[hash];

    if (std::find(fileNameList.cbegin(), fileNameList.cend(), fileName) == fileNameList.cend())
    {
        fileNameList.push_back(fileName);
    }
}

unsigned FileNameDictionary::GetHashCode(const std::string& filePath, unsigned prime)
{
    if (filePath.empty() || filePath[0] == '\0')
        return 0u;
    std::string temp = filePath;
    FireFlame::StringUtils::replace(temp, '\\', '/');
    FireFlame::StringUtils::tolower(temp);

    unsigned ret = 0;
    for (size_t i = 0; i < temp.size(); i++)
    {
        ret = (unsigned)(ret*prime + temp[i]);
    }
    return ret;
}

void FileNameDictionary::NormalizeFileName(std::string& fileName)
{
    for (const auto& virtualRoot : VirtualRoots)
    {
        if (FireFlame::StringUtils::starts_with(fileName, virtualRoot, true))
        {
            fileName = fileName.substr(virtualRoot.size());
            break;
        }
    }
    FireFlame::StringUtils::replace(fileName, '/', '\\');
    FireFlame::StringUtils::trim_start(fileName, '\\');
}
}

