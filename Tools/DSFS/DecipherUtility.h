#pragma once
#include <sstream>
#include <memory>
#include <vector>
#include "DSBhd5Range.h"

struct aes_key_st;
typedef struct aes_key_st AES_KEY;

namespace DSFS {
class DecipherUtility{
public:
    static std::string DecryptAesEcb(std::istream& inputStream, const std::vector<std::uint8_t>& key);

    static void DecryptAesEcb(std::string& inputStream, const std::vector<std::uint8_t>& key, const std::vector<Bhd5Range>& ranges);
    static std::string DecryptAes(const char* inputStream, AES_KEY& aeskey, long length);
};
}

