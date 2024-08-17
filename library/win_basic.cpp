#include "win_basic.h"

#include <fstream>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void set_cursor(_cursor x)
{
	if (x == g_cursor) return;
	g_cursor = x;
	SetCursor(cursors[u64(x)]);
}

void set_clipboard_text(std::wstring_view text)
{
	if (OpenClipboard(0))//открываем буфер обмена
	{
		HGLOBAL hgBuffer;
		char* chBuffer;
		EmptyClipboard(); //очищаем буфер
		size_t ll = text.size() * 2 + 2;
		hgBuffer = GlobalAlloc(GMEM_DDESHARE, ll);//выделяем память
		if (!hgBuffer) goto end;
		chBuffer = (char*)GlobalLock(hgBuffer); //блокируем память
		if (!chBuffer) goto end;
		memcpy(chBuffer, text.data(), ll);
		GlobalUnlock(hgBuffer);//разблокируем память
		SetClipboardData(CF_UNICODETEXT, hgBuffer);//помещаем текст в буфер обмена
	end:
		CloseClipboard(); //закрываем буфер обмена
	}
}

void set_clipboard_text(std::string_view text)
{
	if (OpenClipboard(0))//открываем буфер обмена
	{
		HGLOBAL hgBuffer;
		char* chBuffer;
		EmptyClipboard(); //очищаем буфер
		size_t ll = text.size() + 1;
		hgBuffer = GlobalAlloc(GMEM_DDESHARE, ll);//выделяем память
		if (!hgBuffer) goto end;
		chBuffer = (char*)GlobalLock(hgBuffer); //блокируем память
		if (!chBuffer) goto end;
		strcpy_s(chBuffer, ll, LPCSTR(text.data()));
		GlobalUnlock(hgBuffer);//разблокируем память
		SetClipboardData(CF_TEXT, hgBuffer);//помещаем текст в буфер обмена
	end:
		CloseClipboard(); //закрываем буфер обмена
	}
}

std::filesystem::path get_exe_path(HINSTANCE hinstance)
{
	wchar_t buffer[MAX_PATH];
	GetModuleFileNameW(hinstance, buffer, MAX_PATH);
	std::filesystem::path fn = buffer;
	fn.remove_filename();
	return fn;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
