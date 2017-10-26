#pragma once
#include <string>
#include <comdef.h>

namespace FireFlame {
	class Exception {
	public:
		Exception() = default;
		Exception(HRESULT hr, const std::wstring& functionName, const std::wstring& filename, int lineNumber);

		std::wstring ToString() const;

		HRESULT ErrorCode = S_OK;
		std::wstring FunctionName;
		std::wstring Filename;
		int LineNumber = -1;
	};
}