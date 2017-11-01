#pragma once
#include "Exception\FLException.h"

namespace FireFlame {
inline DXGI_FORMAT FLIndexFormat2DXGIFormat(Index_Format format) {
	switch (format)
	{
	case FireFlame::Index_Format::UINT16:
		return DXGI_FORMAT_R16_UINT;
	case FireFlame::Index_Format::UINT32:
		return DXGI_FORMAT_R32_UINT;
	default:
		return DXGI_FORMAT_R16_UINT;
	}
}
inline std::wstring AnsiToWString(const std::string& str)
{
	WCHAR buffer[512];
	MultiByteToWideChar(CP_ACP, 0, str.c_str(), -1, buffer, 512);
	return std::wstring(buffer);
}

#ifndef ThrowIfFailed
#define ThrowIfFailed(x)                                                    \
{                                                                           \
    HRESULT hr__ = (x);                                                     \
    std::wstring file_name = AnsiToWString(__FILE__);                       \
    if(FAILED(hr__)) { throw Exception(hr__, L#x, file_name, __LINE__); }   \
}
#endif
} // end namespace