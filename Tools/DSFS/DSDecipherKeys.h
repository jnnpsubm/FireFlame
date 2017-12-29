#pragma once
#include <unordered_map>
#include <memory>
#include <vector>

namespace DSFS {
    class DecipherKeys{
    public:
        static bool TryGetRsaFileKey(std::string file, std::string& key);
        static bool TryGetAesFileKey(std::string file, std::vector<std::uint8_t>& key);

    private:
        static std::unordered_map<std::string, std::string>               RsaKeyDictionary;
        static std::unordered_map<std::string, std::vector<std::uint8_t>> AesKeyDictionary;

        /// <summary>
        /// The key can be read by setting a breakpoint on the "create_serialCipherKey" method of the SerialKeyGeneratorSPI class. 
        /// Signature: SerialCipherKey *__fastcall SerialKeyGeneratorSPI::create_serialCipherKey(
        ///     SerialKeyGeneratorSPI *this,
        ///     const void *pKeyType,
        ///     const void *pKey,
        ///     unsigned int keylen,
        ///     const void *pHeapAllocator)
        /// 
        /// These are the DarkSoulsIII.exe 1.3.1.0 offsets:
        /// Address (vtable): 0000000142AECBB8 (DLCR::DLSerialKeyGeneratorSPI::vftable + 0x08) 
        /// Address (method): 0000000141790180 
        /// </summary>
        static const std::vector<std::uint8_t> UserDataKeyDs3;
        static const std::vector<std::uint8_t> UserDataKeyDs2;

        /// <summary>
        /// <see cref="UserDataKeyDs3"/>
        /// </summary>
        static const std::vector<std::uint8_t> RegulationFileKeyDs3;
        static const std::vector<std::uint8_t> RegulationFileKeyDs2;

        /// <summary>
        /// <see cref="UserDataKeyDs3"/>
        /// </summary>
        static const std::vector<std::uint8_t> NetworkSessionKeyDs3;

        static const std::string Data1Key;
        static const std::string Data2Key;
        static const std::string Data3Key;
        static const std::string Data4Key;
        static const std::string Data5Key;
        static const std::string Dlc1Key;
        static const std::string Dlc2Key;
    };
} // end namespace

