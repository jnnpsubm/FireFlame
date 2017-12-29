#include "DSBhd5File.h"
#include "DSException.h"
#include "DSDecipherKeys.h"
#include "src\Utility\io\io.h"
#include "src\FLStringUtils.h"
#include "openssl_rsa_op.h"
#include <sstream>
#include <assert.h>

namespace DSFS {
    const std::string Bhd5File::Bhd5Signature = "BHD5";

    Bhd5File::Bhd5File(const std::string& filePath, GameVersion version) 
        : mGameVersion(version), mFilePath(filePath)
    {
        std::ifstream inFile(filePath, std::ios::binary);
        if (!inFile)
            THROW_FMT(std::runtime_error, "can not open file:%s", filePath.c_str());
        mFileStream.resize(FireFlame::IO::file_size(inFile));
        inFile.read(&mFileStream[0], mFileStream.size());
    }

    int Bhd5File::Decipher()
    {
        std::string fileName = FireFlame::StringUtils::file_name(mFilePath);
        std::string key;
        switch (mGameVersion)
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
            DecipherKeys::TryGetRsaFileKey(fileName, key);
            break;
        }
        if (key.empty())
        {
            THROW_FMT(std::runtime_error, "Missing decryption key for file:%s", mFilePath.c_str());
        }
        auto fileSize = mFileStream.size();
        assert(fileSize % 256 == 0);

        rsa_op rsaDecipher;
        rsaDecipher.open_pubkey_from_string(key);

        int inputBlockSize = rsaDecipher.get_pubkey_block_len();
        size_t index = 0;
        std::string outputStream;
        outputStream.reserve(fileSize);
        unsigned char* outputBlock;
        int out_len = 0;
        while (index < fileSize)
        {
            int len = rsaDecipher.pubkey_decrypt((unsigned char*)mFileStream.data()+index, inputBlockSize, &outputBlock, out_len);
            assert(len == inputBlockSize);
            // skip one, same as BinderTool......
            if (len > 0)
            {
                assert(outputBlock[0] == '\0');
                outputStream.append((char*)outputBlock+1, len - 1);
            }
            index += inputBlockSize;
        }

        mFileStream = std::move(outputStream);
        return 0;
    }
    
    int Bhd5File::Parse()
    {
        std::istringstream inputStream(std::move(mFileStream));

        std::string signature(4, '\0');
        inputStream.read((char*)signature.data(), 4);
        if (signature != Bhd5Signature)
            throw std::runtime_error("Invalid signature");

        int bhdVersion = 0; // 511
        int unknown = 0;    // 1
        int size = 0;       // excluding sizeof(signature)
        int bucketDirectoryEntryCount = 0;
        int bucketDirectoryOffset = 0;
        int saltLength = 0;
        FireFlame::IO::read_type
        (
            inputStream, bhdVersion, unknown, size,
            bucketDirectoryEntryCount, bucketDirectoryOffset,
            saltLength
        );

        std::string salt(saltLength, '\0');
        inputStream.read((char*)salt.data(), saltLength);
        for (int i = 0; i < bucketDirectoryEntryCount; i++)
        {
            _buckets.emplace_back(inputStream, mGameVersion);
        }
        return 0;
    }
}