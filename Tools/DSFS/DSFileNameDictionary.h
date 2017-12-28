#pragma once
#include <vector>
#include <string>
#include <set>
#include <unordered_map>
#include "DSGameVersion.h"
#include "type.h"

namespace DSFS {
    class FileNameDictionary{
    public:
        void Init(GameVersion version);

        bool TryGetFileName(unsigned long hash, crefstr archiveName, refstr fileName);
        bool TryGetFileName(unsigned long hash,crefstr archiveName, crefstr extension, refstr fileName);
        static void NormalizeFileName(refstr fileName);

        const auto& GetDictionary() const { return _dictionary; }
        void SaveDictionary2File(crefstr fileName);

    private:
        void Init(crefstr dictionaryPath, GameVersion version);
        bool TrySplitFileName(crefstr file, refstr archiveName, refstr fileName);
        void Add(string file);

        static unsigned GetHashCode(crefstr filePath, unsigned prime = 37u);

        static const std::vector<string> VirtualRoots;
        static const std::vector<string> PhysicalRootsDs3;
        static const std::unordered_map<string, string> SubstitutionMapDs2;
        static const std::unordered_map<string, string> SubstitutionMapDs3;

        std::unordered_map<string, std::unordered_map<unsigned long, std::set<string>>> _dictionary;
        std::unordered_map<string, string> _substitutionMap;
        std::vector<string> _physicalRoots;
    };
} // end namespace

