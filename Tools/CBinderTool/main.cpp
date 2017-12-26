#include <string>
#include <iostream>
#include "Options.h"
#include "FireFlameHeader.h"
#include "..\CBinderToolLib\FileNameDictionary.h"
#include "..\CBinderToolLib\Bdt5FileStream.h"
#include "..\CBinderToolLib\Utils.h"
#include "..\CBinderToolLib\Bhd5File.h"

void ShowUsage()
{
    std::string info = 
        std::string("BinderTool by Atvaark\n") +
        "  A tool for unpacking Dark Souls II/III Bdt, Bhd, Dcx, Sl2, Tpf, Param and Fmg files\n" +
        "Usage:\n" +
        "  BinderTool file_path [output_path]\n" +
        "Examples:\n" +
        "  BinderTool data1.bdt\n" +
        "  BinderTool data1.bdt data1";
    std::cout << info << std::endl;
}

void UnpackBdtFile(CBinderTool::Options* options);

void main(int argc, char* argv[])
{
    using namespace CBinderTool;

    if (argc <= 1)
    {
        ShowUsage();
    }

    std::unique_ptr<Options> options = nullptr;
    try
    {
        options = std::move(Options::Parse(argc, argv));
    }
    catch (const std::exception& e)
    {
        std::cerr << "exception raised:" << e.what() << std::endl;
    }

    switch (options->GetInputType())
    {
    // These files have a single output file. 
    case CBinderToolLib::FileType::EncryptedBhd:
    case CBinderToolLib::FileType::Bhd:
    case CBinderToolLib::FileType::Dcx:
    case CBinderToolLib::FileType::Fmg:
        break;
    default:
        FireFlame::IO::create_directory(options->GetOutputPath());
        break;
    }

    switch (options->GetInputType())
    {
    case CBinderToolLib::FileType::Regulation:
        //UnpackRegulationFile(options);
        break;
    case CBinderToolLib::FileType::Dcx:
        //UnpackDcxFile(options);
        break;
    case CBinderToolLib::FileType::EncryptedBdt:
        UnpackBdtFile(options.get());
        break;
    case CBinderToolLib::FileType::EncryptedBhd:
        //UnpackBhdFile(options);
        break;
    case CBinderToolLib::FileType::Bdt:
        //UnpackBdf4File(options);
        break;
    case CBinderToolLib::FileType::Bhd:
        //UnpackBhf4File(options);
        break;
    case CBinderToolLib::FileType::Bnd:
        //UnpackBndFile(options);
        break;
    case CBinderToolLib::FileType::Savegame:
        //UnpackSl2File(options);
        break;
    case CBinderToolLib::FileType::Tpf:
        //UnpackTpfFile(options);
        break;
    case CBinderToolLib::FileType::Param:
        //UnpackParamFile(options);
        break;
    case CBinderToolLib::FileType::Fmg:
        //UnpackFmgFile(options);
        break;
    default:
        throw std::runtime_error("Unable to handle type '{options.InputType}'");
    }

    system("pause");
}

