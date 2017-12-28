#include "DSFileNameDictionary.h"
#include "DSException.h"
#include <fstream>
#include <algorithm>
#include <iostream>
#include "src\FLStringUtils.h"

namespace DSFS {
    const std::vector<string> FileNameDictionary::VirtualRoots =
    {
        "N:\\SPRJ\\data\\",
        "N:\\FDP\\data\\"
    };

    const std::vector<string> FileNameDictionary::PhysicalRootsDs3 =
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

    const std::unordered_map<string, string> FileNameDictionary::SubstitutionMapDs2 = 
    {
        {"chr", "gamedata:"},
        {"chrhq", "hqchr:"},
        {"dlc_data", "gamedata:"},
        {"dlc_menu", "gamedata:"},
        {"eventmaker", "gamedata:"},
        {"ezstate", "gamedata:"},
        {"gamedata", "gamedata:"},
        {"gamedata_patch", "gamedata:"},
        {"icon", "gamedata:"},
        {"map", "gamedata:"},
        {"maphq", "hqmap:"},
        {"menu", "gamedata:"},
        {"obj", "gamedata:"},
        {"objhq", "hqobj:"},
        {"parts", "gamedata:"},
        {"partshq", "hqparts:"},
        {"text", "gamedata:"},
    };
    const std::unordered_map<string, string> FileNameDictionary::SubstitutionMapDs3 =
    {
        {"cap_breakobj", "capture:/breakobj"},
        {"cap_dbgsaveload", "capture:/dbgsaveload"},
        {"cap_debugmenu", "capture:/debugmenu"},
        {"cap_entryfilelist", "capture:/entryfilelist"},
        {"cap_envmap", "capture:/envmap"},
        {"cap_report", "capture:/fdp_report"},
        {"cap_gparam", "capture:/gparam"},
        {"cap_havok", "capture:/havok"},
        {"cap_log", "capture:/log"},
        {"cap_mapstudio", "capture:/mapstudio"},
        {"cap_memdump", "capture:/memdump"},
        {"cap_param", "capture:/param"},
        {"cap_screenshot", "capture:/screenshot"},

        {"title", "data1:/"},
        {"event", "data1:/event"},
        {"facegen", "data1:/facegen"},
        {"font", "data1:/font"},
        {"menu", "data1:/menu"},
        {"menuesd_dlc", "data1:/menu"},
        {"menutexture", "data1:/menu"},
        {"movie", "data1:/movie"},
        {"msg", "data1:/msg"},
        {"mtd", "data1:/mtd"},
        {"other", "data1:/other"},
        {"param", "data1:/param"},
        {"gparam", "data1:/param/drawparam"},
        {"regulation", "data1:/param/regulation"},
        {"paramdef", "data1:/paramdef"},
        {"remo", "data1:/remo"},
        {"aiscript", "data1:/script"},
        {"luascriptpatch", "data1:/script"},
        {"script", "data1:/script"},
        {"talkscript", "data1:/script/talk"},
        {"patch_sfxbnd", "data1:/sfx"},
        {"sfx", "data1:/sfx"},
        {"sfxbnd", "data1:/sfx"},
        {"shader", "data1:/shader"},
        {"fmod", "data1:/sound"},
        {"sndchr", "data1:/sound"},
        {"sound", "data1:/sound"},
        {"stayparamdef", "data1:/stayparamdef"},
        {"testdata", "data1:/testdata"},

        {"parts", "data2:/parts"},

        {"action", "data3:/action"},
        {"actscript", "data3:/action/script"},
        {"chr", "data3:/chr"},
        {"chranibnd", "data3:/chr"},
        {"chranibnd_dlc", "data3:/chr"},
        {"chrbnd", "data3:/chr"},
        {"chresd", "data3:/chr"},
        {"chresdpatch", "data3:/chr"},
        {"chrtpf", "data3:/chr"},

        {"obj", "data4:/obj"},
        {"objbnd", "data4:/obj"},

        {"map", "data5:/map"},
        {"maphkx", "data5:/map"},
        {"maptpf", "data5:/map"},
        {"patch_maptpf", "data5:/map"},
        {"breakobj", "data5:/map/breakobj"},
        {"entryfilelist", "data5:/map/entryfilelist"},
        {"mapstudio", "data5:/map/mapstudio"},
        {"onav", "data5:/map/onav"},
        {"sndmap", "data5:/sound"},
        {"sndremo", "data5:/sound"},

        {"adhoc", "debugdata:/adhoc"},
    };

    void FileNameDictionary::Init(GameVersion version)
    {
        std::string dictionaryDirectory;
        std::string dictionaryName = "Dictionary.csv";
        switch (version)
        {
        case GameVersion::DarkSouls2:
            _substitutionMap = SubstitutionMapDs2;
            dictionaryName = "DictionaryDS2.csv";
            break;
        case GameVersion::DarkSouls3:
            _substitutionMap = SubstitutionMapDs3;
            _physicalRoots = PhysicalRootsDs3;
            dictionaryName = "DictionaryDS3.csv";
            break;
        default:
            break;
        }
        std::string dictionaryPath = dictionaryDirectory + dictionaryName;
        return Init(dictionaryPath, version);
    }

    void FileNameDictionary::SaveDictionary2File(crefstr fileName)
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

    bool FileNameDictionary::TryGetFileName(unsigned long hash, crefstr archiveName, refstr fileName)
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
                fileName = fileNames.size() ? *fileNames.begin() : "";
                NormalizeFileName(fileName);
                return true;
            }
        }
        return false;
    }

    bool FileNameDictionary::TryGetFileName(unsigned long hash, crefstr archiveName, crefstr extension, refstr fileName)
    {
        fileName.clear();
        auto itDict = _dictionary.find(archiveName);
        if (itDict != _dictionary.end())
        {
            auto itFileNames = itDict->second.find(hash);
            if (itFileNames != itDict->second.end())
            {
                const auto& fileNames = itFileNames->second;
                fileName = fileNames.size() ? *fileNames.begin() : "";
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

    bool FileNameDictionary::TrySplitFileName(crefstr file, refstr archiveName, refstr fileName)
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

    void FileNameDictionary::Init(const std::string& dictionaryPath, GameVersion version)
    {
        std::fstream fileIn(dictionaryPath);
        if (!fileIn) THROW_FMT(std::runtime_error, "can not open dictionary file[%s]", dictionaryPath.c_str());

        std::vector<std::string> lines;
        std::string line;
        while (std::getline(fileIn, line))
        {
            lines.emplace_back(std::move(line));
        }

        for (size_t i = 0; i < lines.size(); i++)
        {
            Add(lines[i]);
        }
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

        fileNameList.emplace(fileName);
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

