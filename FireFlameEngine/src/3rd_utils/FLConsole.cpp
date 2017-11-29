#include "FLConsole.h"
#include <iostream>

#include <Windows.h>

namespace FireFlame
{
void OpenConsole()
{
    AllocConsole();

    FILE *stream;
    freopen_s(&stream, "CONOUT$", "w+t", stdout);
    freopen_s(&stream, "CONIN$", "r+t", stdin);
}
}