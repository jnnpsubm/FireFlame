#include <string>
#include <iostream>
#include "FireFlameHeader.h"

void main(int argc, char* argv[])
{
    using namespace FireFlame;

    cmdline::parser cmdParser;
    cmdParser.add<std::string>("input", 'i', "M2 file name", true);
    cmdParser.parse_check(argc, argv);

    std::string filename = cmdParser.get<std::string>("input");
    std::cout << "M2:" << filename << std::endl;

    system("pause");
}