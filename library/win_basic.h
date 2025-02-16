﻿#pragma once

#include <SDKDDKVer.h>
#define WIN32_LEAN_AND_MEAN // Исключите редко используемые компоненты из заголовков Windows
#define NOMINMAX
#include <windows.h>

#include "geometry.h"

struct _keyboard
{
	bool alt_key = false;
	bool ctrl_key = false;
	bool shift_key = false;
};

struct _mouse
{
	bool left_button = false;
	bool right_button = false;
	bool middle_button = false;
	_xy position{ 0, 0 };
	_xy prev_position{ 0, 0 };
};

inline _keyboard keyboard;
inline _mouse mouse;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void set_clipboard_text(std::string_view text);
void set_clipboard_text(std::wstring_view text);

std::filesystem::path get_exe_path(HINSTANCE hinstance);

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

enum class _cursor { normal, size_all, hand_point, size_we, size_ns, drag, vcursor }; // виды курсора

inline HCURSOR cursors[u64(_cursor::vcursor)] = { LoadCursor(0, IDC_ARROW), LoadCursor(0, IDC_SIZEALL),
	LoadCursor(0, IDC_HAND), LoadCursor(0, IDC_SIZEWE), LoadCursor(0, IDC_SIZENS), LoadCursor(0, IDC_UPARROW) };

inline _cursor g_cursor = _cursor::normal; // установленный курсор

void set_cursor(_cursor x);

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
