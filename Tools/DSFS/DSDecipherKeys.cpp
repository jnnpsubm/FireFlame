#include "DSDecipherKeys.h"
#include "src\FLStringUtils.h"

namespace DSFS {
    const std::vector<std::uint8_t> DecipherKeys::UserDataKeyDs3 =
    {
        0xFD, 0x46, 0x4D, 0x69, 0x5E, 0x69, 0xA3, 0x9A,
        0x10, 0xE3, 0x19, 0xA7, 0xAC, 0xE8, 0xB7, 0xFA
    };
    const std::vector<std::uint8_t> DecipherKeys::UserDataKeyDs2 =
    {
        0xB7, 0xFD, 0x46, 0x3E, 0x4A, 0x9C, 0x11, 0x02,
        0xDF, 0x17, 0x39, 0xE5, 0xF3, 0xB2, 0xA5, 0x0F
    };
    const std::vector<std::uint8_t> DecipherKeys::RegulationFileKeyDs3 =
    {
        0x64,0x73,0x33,0x23,0x6a,0x6e,0x2f,0x38,0x5f,0x37,0x28,0x72,0x73,0x59,0x39,0x70,
        0x67,0x35,0x35,0x47,0x46,0x4e,0x37,0x56,0x46,0x4c,0x23,0x2b,0x33,0x6e,0x2f,0x29
    };
    const std::vector<std::uint8_t> DecipherKeys::RegulationFileKeyDs2 =
    {
        0x40, 0x17, 0x81, 0x30, 0xDF, 0x0A, 0x94, 0x54,
        0x33, 0x09, 0xE1, 0x71, 0xEC, 0xBF, 0x25, 0x4C
    };
    const std::vector<std::uint8_t> DecipherKeys::NetworkSessionKeyDs3 =
    {
        0x64, 0x73, 0x33, 0x76, 0x76, 0x68, 0x65, 0x73, 0x30, 0x39, 0x64, 0x6a, 0x78, 0x77, 0x63, 0x6a
    };

    const std::string DecipherKeys::Data1Key =
        "-----BEGIN RSA PUBLIC KEY-----\n"
        "MIIBCwKCAQEA05hqyboW/qZaJ3GBIABFVt1X1aa0/sKINklvpkTRC+5Ytbxvp18L\n"
        "M1gN6gjTgSJiPUgdlaMbptVa66MzvilEk60aHyVVEhtFWy+HzUZ3xRQm6r/2qsK3\n"
        "8wXndgEU5JIT2jrBXZcZfYDCkUkjsGVkYqjBNKfp+c5jlnNwbieUihWTSEO+DA8n\n"
        "aaCCzZD3e7rKhDQyLCkpdsGmuqBvl02Ou7QeehbPPno78mOYs2XkP6NGqbFFGQwa\n"
        "swyyyXlQ23N15ZaFGRRR0xYjrX4LSe6OJ8Mx/Zkec0o7L28CgwCTmcD2wO8TEATE\n"
        "AUbbV+1Su9uq2+wQxgnsAp+xzhn9og9hmwIEC35bSQ==\n"
        "-----END RSA PUBLIC KEY-----";

    const std::string DecipherKeys::Data2Key =
        "-----BEGIN RSA PUBLIC KEY-----\n"
        "MIIBCwKCAQEAvCZAK9UfPdk5JaTlG7n1r0LSVzIan3h0BSLaMXQHOwO7tTGpvtdX\n"
        "m2ZLY9y8SVmOxWTQqRq14aVGLTKDyH87hPuKd47Y0E5K5erTqBbXW6AD4El1eir2\n"
        "VJz/pwHt73FVziOlAnao1A5MsAylZ9B5QJyzHJQG+LxzMzmWScyeXlQLOKudfiIG\n"
        "0qFw/xhRMLNAI+iypkzO5NKblYIySUV5Dx7649XdsZ5UIwJUhxONsKuGS+MbeTFB\n"
        "mTMehtNj5EwPxGdT4CBPAWdeyPhpoHJHCbgrtnN9akwQmpwdBBxT/sTD16Adn9B+\n"
        "TxuGDQQALed4S4KvM+fadx27pQz8pP9VLwIEL67iCQ==\n"
        "-----END RSA PUBLIC KEY-----";

    const std::string DecipherKeys::Data3Key =
        "-----BEGIN RSA PUBLIC KEY-----\n"
        "MIIBCwKCAQEAqLytWD20TSXPeAA1RGDwPW18nJwe2rBX+0HPtdzFmQc/KmQlWrP+\n"
        "94k6KClK5f7m0xUHwT8+yFGLxPdRvUPyOhBEnRA6tkObVDSxij5y0Jh4h4ilAO73\n"
        "I8VMcmscS71UKkck4444+eR4vVd+SPlzIu8VgqLefvEn/sX/pAevDp7w+gD0NgvO\n"
        "e9U6iWEXKwTOPB97X+Y2uB03gSSognmV8h2dtUFJ4Ryn5jrpWmsuUbdvGp0CWBKH\n"
        "CFruNXnfsG0hlf9LqbVmEzbFl/MhjBmbVjjtelorZsoLPK+OiPTHW5EcwwnPh1vH\n"
        "FFGM7qRMc0yvHqJnniEWDsSz8Bvg+GxpgQIEC8XNVw==\n"
        "-----END RSA PUBLIC KEY-----";

