#pragma once
#include <string>
#include <array>
#include <unordered_map>
#include "GameVersion.h"
#include "FireFlameHeader.h"

namespace CBinderToolLib {
class FileNameDictionary
{
public:
    FileNameDictionary();
    FileNameDictionary(GameVersion version);

    void SaveDictionary2File(const std::string& fileName);

private:
    static const std::array<std::string, 2> VirtualRoots;
    static const std::vector<std::string> PhysicalRootsDs3;

    static std::unordered_map<std::string, std::string> SubstitutionMapDs2;

    /// <example>
    ///     1. gparam:/m_template.gparam.dcx
    ///     2. data1:/param/drawparam/m_template.gparam.dcx
    ///     3. /param/drawparam/m_template.gparam.dcx
    /// </example>
    static std::unordered_map<std::string, std::string> SubstitutionMapDs3;

	std::unordered_map<std::string, std::unordered_map<unsigned long, std::vector<std::string>>> _dictionary;
	std::unordered_map<std::string, std::string> _substitutionMap;
    std::vector<std::string> _physicalRoots;

public:
    bool TryGetFileName(unsigned long hash, const std::string& archiveName, std::string& fileName);
    bool TryGetFileName
    (
        unsigned long hash,
        const std::string& archiveName, const std::string& extension, std::string& fileName
    );

private:
    bool TrySplitFileName(const std::string& file, std::string& archiveName, std::string& fileName);
    void Add(std::string file);

public:
    static FileNameDictionary* OpenFromFile(GameVersion version);
    static FileNameDictionary* OpenFromFile(const std::string& dictionaryPath, GameVersion version);

    static unsigned GetHashCode(const std::string& filePath, unsigned prime = 37u);
    
    static void NormalizeFileName(std::string& fileName);
};
} // end namespace

