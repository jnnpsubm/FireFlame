#include "DecipherUtility.h"
#include <string>
#include <fstream>
#include <sstream>
#include <assert.h>
#include <vector>
#include <algorithm>
#include "openssl_rsa_op.h"
#include <openssl/aes.h>
#include "src\Utility\io\io.h"

namespace DSFS {
std::string DecipherUtility::DecryptAesEcb(std::istream& inputStream, const std::vector<std::uint8_t>& key)
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
        AES_decrypt((std::uint8_t*)input.data() + i, (std::uint8_t*)&output[0] + i, &aeskey);
    }
    output.resize((unsigned)input_len);
    return output;
}

void DecipherUtility::DecryptAesEcb(std::string& inputStream, const std::vector<std::uint8_t>& key, const std::vector<Bhd5Range>& ranges)
{
    AES_KEY aeskey;
    AES_set_decrypt_key(key.data(), (int)key.size() * 8, &aeskey);

    for (const auto& range : ranges)
    {
        auto startOffset = range.GetStartOffset(), endOffset = range.GetEndOffset();
        if (startOffset == -1 || endOffset == -1) continue;
        if (startOffset >= (decltype(startOffset))inputStream.size()) continue;
        
        long length = (long)((std::min)(endOffset, (std::int64_t)inputStream.size()) - startOffset);
        std::string decryptedStream = DecryptAes(&inputStream[startOffset], aeskey, length);
        inputStream.replace(startOffset, length, decryptedStream);
    }
}

std::string DecipherUtility::DecryptAes(const char* inputStream, AES_KEY& aeskey, long length)
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