    const std::string DecipherKeys::Data4Key =
        "-----BEGIN RSA PUBLIC KEY-----\n"
        "MIIBCwKCAQEArfUaZWjYAUaZ0q+5znpX55GeyepawCZ5NnsMjIW9CA3vrOgUGRkh\n"
        "6aAU9frlafQ81LQMRgAznOnQGE7K3ChfySDpq6b47SKm4bWPqd7Ulh2DTxIgi6QP\n"
        "qm4UUJL2dkLaCnuoya/pGMOOvhT1LD/0CKo/iKwfBcYf/OAnwSnxMRC3SNRugyvF\n"
        "ylCet9DEdL5L8uBEa4sV4U288ZxZSZLg2tB10xy5SHAsm1VNP4Eqw5iJbqHEDKZW\n"
        "n2LJP5t5wpEJvV2ACiA4U5fyjQLDzRwtCKzeK7yFkKiZI95JJhU/3DnVvssjIxku\n"
        "gYZkS9D3k9m+tkNe0VVrd4mBEmqVxg+V9wIEL6Y6tw==\n"
        "-----END RSA PUBLIC KEY-----";

    const std::string DecipherKeys::Data5Key =
        "-----BEGIN RSA PUBLIC KEY-----\n"
        "MIIBCwKCAQEAvKTlU3nka4nQesRnYg1NWovCCTLhEBAnjmXwI69lFYfc4lvZsTrQ\n"
        "E0Y25PtoP0ZddA3nzflJNz1rBwAkqfBRGTeeTCAyoNp/iel3EAkid/pKOt3JEkHx\n"
        "rojRuWYSQ0EQawcBbzCfdLEjizmREepRKHIUSDWgu0HTmwSFHHeCFbpBA1h99L2X\n"
        "izH5XFTOu0UIcUmBLsK6DYsIj5QGrWaxwwXcTJN/X+/syJ/TbQK9W/TCGaGiirGM\n"
        "1u2wvZXSZ7uVM3CHwgNhAMiqLvqORygcDeNqxgq+dXDTxka43j7iPJWdHs8b25fy\n"
        "aH3kbUxKlDGaEENNNyZQcQrgz8Q76jIE0QIEFUsz9w==\n"
        "-----END RSA PUBLIC KEY-----";

    const std::string DecipherKeys::Dlc1Key =
        "-----BEGIN RSA PUBLIC KEY-----\n"
        "MIIBCwKCAQEAsCGM9dFwzaIOUIin3DXy7xrmI2otKGLZJQyKi5X3znKhSTywpcFc\n"
        "KoW6hgjeh4fJW24jhzwBosG6eAzDINm+K02pHCG8qZ/D/hIbu+ui0ENDKqrVyFhn\n"
        "QtX5/QJkVQtj8M4a0FIfdtE3wkxaKtP6IXWIy4DesSdGWONVWLfi2eq62A5ts5MF\n"
        "qMoSV3XjTYuCgXqZQ6eOE+NIBQRqpZxLNFSzbJwWXpAg2kBMkpy5+ywOByjmWzUw\n"
        "jnIFl1T17R8DpTU/93ojx+/q1p+b1o5is5KcoP7QwjOqzjHJH8bTytzRbgmRcDMW\n"
        "3ahxgI070d45TMXK2YwRzI6/JbM1P29anQIEFezyYw==\n"
        "-----END RSA PUBLIC KEY-----";

    const std::string DecipherKeys::Dlc2Key =
        "-----BEGIN RSA PUBLIC KEY-----\n"
        "MIIBCwKCAQEAtCXU9a/GBMVoqtpQox9p0/5sWPaIvDp8avLFnIBhN7vkgTwulZHi\n"
        "u64vZAiUAdVeFX4F+Qtk+5ivK488Mu2CzAMJcz5RvyMQJtOQXuDDqzIv21Tr5zuu\n"
        "sswoErHxxP8TZNxkHm7Ram7Oqtn7LQnMTYxsBgZZ34yJkRtAmZnGoCu5YaUR5euk\n"
        "8lF75idi97ssczUNV212tLzIMa1YOV7sxOb7+gc0VTIqs3pa+OXLPI/bMfwUc/KN\n"
        "jur5aLDDntQHGx5zuNtc78gMGwlmPqDhgTusKPO4VyKvoL0kITYvukoXJATaa1HI\n"
        "WVUjhLm+/uj8r8PNgolerDeS+8FM5Bpe9QIEHwCZLw==\n"
        "-----END RSA PUBLIC KEY-----";

    std::unordered_map<std::string, std::string> DecipherKeys::RsaKeyDictionary =
    {
        { "data1.bhd", Data1Key },
        { "data2.bhd", Data2Key },
        { "data3.bhd", Data3Key },
        { "data4.bhd", Data4Key },
        { "data5.bhd", Data5Key },
        { "dlc1.bhd", Dlc1Key },
        { "dlc2.bhd", Dlc2Key },
    };
    std::unordered_map<std::string, std::vector<std::uint8_t>> DecipherKeys::AesKeyDictionary =
    {
        { "regulation.regbnd.dcx.enc", RegulationFileKeyDs3 },
        { "enc_regulation.bnd.dcx", RegulationFileKeyDs2 },
    };

    bool DecipherKeys::TryGetRsaFileKey(std::string file, std::string& key)
    {
        FireFlame::StringUtils::tolower(file);
        const auto& it = RsaKeyDictionary.find(file);
        if (it == RsaKeyDictionary.end()) return false;
        key = it->second;
        return true;
    }

    bool DecipherKeys::TryGetAesFileKey(std::string file, std::vector<std::uint8_t>& key)
    {
        FireFlame::StringUtils::tolower(file);
        const auto& it = AesKeyDictionary.find(file);
        if (it == AesKeyDictionary.end()) return false;
        key = it->second;
        return true;
    }
}