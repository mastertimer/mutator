#include "graphics.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool _bitmap::resize(_isize wh)
{
	wh = wh.correct();
	if (size == wh) return false;
	size = wh;
	area = size;
	BITMAPINFO bmi = { sizeof(BITMAPINFOHEADER), (long)size.x, (long)-size.y, 1, 32, BI_RGB, 0, 0, 0, 0, 0 };
	bitmap2 = CreateDIBSection(0, &bmi, DIB_RGB_COLORS, (void**)(&data), 0, 0);
	HGDIOBJ old = (bitmap2 != 0) ? SelectObject(hdc, bitmap2) : 0;
	if (old != 0) DeleteObject(old);
	return true;
}

_bitmap::_bitmap(int rx3, int ry3) : _picture({ rx3, ry3 })
{
	font.lfHeight         = 13;              // высота шрифта или символа
	font.lfWidth          = 0;               // средняя ширина символов в шрифте
	font.lfEscapement     = 0;               // угол, между вектором наклона и осью X устройства
	font.lfOrientation    = 0;               // угол, между основной линией каждого символа и осью X устройства
	font.lfWeight         = 100;             // толщина шрифта в диапазоне от 0 до 1000
	font.lfItalic         = 0;               // курсивный шрифт
	font.lfUnderline      = 0;               // подчеркнутый шрифт
	font.lfStrikeOut      = 0;               // зачеркнутый шрифт
	font.lfCharSet        = RUSSIAN_CHARSET; // набор символов
	font.lfOutPrecision   = 0;               // точность вывода
	font.lfClipPrecision  = 0;               // точность отсечения
	font.lfQuality        = 0;               // качество вывода
	font.lfPitchAndFamily = 0;               // ширина символов и семейство шрифта
	memcpy(font.lfFaceName, L"Tahoma", 14);  // название шрифта

	BITMAPINFO bmi = { sizeof(BITMAPINFOHEADER), rx3, -ry3, 1, 32, BI_RGB, 0, 0, 0, 0, 0 };
	bitmap2 = CreateDIBSection(0, &bmi, DIB_RGB_COLORS, (void**)(&data), 0, 0);
	hdc = CreateCompatibleDC(0);
	if (hdc == 0) return;
	if (bitmap2 != 0) SelectObject(hdc, bitmap2);
	f_c = GetTextColor(hdc);
	f_cf = GetBkColor(hdc);
}

_bitmap::~_bitmap()
{
	DeleteDC(hdc);
	DeleteObject(bitmap2);
	if (hfont != 0) DeleteObject(hfont);
	data = 0; // чтобы ~Picture не выругался
}

void _bitmap::text(_ixy p, std::wstring_view s, int h, uint c, uint bg)
{
	auto text_area = size_text(s, h).move(p) & area;
	if (text_area.empty()) return;
	podg_font(h);
	podg_cc(c, bg);
	TextOutW(hdc, p.x, p.y, s.data(), (int)s.size());
}

void _bitmap::text(_ixy p, std::string_view s, int h, uint c, uint bg)
{
	auto text_area = size_text(s, h).move(p) & area;
	if (text_area.empty()) return;
	podg_font(h);
	podg_cc(c, bg);
	TextOutA(hdc, p.x, p.y, s.data(), (int)s.size());
}

_isize _bitmap::size_text(std::wstring_view s, int h)
{
	podg_font(h);
	SIZE a;
	GetTextExtentPoint32W(hdc, s.data(), (int)s.size(), &a);
	return { a.cx, a.cy };
}

_isize _bitmap::size_text(std::string_view s, int h)
{
	podg_font(h);
	SIZE a;
	GetTextExtentPoint32A(hdc, s.data(), (int)s.size(), &a);
	return { a.cx, a.cy };
}

void _bitmap::podg_font(int r)
{
	if (r != font.lfHeight)
	{
		font.lfHeight = r;
		izm_font = true;
	}
	if (izm_font)
	{
		izm_font = false;
		hfont = CreateFontIndirect(&font);
		HGDIOBJ old = SelectObject(hdc, hfont);
		DeleteObject(old);
	}
}

void _bitmap::podg_cc(uint c, uint cf)
{
	c = ((c & 0xff0000) >> 16) + (c & 0xff00) + ((c & 0xff) << 16);
	cf = ((cf & 0xff0000) >> 16) + (cf & 0xff00ff00) + ((cf & 0xff) << 16);
	if (c != f_c) SetTextColor(hdc, f_c = c);
	if (cf != f_cf)
	{
		if (cf >> 24 == 0xff) {
			SetBkColor(hdc, cf & 0xffffff);
			if (f_cf >> 24 != 0xff) SetBkMode(hdc, OPAQUE);
		}
		else SetBkMode(hdc, TRANSPARENT);
		f_cf = cf;
	}
}

wstr _bitmap::get_font_name()
{
	return font.lfFaceName;
}

void _bitmap::set_font(wstr name, bool bold)
{
	izm_font = true;
	font.lfWeight = (bold) ? 700 : 100;
	wcscpy_s(font.lfFaceName, name);
}

void _bitmap::grab_ecran_oo2(HWND hwnd)
{
	RECT rr;
	GetClientRect(hwnd, &rr);
	resize({ (i64)rr.right - rr.left, (i64)rr.bottom - rr.top });
	HDC X = GetDC(hwnd);
	BitBlt(hdc, 0, 0, rr.right - rr.left, rr.bottom - rr.top, X, rr.left, rr.top, SRCCOPY);
	ReleaseDC(hwnd, X);
	// исправление альфа канала
	unsigned long long* ee = (unsigned long long*)data;
	unsigned long long* eemax = (unsigned long long*) & (data[size.x * size.y - 1]);
	while (ee < eemax)*ee++ |= 0xff000000ff000000;
	if (ee == eemax)*((unsigned int*)ee) |= 0xff000000;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

