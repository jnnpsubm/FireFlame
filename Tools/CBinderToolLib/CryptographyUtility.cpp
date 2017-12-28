#include "CryptographyUtility.h"
#include <string>
#include <fstream>
#include <sstream>
#include <assert.h>
#include <vector>
#include "openssl_rsa_op.h"
#include "FireFlameHeader.h"
#include <openssl/aes.h>

namespace CBinderToolLib {
std::istringstream* CryptographyUtility::DecryptRsa(const std::string& filePath, const std::string& key)
{
    if (filePath.empty())
    {
        throw std::runtime_error("empty filePath in DecryptRsa");
    }
    if (key.empty())
    {
        throw std::runtime_error("empty key in DecryptRsa");
    }
    std::ifstream inputStream(filePath, std::ios::binary);
    if (!inputStream)
    {
        throw std::runtime_error("can not open filePath");
    }

    inputStream.seekg(0, std::ios::end);
    auto fileSize = inputStream.tellg();
    inputStream.seekg(0, std::ios::beg);
    assert(fileSize%256 == 0);

    rsa_op rsaDecipher;
    rsaDecipher.open_pubkey_from_string(key);

    std::ostringstream outputStream(std::ios::binary);
    int inputBlockSize = rsaDecipher.get_pubkey_block_len();
    std::vector<char> inputBlock;
    inputBlock.resize(inputBlockSize);
    unsigned char* outputBlock;
    int out_len = 0;
    while (inputStream.read(inputBlock.data(), 256))
    {
        int len = rsaDecipher.pubkey_decrypt((unsigned char*)inputBlock.data(), 256, &outputBlock, out_len);
        assert(len == inputBlockSize);
        // skip one as BinderTool......
        if (len > 0)
        {
            assert(outputBlock[0] == '\0');
            outputStream.write((char*)outputBlock + 1, len - 1);
        }
    }

    std::istringstream* outStream = new std::istringstream(outputStream.str(), std::ios::binary);
    return outStream;
}

CryptographyUtility::ISS_U_PTR CryptographyUtility::DecryptAesEcb(std::istream& inputStream, const std::vector<std::uint8_t>& key)
{
    AES_KEY aeskey;
    AES_set_decrypt_key(key.data(), (int)key.size() * 8, &aeskey);

    auto input_len = FireFlame::IO::file_size(inputStream);
    auto padded_len = input_len;
    if (padded_len % AES_BLOCK_SIZE)
    {
        padded_len += AES_BLOCK_SIZE - padded_len % AES_BLOCK_SIZE;
    }

    std::string input((unsigned)padded_len, 0);
    std::string output((unsigned)padded_len, 0);
    inputStream.read(&input[0], input_len);

    for (size_t i = 0; i < input.size(); i += AES_BLOCK_SIZE)
    {
        AES_decrypt((std::uint8_t*)input.data()+i, (std::uint8_t*)&output[0]+i, &aeskey);
    }
    output.resize((unsigned)input_len);
    ISS_U_PTR ret = std::make_unique<std::istringstream>(output);
    return std::move(ret);
}

void CryptographyUtility::DecryptAesEcb(std::string& inputStream, const std::vector<std::uint8_t>& key, const std::vector<Bhd5Range>& ranges)
{
    AES_KEY aeskey;
    AES_set_decrypt_key(key.data(), (int)key.size() * 8, &aeskey);

    for (const auto& range : ranges)
    {
        if (range.StartOffset == -1 || range.EndOffset == -1)
        {
            continue;
        }
        long length = range.EndOffset - range.StartOffset;
        std::string decryptedStream = DecryptAes(&inputStream[range.StartOffset], aeskey, length);
        inputStream.replace(range.StartOffset, length, decryptedStream);
    }
}

std::string CryptographyUtility::DecryptAes(const char* inputStream, AES_KEY& aeskey, long length)
{
    int paddedLength = (int)length;
    if (paddedLength % AES_BLOCK_SIZE > 0)
    {
        paddedLength += AES_BLOCK_SIZE - paddedLength % AES_BLOCK_SIZE;
    }

    std::string output(paddedLength, 0);

    for (size_t i = 0; i < output.size(); i += AES_BLOCK_SIZE)
    {
        AES_decrypt((std::uint8_t*)inputStream + i, (std::uint8_t*)&output[0] + i, &aeskey);
    }
    output.resize(length);
    return output;
}
}
