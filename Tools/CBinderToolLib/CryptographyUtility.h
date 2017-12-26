#pragma once
#include <sstream>

namespace CBinderToolLib{
class CryptographyUtility
{
    /*public static MemoryStream DecryptAesEcb(Stream inputStream, byte[] key)
    {
        var cipher = CreateAesEcbCipher(key);
        return DecryptAes(inputStream, cipher, inputStream.Length);
    }

    public static MemoryStream DecryptAesCbc(Stream inputStream, byte[] key, byte[] iv)
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
    }

    private static MemoryStream DecryptAes(Stream inputStream, BufferedBlockCipher cipher, long length)
    {
        int blockSize = cipher.GetBlockSize();
        int inputLength = (int)length;
        int paddedLength = inputLength;
        if (paddedLength % blockSize > 0)
        {
            paddedLength += blockSize - paddedLength % blockSize;
        }

        byte[] input = new byte[paddedLength];
        byte[] output = new byte[cipher.GetOutputSize(paddedLength)];

        inputStream.Read(input, 0, inputLength);
        int len = cipher.ProcessBytes(input, 0, input.Length, output, 0);
        cipher.DoFinal(output, len);

        MemoryStream outputStream = new MemoryStream();
        outputStream.Write(output, 0, inputLength);
        outputStream.Seek(0, SeekOrigin.Begin);
        return outputStream;
    }*/

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
    }

    public static void DecryptAesEcb(MemoryStream inputStream, byte[] key, Bhd5Range[] ranges)
    {
        var cipher = CreateAesEcbCipher(key);

        foreach(var range in ranges)
        {
            if (range.StartOffset == -1 || range.EndOffset == -1)
            {
                continue;
            }

            inputStream.Position = range.StartOffset;
            long length = range.EndOffset - range.StartOffset;
            MemoryStream decryptedStream = DecryptAes(inputStream, cipher, length);
            inputStream.Position = range.StartOffset;
            decryptedStream.WriteTo(inputStream);
        }
    }*/
};
}
