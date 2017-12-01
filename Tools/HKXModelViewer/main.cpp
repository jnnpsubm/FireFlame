#include "FireFlameHeader.h"

int main()
{
    FireFlame::Engine engine(GetModuleHandle(NULL));

    FireFlame::HKXLoader hkxloader;
    hkxloader.load("D:\\DSIII_CHR\\c1400\\hkx\\skeleton.HKX");

    FireFlame::FLVERLoader flverloader;
    flverloader.load("D:\\DSIII_CHR\\c1400\\c1400.flver");

    system("pause");
    return 0;
}