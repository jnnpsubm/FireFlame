#pragma once
#include <sstream>
#include <memory>
#include <vector>
#include "Bhd5Range.h"

struct aes_key_st;
typedef struct aes_key_st AES_KEY;

namespace CBinderToolLib{
class CryptographyUtility
{
    typedef std::unique_ptr<std::istringstream> ISS_U_PTR;

public:
    static ISS_U_PTR DecryptAesEcb(std::istream& inputStream, const std::vector<std::uint8_t>& key);

    /*public static MemoryStream DecryptAesCbc(Stream inputStream, byte[] key, byte[] iv)
    {
        AesEngine engine = new AesEngine();
        KeyParameter keyParameter = new KeyParameter(key);
        ICipherParameters parameters = new ParametersWithIV(keyParameter, iv);

        BufferedBlockCipher cipher = new BufferedBlockCipher(new CbcBlockCipher(engine));
        cipher.Init(false, parameters);
        return DecryptAes(inputStream, cipher, inputStream.Length);
    }

    public static MemoryStream DecryptAesCtr(Stream inputStream, byte[] key, byte[] iv)
    {
        AesEngine engine = new AesEngine();
        KeyParameter keyParameter = new KeyParameter(key);
        ICipherParameters parameters = new ParametersWithIV(keyParameter, iv);

        BufferedBlockCipher cipher = new BufferedBlockCipher(new SicBlockCipher(engine));
        cipher.Init(false, parameters);
        return DecryptAes(inputStream, cipher, inputStream.Length);
    }

    private static BufferedBlockCipher CreateAesEcbCipher(byte[] key)
    {
        AesEngine engine = new AesEngine();
        KeyParameter parameter = new KeyParameter(key);
        BufferedBlockCipher cipher = new BufferedBlockCipher(engine);
        cipher.Init(false, parameter);
        return cipher;
    }*/

private:
    static std::string DecryptAes(const char* inputStream, AES_KEY& aeskey, long length);

    /// <summary>
    ///     Decrypts a file with a provided decryption key.
    /// </summary>
    /// <param name="filePath">An encrypted file</param>
    /// <param name="key">The RSA key in PEM format</param>
    /// <exception cref="ArgumentNullException">When the argument filePath is null</exception>
    /// <exception cref="ArgumentNullException">When the argument keyPath is null</exception>
    /// <returns>A memory stream with the decrypted file</returns>
public:
    static std::istringstream* DecryptRsa(const std::string& filePath, const std::string& key);

    /*public static AsymmetricKeyParameter GetKeyOrDefault(string key)
    {
        try
        {
            PemReader pemReader = new PemReader(new StringReader(key));
            return (AsymmetricKeyParameter)pemReader.ReadObject();
        }
        catch
        {
            return null;
        }
    }*/

    static void DecryptAesEcb(std::string& inputStream, const std::vector<std::uint8_t>& key, const std::vector<Bhd5Range>& ranges);
};
}
