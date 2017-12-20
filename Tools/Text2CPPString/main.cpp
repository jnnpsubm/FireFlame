#include "FireFlameHeader.h"
#include <fstream>

int main(int argc, char* argv[])
{
    using namespace FireFlame;

    cmdline::parser cmdParser;
    cmdParser.add<std::string>("input", 'i', "input file name", true);
    cmdParser.add<std::string>("output", 'o', "output file name", true);
    cmdParser.parse_check(argc, argv);
    auto inputFileName = cmdParser.get<std::string>("input");
    auto outputFileName = cmdParser.get<std::string>("output");

    std::ifstream in(inputFileName);
    std::ofstream out(outputFileName);
    if (!in)
    {
        std::cerr << "can not open input file:" << inputFileName << std::endl;
        return 0;
    }
    if (!out)
    {
        std::cerr << "can not open output file:" << outputFileName << std::endl;
        return 0;
    }

    std::string line;
    while (std::getline(in, line))
    {
        auto pos = line.find('\"');
        while (pos != std::string::npos)
        {
            line.insert(pos, "\\");
            pos = line.find('\"', pos+=2);
        }
        line.insert(line.begin(), '\"');
        line.push_back('\\');
        line.push_back('n');
        line.push_back('\"');
        std::cout << line << std::endl;
        out << line << std::endl;
    }

    system("pause");
    return 0;
}