#include <string>
#include <iostream>
#include "Options.h"

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

    system("pause");
}