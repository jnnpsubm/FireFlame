#include "DSBhd5BucketEntry.h"
#include "src\Utility\io\io.h"
#include "src\FLStringUtils.h"
#include "DSBdt5FileStream.h"
#include "DecipherUtility.h"
#include "Dcx\DcxFile.h"
#include "DSUtility.h"
#include <sstream>
#include <algorithm>

namespace DSFS {
Bhd5BucketEntry::Bhd5BucketEntry(std::istream& reader, GameVersion version){
    std::int64_t saltedHashOffset = 0;
    std::int64_t aesKeyOffset = 0;
    FireFlame::IO::read_type(reader, FileNameHash);
    FireFlame::IO::read_type(reader, PaddedFileSize);
    FireFlame::IO::read_type(reader, FileOffset);
    FireFlame::IO::read_type(reader, saltedHashOffset);
    FireFlame::IO::read_type(reader, aesKeyOffset);

    switch (version){
    case GameVersion::DarkSouls3:
        FireFlame::IO::read_type(reader, FileSize);
        break;
    default:
        FileSize = PaddedFileSize;
        break;
    }

    if (saltedHashOffset != 0){
        auto currentPosition = reader.tellg();
        reader.seekg(saltedHashOffset, std::ios::beg);
        ShaHash.reset(new Bhd5SaltedShaHash(reader));
        reader.seekg(currentPosition, std::ios::beg);
    }
    if (aesKeyOffset != 0){
        auto currentPosition = reader.tellg();
        reader.seekg(aesKeyOffset, std::ios::beg);
        Aes.reset(new Bhd5Aes(reader));
        reader.seekg(currentPosition, std::ios::beg);
    }
}

std::string Bhd5BucketEntry::Decipher(Bdt5FileStream& bdtStream, unsigned bytes)
{
    std::string data = std::move(bdtStream.Read(FileOffset, (std::min)((long)PaddedFileSize,(long)bytes)));
    DecipherUtility::DecryptAesEcb(data, Aes->GetKey(), Aes->GetRanges());
    return data;
}

bool Bhd5BucketEntry::TryGetFileSize(Bdt5FileStream& bdtStream)
{
    const int sampleLength = 48;
    std::istringstream data(std::move(bdtStream.Read(FileOffset, sampleLength)));

    if (IsEncrypted())
    {
        std::string dataDeciphered = DecipherUtility::DecryptAesEcb(data, Aes->GetKey());
        data = std::move(std::istringstream(std::move(dataDeciphered)));
    }

    std::string sampleSignature(4,'\0');
    data.read(&sampleSignature[0], 4);
    if (sampleSignature != DcxFile::DcxSignature)
    {
        return false;
    }
    data.seekg(0, std::ios::beg);

    FileSize = DcxFile::DcxSize + DcxFile::ReadCompressedSize(data);
    return true;
}

std::string Bhd5BucketEntry::GetDataExtension(Bdt5FileStream& bdtStream)
{
    std::string signature(4,'\0');
    std::string extension;

    std::string data;
    if (IsEncrypted())
        data = std::move(Decipher(bdtStream, 128));
    else
        data = std::move(bdtStream.Read(FileOffset, (std::min)((long)PaddedFileSize, (long)128)));

    if (DSUtility::TryGetFileExtension(data.substr(0,4), extension))
    {
        //std::cout << "extension:" << extension << std::endl;
        return extension;
    }

    const wchar_t* wcstr = reinterpret_cast<const wchar_t*>(&data[0]);
    std::wstring wstr(wcstr, 4);
    signature = FireFlame::StringUtils::wstring2string(wstr);
    if (DSUtility::TryGetFileExtension(signature, extension))
    {
        return extension;
    }

    if (DSUtility::TryGetFileExtension(data.substr(12, 14), extension))
    {
        return extension;
    }

    std::cout << "Unknown signature" << std::endl;
    //Debug.WriteLine($"Unknown signature: '{BitConverter.ToString(Encoding.ASCII.GetBytes(signature)).Replace("-", " ")}'");
    return ".bin";
}
}