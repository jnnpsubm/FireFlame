#include <string>
#include "TerrainGenerator.h"

int main(int argc, char* argv[])
{
    using namespace FireFlame;

    cmdline::parser cmdParser;
    cmdParser.add<std::string>("output", 'o', "output file name", true);
    cmdParser.add<std::string>("type", 't', "terrain mesh type", false, "grid", cmdline::oneof<std::string>("geosphere", "grid"));
    cmdParser.add<std::uint8_t>("subdivide", 'd', "terrain subdivide level", false, 8, cmdline::range(1, 32));
    cmdParser.add<std::uint32_t>("width", 'w', "terrain width", false, 4096, cmdline::range(256, 40960));
    cmdParser.add<std::uint32_t>("height", 'h', "terrain height", false, 4096, cmdline::range(256, 40960));
    cmdParser.add<std::float_t>("noise", 'n', "noise length", false, 0.f, cmdline::range(0.f, 512.f));
    cmdParser.add("abs", '\0', "use absolute noise value");
    cmdParser.parse_check(argc, argv);

    TerrainGenerator generator
    (
        cmdParser.get<std::uint32_t>("width"),
        cmdParser.get<std::uint32_t>("height"),
        cmdParser.get<std::float_t>("noise"),
        cmdParser.get<std::string>("output"),
        cmdParser.get<std::string>("type"),
        cmdParser.get<std::uint8_t>("subdivide"),
        cmdParser.exist("abs")
    );
    generator.Go();

    system("pause");
    return 0;
}