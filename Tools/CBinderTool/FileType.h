#pragma once

namespace CBinderTool {
enum class FileType:std::uint8_t
{
    Unknown,
    Regulation,
    Dcx,
    EncryptedBdt,
    EncryptedBhd,
    Bdt,
    Bhd,
    Bnd,
    Savegame,
    Tpf,
    Param,
    Fmg
};
}

