#pragma once

#include "mbasic.h"

#if TARGET_SYSTEM_WINDOWS
	#define NOMINMAX
	#include <atlbase.h>
	#include <atlconv.h>
	#include <windows.h>
	#include <windowsx.h>

namespace sysg
{
	inline HINSTANCE hInstance;
	inline HINSTANCE hPrevInstance;
	inline LPSTR     lpCmdLine;
	inline int       nCmdShow;
} // namespace sysg

#endif