void UnpackBdtFile(CBinderTool::Options* options)
{
    using namespace CBinderToolLib;

    std::unique_ptr<FileNameDictionary> dictionary = nullptr;
    dictionary.reset(FileNameDictionary::OpenFromFile(options->GetInputGameVersion()));
    //dictionary->SaveDictionary2File("dictionary_save.txt");
    std::string fileNameWithoutExtension = FireFlame::StringUtils::file_name(options->GetInputPath());
    FireFlame::StringUtils::replace(fileNameWithoutExtension, "Ebl.bdt", "");
    FireFlame::StringUtils::replace(fileNameWithoutExtension, ".bdt", "");
    std::string archiveName = fileNameWithoutExtension;
    FireFlame::StringUtils::tolower(archiveName);

    //using (Bdt5FileStream bdtStream = Bdt5FileStream.OpenFile(options.InputPath, FileMode.Open, FileAccess.Read))
    {
        std::unique_ptr<Bdt5FileStream> bdtStream = nullptr;
        bdtStream.reset(Bdt5FileStream::OpenFile(options->GetInputPath()));

        std::unique_ptr<std::istream> inputStream = nullptr;
        inputStream.reset(
            Utils::DecryptBhdFile
            (
                FireFlame::StringUtils::change_extension(options->GetInputPath(), "bhd"),
                options->GetInputGameVersion()
            )
        );
        auto bhdFile = Bhd5File::Read(*inputStream.get(),options->GetInputGameVersion());
        /*foreach(var bucket in bhdFile.GetBuckets())
        {
            foreach(var entry in bucket.GetEntries())
            {
                MemoryStream data;
                if (entry.FileSize == 0)
                {
                    long fileSize;
                    if (!TryReadFileSize(entry, bdtStream, out fileSize))
                    {
                        Console.WriteLine($"Unable to determine the length of file '{entry.FileNameHash:D10}'");
                        continue;
                    }

                    entry.FileSize = fileSize;
                }

                if (entry.IsEncrypted)
                {
                    data = bdtStream.Read(entry.FileOffset, entry.PaddedFileSize);
                    CryptographyUtility.DecryptAesEcb(data, entry.AesKey.Key, entry.AesKey.Ranges);
                    data.Position = 0;
                    data.SetLength(entry.FileSize);
                }
                else
                {
                    data = bdtStream.Read(entry.FileOffset, entry.FileSize);
                }

                string fileName;
                string dataExtension = GetDataExtension(data);
                bool fileNameFound = dictionary.TryGetFileName(entry.FileNameHash, archiveName, out fileName);
                if (!fileNameFound)
                {
                    fileNameFound = dictionary.TryGetFileName(entry.FileNameHash, archiveName, dataExtension, out fileName);
                }

                string extension;
                if (fileNameFound)
                {
                    extension = Path.GetExtension(fileName);

                    if (dataExtension == ".dcx" && extension != ".dcx")
                    {
                        extension = ".dcx";
                        fileName += ".dcx";
                    }
                }
                else
                {
                    extension = dataExtension;
                    fileName = $"{entry.FileNameHash:D10}_{fileNameWithoutExtension}{extension}";
                }

                if (extension == ".enc")
                {
                    byte[] decryptionKey;
                    if (DecryptionKeys.TryGetAesFileKey(Path.GetFileName(fileName), out decryptionKey))
                    {
                        EncFile encFile = EncFile.ReadEncFile(data, decryptionKey);
                        data = encFile.Data;

                        fileName = Path.Combine(Path.GetDirectoryName(fileName), Path.GetFileNameWithoutExtension(fileName));
                        extension = Path.GetExtension(fileName);
                    }
                    else
                    {
                        Debug.WriteLine($"No decryption key for file \'{fileName}\' found.");
                    }
                }

                if (extension == ".dcx")
                {
                    DcxFile dcxFile = DcxFile.Read(data);
                    data = new MemoryStream(dcxFile.Decompress());

                    fileName = Path.Combine(Path.GetDirectoryName(fileName), Path.GetFileNameWithoutExtension(fileName));

                    if (fileNameFound)
                    {
                        extension = Path.GetExtension(fileName);
                    }
                    else
                    {
                        extension = GetDataExtension(data);
                        fileName += extension;
                    }
                }

                Debug.WriteLine(
                    "{0}\t{1}\t{2}\t{3}\t{4}\t{5}\t{6}\t{7}\t{8}",
                    fileNameWithoutExtension,
                    fileName,
                    extension,
                    entry.FileNameHash,
                    entry.FileOffset,
                    entry.FileSize,
                    entry.PaddedFileSize,
                    entry.IsEncrypted,
                    fileNameFound);

                string newFileNamePath = Path.Combine(options.OutputPath, fileName);
                Directory.CreateDirectory(Path.GetDirectoryName(newFileNamePath));
                File.WriteAllBytes(newFileNamePath, data.ToArray());
            }
        }*/
    }
}