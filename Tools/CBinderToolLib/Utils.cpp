#include "Utils.h"
#include "FireFlameHeader.h"
#include "DecryptionKeys.h"
#include <regex>
#include "CryptographyUtility.h"
#include "DcxFile.h"

namespace CBinderToolLib {
std::unordered_multimap<std::string, std::string> Utils::_fileExtensions =
{
    { "BND4",".bnd"},
    { "BHF4",".bhd"},
    { "BDF4",".bdt"},
    { {'D','C','X','\0'},".dcx"},
    { "DDS ",".dds"},
    { "TAE ",".tae"},
    { "FSB5",".fsb"},
    { "fsSL",".esd"},
    { "fSSL",".esd" },
    { {'T','P','F','\0'},".tpf"},
    { "PFBB",".pfbbin"},
    { "OBJB",".breakobj"},
    { "filt",".fltparam"}, // DS II //extension = ".gparam"; // DS III
    { "VSDF",".vsd"},
    { "NVG2",".ngp"},
    { "#BOM",".txt"},
    { "\x1BLua",".lua"}, // or .hks
    { "RIFF",".fev"},
    { "GFX\v",".gfx"},
    { {'S','M','D','\0'},".metaparam"},
    { "SMDD",".metadebug"},
    { "CLM2",".clm2"},
    { "FLVE",".flver"},
    { "F2TR",".flver2tri"},
    { "FRTR",".tri"},
    { {'F','X','R','\0'},".fxr"},
    { "ITLIMITER_INFO",".itl"},
    { "EVD\0",".emevd"},
    { "ENFL",".entryfilelist"},
    { "NVMA",".nvma"}, // ?
    { "MSB ",".msb"}, // ?
    { "BJBO",".bjbo"}, // ?
    { "ONAV",".onav"} // ?
};

bool Utils::TryReadFileSize(Bhd5BucketEntry& entry, Bdt5FileStream& bdtStream, long& fileSize)
{
    fileSize = 0;

    const int sampleLength = 48;
    std::unique_ptr<std::istringstream> data = std::make_unique<std::istringstream>(bdtStream.Read((long)entry.FileOffset, sampleLength));

    if (entry.IsEncrypted())
    {
        data = CryptographyUtility::DecryptAesEcb(*data.get(), entry.AesKey->Key);
    }

    std::string sampleSignature;
    if (!TryGetAsciiSignature(*data.get(), 4, sampleSignature) || sampleSignature != DcxFile::DcxSignature)
    {
        return false;
    }

    fileSize = DcxFile::DcxSize + DcxFile::ReadCompressedSize(*data.get());
    return true;
}

bool Utils::TryGetAsciiSignature(std::istream& stream, int signatureLength, std::string& signature)
{
    const int asciiBytesPerChar = 1;
    return TryGetSignature(stream, Encoding::ASCII, asciiBytesPerChar, signatureLength, signature);
}

bool Utils::TryGetUnicodeSignature(std::istream& stream, int signatureLength, std::string& signature)
{
    const int unicodeBytesPerChar = 2;
    return TryGetSignature(stream, Encoding::Unicode, unicodeBytesPerChar, signatureLength, signature);
}

bool Utils::TryGetSignature(std::istream& stream, Encoding encoding, int bytesPerChar, int signatureLength, std::string& signature)
{
    signature.clear();

    auto streamLength = FireFlame::IO::file_size(stream);
    auto startPosition = stream.tellg();
    if (streamLength - startPosition < bytesPerChar * signatureLength)
    {
        return false;
    }

    signature.resize(signatureLength);
    stream.read(&signature[0], signatureLength);
    stream.seekg(startPosition);

    return true;
}

std::string Utils::GetDataExtension(const std::string& data)
{
    std::string signature;
    std::string extension;

    if (TryGetAsciiSignature(std::istringstream(data), 4, signature) 
        && TryGetFileExtension(signature, extension))
    {
        //std::cout << "extension:" << extension << std::endl;
        return extension;
    }

    const wchar_t* wcstr = reinterpret_cast<const wchar_t*>(&data[0]);
    std::wstring wstr(wcstr, 4);
    signature = FireFlame::StringUtils::wstring2string(wstr);
    if (TryGetFileExtension(signature, extension))
    {
        return extension;
    }

    if (TryGetAsciiSignature(std::istringstream(data), 26, signature)
        && TryGetFileExtension(signature.substr(12, 14), extension))
    {
        return extension;
    }

    std::cout << "Unknown signature" << std::endl;
    //Debug.WriteLine($"Unknown signature: '{BitConverter.ToString(Encoding.ASCII.GetBytes(signature)).Replace("-", " ")}'");
    return ".bin";
}

bool Utils::TryGetFileExtension(const std::string& signature, std::string& extension)
{
    auto& it = _fileExtensions.find(signature);
    if (it == _fileExtensions.end())
    {
        //std::cerr << "unknown file extension" << std::endl;
        extension = ".bin";
        return false;
    }
    extension = it->second;
    return true;
}

std::pair<FileType, GameVersion> Utils::GetFileType(const std::string& fileName)
{
    if (fileName.empty())
    {
        throw std::runtime_error("empty file name......");
    }

    if (fileName == "Data0.bdt")
    {
        return { FileType::Regulation, GameVersion::DarkSouls3 };
    }

    if (fileName == "enc_regulation.bnd.dcx")
    {
        return { FileType::Regulation, GameVersion::DarkSouls2 };
    }

    // file.dcx file.bnd.dcx
    if (FireFlame::StringUtils::ends_with(fileName, ".dcx", true))
    {
        return { FileType::Dcx, GameVersion::Common };
    }

    // .anibnd .chrbnd .chrtpfbhd .mtdbnd .shaderbnd .objbnd .partsbnd .rumblebnd .hkxbhd .tpfbhd
    // .shaderbdle .shaderbdledebug
    if (FireFlame::StringUtils::ends_with(fileName, "bnd", true) ||
        FireFlame::StringUtils::ends_with(fileName, "bdle", true) ||
        FireFlame::StringUtils::ends_with(fileName, "bdledebug", true))
    {
        return { FileType::Bnd, GameVersion::Common };
    }

    // DS30000.sl2
    if (std::regex_match(fileName, std::regex("^DS3\\d+.*\\.sl2", std::regex::icase)))
    {
        return { FileType::Savegame, GameVersion::DarkSouls3 };
    }

    // DARKSII0000.sl2
    if (std::regex_match(fileName, std::regex("^DARKSII\\d+.*\\.sl2", std::regex::icase)))
    {
        return { FileType::Savegame, GameVersion::DarkSouls2 };
    }

    if (std::regex_match(fileName, std::regex("^(?:Data|DLC)\\d\\.bdt$", std::regex::icase)))
    {
        return { FileType::EncryptedBdt, GameVersion::DarkSouls3 };
    }

    if (std::regex_match(fileName, std::regex("^[^\\W_]+Ebl\\.bdt$", std::regex::icase)))
    {
        return { FileType::EncryptedBdt, GameVersion::DarkSouls2 };
    }

    if (std::regex_match(fileName, std::regex("^(?:Data|DLC|)\\d\\.bhd$", std::regex::icase)))
    {
        return { FileType::EncryptedBhd, GameVersion::DarkSouls3 };
    }

    if (std::regex_match(fileName, std::regex("^[^\\W_]+Ebl\\.bhd$", std::regex::icase)))
    {
        return { FileType::EncryptedBhd, GameVersion::DarkSouls2 };
    }

    // file.bdt file.hkxbdt file.tpfbdt
    if (FireFlame::StringUtils::ends_with(fileName, "bdt", true))
    {
        return { FileType::Bdt, GameVersion::Common };
    }

    // file.bhd file.hkxbhd file.tpfbhd
    if (FireFlame::StringUtils::ends_with(fileName, "bhd", true))
    {
        return { FileType::Bhd, GameVersion::Common };
    }

    if (FireFlame::StringUtils::ends_with(fileName, ".tpf", true))
    {
        return { FileType::Tpf, GameVersion::Common };
    }

    if (FireFlame::StringUtils::ends_with(fileName, ".param", true))
    {
        return { FileType::Param, GameVersion::Common };
    }

    if (FireFlame::StringUtils::ends_with(fileName, ".fmg", true))
    {
        return { FileType::Fmg, GameVersion::Common };
    }

    return { FileType::Unknown, GameVersion::Common };
}
std::string Utils::GetFileTypeDescription(const std::pair<FileType, GameVersion>& fileType)
{
    return GetGameVersionStr(fileType.second) + ":" + GetFileTypeStr(fileType.first);
}

std::istream* Utils::DecryptBhdFile(const std::string& filePath, GameVersion version)
{
    std::string fileDirectory = FireFlame::StringUtils::dir_name(filePath);
    std::string fileName = FireFlame::StringUtils::file_name(filePath);
    std::string key;
    switch (version)
    {
    case GameVersion::DarkSouls2: // todo
        /*string keyFileName = Regex.Replace(fileName, @"Ebl\.bhd$", "KeyCode.pem", RegexOptions.IgnoreCase);
            string keyFilePath = Path.Combine(fileDirectory, keyFileName);
        if (File.Exists(keyFilePath))
        {
            key = File.ReadAllText(keyFilePath);
        }*/
        break;
    case GameVersion::DarkSouls3:
        DecryptionKeys::TryGetRsaFileKey(fileName, key);
        break;
    }
    if (key.empty())
    {
        throw std::runtime_error("Missing decryption key for file \'{fileName}\'");
    }
    return (CryptographyUtility::DecryptRsa(filePath, key));
}
}
