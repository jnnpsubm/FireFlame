#include "CryptographyUtility.h"
#include <string>
#include <fstream>
#include <sstream>
#include <assert.h>
#include <vector>
#include <openssl/aes.h>
#include "openssl_rsa_op.h"

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

CryptographyUtility::ISS_U_PTR CryptographyUtility::DecryptAesEcb(std::istream& inputStream, std::vector<std::uint8_t>& key)
{
    AES_KEY aeskey;
    AES_set_decrypt_key(key.data(), key.size() * 8, &aeskey);

    return nullptr;
}
}
