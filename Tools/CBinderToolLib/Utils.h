#pragma once
#include "GameVersion.h"
#include "FileType.h"
#include "Bhd5BucketEntry.h"
#include "Bdt5FileStream.h"
#include "StreamEncoding.h"
#include <utility>
#include <string>
#include <memory>
#include <unordered_map>

namespace CBinderToolLib {
class Utils
{
public:
    static bool TryReadFileSize(Bhd5BucketEntry& entry, Bdt5FileStream& bdtStream, long& fileSize);
    static bool TryGetAsciiSignature(std::istream& stream, int signatureLength, std::string& signature);
    static bool TryGetUnicodeSignature(std::istream& stream, int signatureLength, std::string& signature);
    static bool TryGetSignature(std::istream& stream, Encoding encoding, int bytesPerChar, int signatureLength, std::string& signature);

    static std::pair<FileType, GameVersion> GetFileType(const std::string& fileName);
    static std::string GetFileTypeDescription(const std::pair<FileType, GameVersion>& fileType);

    static std::istream* DecryptBhdFile(const std::string& filePath, GameVersion version);

    static std::string GetDataExtension(const std::string& data);

    static std::unordered_multimap<std::string, std::string> _fileExtensions;
    static bool TryGetFileExtension(const std::string& signature, std::string& extension);
};

}

