#include "Utils.h"
#include "FireFlameHeader.h"
#include "DecryptionKeys.h"
#include <regex>
#include "CryptographyUtility.h"

namespace CBinderToolLib {
bool Utils::TryReadFileSize(Bhd5BucketEntry& entry, Bdt5FileStream& bdtStream, long& fileSize)
{
    fileSize = 0;

    const int sampleLength = 48;
    std::unique_ptr<std::istream> data = std::move(bdtStream.Read((long)entry.FileOffset, sampleLength));

    if (entry.IsEncrypted())
    {
        data = CryptographyUtility::DecryptAesEcb(*data.get(), entry.AesKey->Key);
    }

    /*string sampleSignature;
    if (!TryGetAsciiSignature(data, 4, out sampleSignature)
        || sampleSignature != DcxFile.DcxSignature)
    {
        return false;
    }

    fileSize = DcxFile.DcxSize + DcxFile.ReadCompressedSize(data);*/
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
