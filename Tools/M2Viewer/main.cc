#include <string>
#include <iostream>
#include "FireFlameHeader.h"
#include "..\M2Lib\src\M2.h"

void main(int argc, char* argv[])
{
    using namespace FireFlame;

    cmdline::parser cmdParser;
    cmdParser.add<std::string>("input", 'i', "M2 file name", true);
    cmdParser.parse_check(argc, argv);

    std::string filename = cmdParser.get<std::string>("input");
    std::cout << "M2:" << filename << std::endl;

    std::wstring wfilename = AnsiToWString(filename);
    std::unique_ptr<M2Lib::M2> M2File = std::make_unique<M2Lib::M2>();
    M2File->Load(wfilename.c_str());

    system("pause");
}