#pragma once
#include <string>
#include <array>
#include <unordered_map>
#include "GameVersion.h"

namespace CBinderToolLib {
class FileNameDictionary
{
public:
    FileNameDictionary();

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
	FileNameDictionary(GameVersion version):FileNameDictionary()
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

    bool TryGetFileName(unsigned long hash, std::string archiveName, std::string& fileName)
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
				fileName = NormalizeFileName(fileNames.size() ? fileNames[0] : "");
				return true;
			}
		}
		return false;
    }

    //public bool TryGetFileName(ulong hash, string archiveName, string extension, out string fileName)
    //{
    //    fileName = "";
    //    Dictionary<ulong, List<string>> archiveDictionary;
    //    if (_dictionary.TryGetValue(archiveName, out archiveDictionary))
    //    {
    //        List<string> fileNames;
    //        if (archiveDictionary.TryGetValue(hash, out fileNames))
    //        {
    //            //if (fileNames.Count > 1)
    //            //{
    //            //    Debug.WriteLine($"Hashcollision: {hash}\t{archiveName}\t{fileNames.Count}\t{string.Join("\t", fileNames)}");
    //            //}

    //            fileName = fileNames.FirstOrDefault(e = > e.EndsWith(extension)) ? ? fileNames.First();
    //            fileName = NormalizeFileName(fileName);
    //            return true;
    //        }
    //    }

    //    return false;
    //}

    //private bool TrySplitFileName(string file, out string archiveName, out string fileName)
    //{
    //    archiveName = null;
    //    fileName = null;

    //    int i = file.IndexOf(":/", StringComparison.Ordinal);
    //    if (i == -1)
    //    {
    //        return false;
    //    }

    //    archiveName = file.Substring(0, i);
    //    fileName = file.Substring(i + 2, file.Length - i - 2);

    //    return true;
    //}

    //private void Add(string file)
    //{
    //    string archiveName;
    //    string fileName;
    //    if (!TrySplitFileName(file, out archiveName, out fileName))
    //    {
    //        return;
    //    }

    //    string substitutionArchiveName;
    //    if (!_substitutionMap.TryGetValue(archiveName, out substitutionArchiveName))
    //    {
    //        if (!_physicalRoots.Contains(archiveName))
    //        {
    //            return;
    //        }

    //        substitutionArchiveName = archiveName + ":";
    //    }

    //    file = substitutionArchiveName + "/" + fileName;
    //    if (!TrySplitFileName(file, out archiveName, out fileName))
    //    {
    //        return;
    //    }

    //    string hashablePath = "/" + fileName;
    //    uint hash = GetHashCode(hashablePath);

    //    Dictionary<ulong, List<string>> archiveDictionary;
    //    if (_dictionary.TryGetValue(archiveName, out archiveDictionary) == false)
    //    {
    //        archiveDictionary = new Dictionary<ulong, List<string>>();
    //        _dictionary.Add(archiveName, archiveDictionary);
    //    }

    //    List<string> fileNameList;
    //    if (archiveDictionary.TryGetValue(hash, out fileNameList) == false)
    //    {
    //        fileNameList = new List<string>();
    //        archiveDictionary.Add(hash, fileNameList);
    //    }

    //    if (fileNameList.Contains(fileName) == false)
    //    {
    //        fileNameList.Add(fileName);
    //    }
    //}

    //public static FileNameDictionary OpenFromFile(GameVersion version)
    //{
    //    string dictionaryDirectory = Path.GetDirectoryName(Assembly.GetEntryAssembly().Location) ? ? string.Empty;
    //    string dictionaryName = "Dictionary.csv";
    //    switch (version)
    //    {
    //    case GameVersion.DarkSouls2:
    //        dictionaryName = "DictionaryDS2.csv";
    //        break;
    //    case GameVersion.DarkSouls3:
    //        dictionaryName = "DictionaryDS3.csv";
    //        break;
    //    }
    //    string dictionaryPath = Path.Combine(dictionaryDirectory, dictionaryName);
    //    return OpenFromFile(dictionaryPath, version);
    //}

    //public static FileNameDictionary OpenFromFile(string dictionaryPath, GameVersion version)
    //{
    //    var dictionary = new FileNameDictionary(version);

    //    string[] lines = File.ReadAllLines(dictionaryPath);
    //    foreach(string line in lines)
    //    {
    //        dictionary.Add(line);
    //    }

    //    return dictionary;
    //}

    //private static uint GetHashCode(string filePath, uint prime = 37u)
    //{
    //    if (string.IsNullOrEmpty(filePath))
    //        return 0u;
    //    return filePath.Replace('\\', '/')
    //        .ToLowerInvariant()
    //        .Aggregate(0u, (i, c) = > i * prime + c);
    //}

    static std::string NormalizeFileName(const std::string& fileName)
    {
		for (const auto& virtualRoot : VirtualRoots)
		{

		}
        foreach(var virtualRoot in VirtualRoots)
        {
            if (fileName.StartsWith(virtualRoot))
            {
                fileName = fileName.Substring(virtualRoot.Length);
                break;
            }
        }

        return fileName.Replace('/', '\\').TrimStart('\\');
    }
};
} // end namespace

