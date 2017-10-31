#pragma once

namespace FireFlame {
enum class API_Feature {
	API_DX12 = 0,
	API_DX11On12,
	API_OpenGL42,
	API_OpenGLES,
	API_VULKAN,
};

enum class Index_Format {
	UINT16 = 0,
	UINT32
};
inline unsigned int IndexFormatByteLength(Index_Format format) {
	switch (format)
	{
	case Index_Format::UINT16:
		return 2;
	case Index_Format::UINT32:
		return 4;
	default:
		return 4;
	}
}

const unsigned long VERTEX_FORMAT_POS_FLOAT_3   = 1UL << 0;
const unsigned long VERTEX_FORMAT_COLOR_FLOAT_4 = 1UL << 1;
} // end namespace