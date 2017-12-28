#include <string>
#include <iostream>
#include <iomanip>
#include "Options.h"
#include "FireFlameHeader.h"
#include "..\CBinderToolLib\FileNameDictionary.h"
#include "..\CBinderToolLib\Bdt5FileStream.h"
#include "..\CBinderToolLib\Utils.h"
#include "..\CBinderToolLib\Bhd5File.h"
#include "..\CBinderToolLib\CryptographyUtility.h"
#include "..\CBinderToolLib\DcxFile.h"

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
        for (auto& bucket : bhdFile->GetBuckets())
        {
            for(auto& entry : bucket.GetEntries())
            {
                if (entry.FileSize == 0)
                {
                    long fileSize;
                    if (!Utils::TryReadFileSize(entry, *bdtStream.get(), fileSize))
                    {
                        std::cerr << "Unable to determine the length of file, hash:" << entry.FileNameHash << std::endl;
                        continue;
                    }
                    entry.FileSize = fileSize;
                }
                //std::cout << "entry file size:" << entry.FileSize << std::endl;

                std::string data;
                if (entry.IsEncrypted())
                {
                    data = std::move(bdtStream->Read((long)entry.FileOffset, (long)entry.PaddedFileSize));
                    CryptographyUtility::DecryptAesEcb(data, entry.AesKey->Key, entry.AesKey->Ranges);
                    data.resize((unsigned)entry.FileSize);
                }
                else
                {
                    data = bdtStream->Read((long)entry.FileOffset, (long)entry.FileSize);
                }

                std::string fileName;
                std::string dataExtension = Utils::GetDataExtension(data.substr(0,128));
                bool fileNameFound = dictionary->TryGetFileName(entry.FileNameHash, archiveName, fileName);
                if (!fileNameFound)
                {
                    fileNameFound = dictionary->TryGetFileName(entry.FileNameHash, archiveName, dataExtension, fileName);
                }

                std::string extension;
                if (fileNameFound)
                {
                    extension = FireFlame::StringUtils::file_extension(fileName);

                    if (dataExtension == ".dcx" && extension != ".dcx")
                    {
                        extension = ".dcx";
                        fileName += ".dcx";
                    }
                }
                else
                {
                    extension = dataExtension;
                    std::ostringstream oss;
                    oss << std::setw(10) << std::setfill('0') << std::right << entry.FileNameHash << '_' << fileNameWithoutExtension << extension;
                    fileName = oss.str();
                    //std::cout << fileName << std::endl;
                    //fileName = $"{entry.FileNameHash:D10}_{fileNameWithoutExtension}{extension}";
                }

                if (extension == ".enc")
                {
                    throw std::runtime_error("unimplemented......");
                    /*byte[] decryptionKey;
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
                    }*/
                }

                if (extension == ".dcx")
                {
                    DcxFile dcxFile;
                    dcxFile.Read(data);
                    data = std::move(dcxFile.Decompress());

                    
                    fileName = FireFlame::StringUtils::combine_path
                    (
                        FireFlame::StringUtils::dir_name(fileName), 
                        FireFlame::StringUtils::file_name_noext(fileName)
                    );

                    if (fileNameFound)
                    {
                        extension = FireFlame::StringUtils::file_extension(fileName);
                    }
                    else
                    {
                        extension = Utils::GetDataExtension(data.substr(0,128));
                        fileName += extension;
                    }
                }

                std::ostringstream oss;
                oss << fileNameWithoutExtension << '\t'
                    << fileName << '\t'
                    << extension << '\t'
                    << entry.FileNameHash << '\t'
                    << entry.FileOffset << '\t'
                    << entry.FileSize << '\t'
                    << entry.PaddedFileSize << '\t'
                    << entry.IsEncrypted() << '\t'
                    << fileNameFound << std::endl;
                OutputDebugStringA(oss.str().c_str());
                std::cout << oss.str() << std::endl;

                //if (fileNameFound) continue;
                std::string newFileNamePath 
                    = FireFlame::StringUtils::combine_path(options->GetOutputPath(), fileName);
                FireFlame::IO::create_directory(FireFlame::StringUtils::dir_name(newFileNamePath));
                std::ofstream outFile(newFileNamePath, std::ios::binary);
                outFile.write(data.data(), data.size());
            }
        }
    }
}