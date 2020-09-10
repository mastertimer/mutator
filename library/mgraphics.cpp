#include "mgraphics.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

_bitmap temp_bmp(10, 10);

#define px(xx,yy) ((u1*)&data[(yy) * size.x + (xx)])

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

_bitmap::_bitmap(int rx3, int ry3) : _picture(rx3, ry3)
{
	font.lfHeight = 13; // высота шрифта или символа
	font.lfWidth = 0; // средняя ширина символов в шрифте
	font.lfEscapement = 0; // угол, между вектором наклона и осью X устройства
	font.lfOrientation = 0; // угол, между основной линией каждого символа и осью X устройства
	font.lfWeight = 100; // толщина шрифта в диапазоне от 0 до 1000
	font.lfItalic = 0; // курсивный шрифт
	font.lfUnderline = 0; // подчеркнутый шрифт
	font.lfStrikeOut = 0; // зачеркнутый шрифт
	font.lfCharSet = RUSSIAN_CHARSET; // набор символов
	font.lfOutPrecision = 0; // точность вывода
	font.lfClipPrecision = 0; // точность отсечения
	font.lfQuality = 0; // качество вывода
	font.lfPitchAndFamily = 0; // ширина символов и семейство шрифта
	memcpy(font.lfFaceName, L"Tahoma", 14); // название шрифта

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

void _bitmap::text(int x, int y, std::wstring_view s, int h, uint c, uint bg)
{ 
	podg_font(h);
	podg_cc(c, bg);
	TextOutW(hdc, x, y, s.data(), (int)s.size());
}

void _bitmap::text(int x, int y, std::string_view s, int h, uint c, uint bg)
{ 
	podg_font(h);
	podg_cc(c, bg);
	TextOutA(hdc, x, y, s.data(), (int)s.size());
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
			SetBkColor(hdc, cf);
			if (f_cf >> 24 != 0xff) SetBkMode(hdc, OPAQUE);
		}
		else SetBkMode(hdc, TRANSPARENT);
		f_cf = cf;
	}
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
	resize({ (int64)rr.right - rr.left, (int64)rr.bottom - rr.top });
	HDC X = GetDC(hwnd);
	BitBlt(hdc, 0, 0, rr.right - rr.left, rr.bottom - rr.top, X, rr.left, rr.top, SRCCOPY);
	ReleaseDC(hwnd, X);
	// исправление альфа канала
	unsigned long long* ee = (unsigned long long*)data;
	unsigned long long* eemax = (unsigned long long*) & (data[size.x * size.y - 1]);
	while (ee < eemax)* ee++ |= 0xff000000ff000000;
	if (ee == eemax)* ((unsigned int*)ee) |= 0xff000000;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

_picture::_picture(int rx3, int ry3) : data(nullptr), size{ rx3, ry3 }
{
	if (!size.empty()) data = new uint[size.square()];
	area = size;
}

_picture::_picture(_picture&& move) noexcept :
   data(move.data), size(move.size), transparent(move.transparent), area(move.area)
{
	move.data = nullptr;
}

void _picture::operator=(const _picture& move) noexcept
{
	resize(move.size);
	transparent = move.transparent;
	memcpy(data, move.data, size.square() * 4);
	area = size;
}

_picture& _picture::operator=(_picture&& move) noexcept
{
	if (&move == this) return *this;
	delete[] data;
	data        = move.data;
	size        = move.size;
	area        = move.area;
	transparent = move.transparent;
	move.data   = nullptr;
	return *this;
}

void _picture::invert_alpha()
{
	i8 r = size.square();
	for (i8 i = 0; i < r; i++)
	{
		uint a = 255 - (data[i] >> 24);
		data[i] = (data[i] & 0xffffff) + (a << 24);
	}
}

void _picture::set_transparent()
{
	int64 r = size.square();
	for (int64 i = 0; i < r; i++)
		if ((data[i] & 0xff000000) != 0xff000000)
		{
			transparent = true;
			return;
		}
	transparent = false;
}

void _picture::draw(_ixy r, _picture &bm)
{
	_iarea b = move(bm.size, r) & area;
	if (b.empty()) return;
	if (!bm.transparent)
	{
		for (i8 j = b.y.min; j < b.y.max; j++)
			memcpy(px(b.x.min, j), (bm.data + ((j - r.y) * bm.size.x + (b.x.min - r.x))), (b.x.max - b.x.min) * 4);
		return;
	}
	for (i8 j = b.y.min; j < b.y.max; j++)
	{
		uchar* s1_ = px(b.x.min, j);
		uchar* s2_ = (uchar*)(bm.data + ((j - r.y) * bm.size.x + (b.x.min - r.x)));
		for (i8 i = b.x.min; i < b.x.max; i++)
		{
			uint pp2 = s2_[3];
			uint pp1 = 256 - pp2;
			s1_[0] = (s1_[0] * pp1 + s2_[0] * pp2) >> 8;
			s1_[1] = (s1_[1] * pp1 + s2_[1] * pp2) >> 8;
			s1_[2] = (s1_[2] * pp1 + s2_[2] * pp2) >> 8;
			s1_ += 4;
			s2_ += 4;
		}
	}
}

bool _picture::resize(_isize wh)
{
	wh = wh.correct();
	if (size == wh) return false;
	size = wh;
	delete[] data;
	data = (size.x * size.y) ? (new uint[size.x * size.y]) : 0;
	area = size;
	return true;
}

void _picture::clear(uint c)
{
	transparent = ((c >> 24) != 0xff);
	fill_rectangle(size, c, true);
}

void _picture::line(_ixy p1, _ixy p2, uint c, bool rep)
{
	uint kk = 255 - (c >> 24);
	if ((kk == 0xFF) && (!rep)) return; // полностью прозрачная
	if (kk == 0) rep = true;
	if (p1.y == p2.y) // горизонтальная линия
	{
		if (p1.x > p2.x) std::swap(p1.x, p2.x);
		if (p1.x < area.x.min) p1.x = area.x.min;
		if (p2.x >= area.x.max) p2.x = area.x.max - 1;
		if ((p1.x > p2.x) || (p1.y < area.y.min) || (p1.y >= area.y.max)) return; // за пределы
		if (rep)
		{
			uint64  cc    = (((uint64)c) << 32) + c;
			uint64* ee    = (uint64*)&(data[p1.y * size.x + p1.x]);
			uint64* eemax = (uint64*)&(data[p1.y * size.x + p2.x]);
			while (ee < eemax) *ee++ = cc;
			if (ee == eemax) *((uint*)ee) = c;
			return;
		}
		uint   k2 = 256 - kk;
		uint   d1 = (c & 255) * k2;
		uint   d2 = ((c >> 8) & 255) * k2;
		uint   d3 = ((c >> 16) & 255) * k2;
		uchar* c2 = (uchar*)&(data[p1.y * size.x + p1.x]);
		for (int64 d = p1.x - p2.x; d <= 0; d++)
		{
			c2[0] = (c2[0] * kk + d1) >> 8;
			c2[1] = (c2[1] * kk + d2) >> 8;
			c2[2] = (c2[2] * kk + d3) >> 8;
			c2 += 4;
		}
		return;
	}
	if (p1.x == p2.x) // вертикальная линия
	{
		if (p1.y > p2.y) std::swap(p1.y, p2.y);
		if (p1.y < area.y.min) p1.y = area.y.min;
		if (p2.y >= area.y.max) p2.y = area.y.max - 1;
		if ((p1.y > p2.y) || (p1.x < area.x.min) || (p1.x >= area.x.max)) return; // за пределы
		if (rep)
		{
			uint* c2 = &data[p1.y * size.x + p1.x];
			for (int64 y = p1.y - p2.y; y <= 0; y++)
			{
				*c2 = c;
				c2 += size.x;
			}
			return;
		}
		uint   k2  = 256 - kk;
		uint   d1  = (c & 255) * k2;
		uint   d2  = ((c >> 8) & 255) * k2;
		uint   d3  = ((c >> 16) & 255) * k2;
		int64    dc2 = size.x * 4;
		uchar* c2  = (uchar*)&(data[p1.y * size.x + p1.x]);
		for (int64 y = p1.y - p2.y; y <= 0; y++)
		{
			c2[0] = (c2[0] * kk + d1) >> 8;
			c2[1] = (c2[1] * kk + d2) >> 8;
			c2[2] = (c2[2] * kk + d3) >> 8;
			c2 += dc2;
		}
		return;
	}
	int64 d = (abs(p1.x - p2.x) >= abs(p1.y - p2.y)) ? abs(p1.x - p2.x) : abs(p1.y - p2.y);
	int64 dx = ((p2.x - p1.x) << 32) / d;
	int64 dy = ((p2.y - p1.y) << 32) / d;
	int64 x  = ((p1.x * 2 + 1) << 31) - dx; // ?? +1 для отрицательных p1.x, p1.y ??
	int64 y  = ((p1.y * 2 + 1) << 31) - dy;
	int64 n  = 0;
	int64 k  = d;
	if (p1.x < area.x.min)
	{
		if (dx <= 0) return;
		n = (int64)((((area.x.min - 0) << 32) - x) / dx);
	}
	else if (p1.x >= area.x.max)
	{
		if (dx >= 0) return;
		n = (int)((((area.x.max - 0) << 32) - x) / dx);
	}
	if (p2.x < area.x.min)
	{
		if (dx >= 0) return;
		k = (int)((((area.x.min - 0) << 32) - x) / dx - 1);
	}
	else if (p2.x >= area.x.max)
	{
		if (dx <= 0) return;
		k = (int)((((area.x.max - 1) << 32) - x) / dx);
	}
	if (p1.y < area.y.min)
	{
		if (dy <= 0) return;
		int n2 = (int)((((area.y.min - 0) << 32) - y) / dy);
		if (n2 > n) n = n2;
	}
	else if (p1.y >= area.y.max)
	{
		if (dy >= 0) return;
		int n2 = (int)((((area.y.max - 0) << 32) - y) / dy);
		if (n2 > n) n = n2;
	}
	if (p2.y < area.y.min)
	{
		if (dy >= 0) return;
		int k2 = (int)((((area.y.min - 0) << 32) - y) / dy - 1);
		if (k2 < k) k = k2;
	}
	else if (p2.y >= area.y.max)
	{
		if (dy <= 0) return;
		int k2 = (int)((((area.y.max - 1) << 32) - y) / dy);
		if (k2 < k) k = k2;
	}
	x += dx * n;
	y += dy * n;
	if (rep)
	{
		for (int64 i = k - n; i >= 0; i--) data[((y += dy) >> 32) * size.x + ((x += dx) >> 32)] = c;
		return;
	}
	uint k2 = 256 - kk;
	uint d1 = (c & 255) * k2;
	uint d2 = ((c >> 8) & 255) * k2;
	uint d3 = ((c >> 16) & 255) * k2;
	for (int64 i = k - n; i >= 0; i--)
	{
		uchar* c2 = (uchar*)&data[((y += dy) >> 32) * size.x + ((x += dx) >> 32)];
		c2[0]     = (c2[0] * kk + d1) >> 8;
		c2[1]     = (c2[1] * kk + d2) >> 8;
		c2[2]     = (c2[2] * kk + d3) >> 8;
	}
}

void _picture::lines(_xy p1, _xy p2, double l, uint c)
{
	uint kk = 255 - (c >> 24);
	if (kk == 0xFF) return; // полностью прозрачная
	if (p1.y > p2.y) std::swap(p1, p2);
	l *= 0.5; // половина толщины
	int64 xmin = int(std::min(p1.x, p2.x) - l);
	int64 xmax = int(std::max(p1.x, p2.x) + l);
	int64 ymin = int(p1.y - l);
	int64 ymax = int(p2.y + l);
	if (xmin < area.x.min) xmin = area.x.min;
	if (ymin < area.y.min) ymin = area.y.min;
	if (xmax >= area.x.max) xmax = area.x.max - 1;
	if (ymax >= area.y.max) ymax = area.y.max - 1;
	if ((xmax < xmin) || (ymax < ymin)) return; // активная область за экраном
	double dx = p2.x - p1.x;
	double dy = p2.y - p1.y;
	if (abs(dy) < 0.001) dy = 0.001; // костыль против горизонтальной линии
	double ll_1 = 1.0 / sqrt(dx * dx + dy * dy);
	if (ll_1 > 2) return; // слишком короткая линия
	double x2y1_y2x1    = p2.x * p1.y - p2.y * p1.x;
	double rcentrscreen = (dy * size.x * 0.5 - dx * size.y * 0.5 + x2y1_y2x1) * ll_1; // расстояние от линии до центра эрана
	if (rcentrscreen < 0) rcentrscreen = -rcentrscreen;
	if (rcentrscreen - l > ((double)size.x + size.y) * 0.5) return; // линия не задевает экран
	double dxdy = dx / dy;
	double db   = l * sqrt(dxdy * dxdy + 1.0);
	double xx1  = p1.x + (ymin - p1.y) * dxdy; // х верхней точки пересечения
	double xx2  = xx1 + dxdy;                  // x нижней точки пересечения
	if (dxdy < 0) std::swap(xx1, xx2);
	xx1 -= db;
	xx2 += db;
	double drr   = dy * ll_1;
	double temp1 = dy * 0.5 - dx * 0.5 + x2y1_y2x1;
	uint   k2    = 256 - kk;
	uint   red   = (c & 255);
	uint   green = ((c >> 8) & 255);
	uint   blue  = ((c >> 16) & 255);
	uint   d1    = red * k2;
	uint   d2    = green * k2;
	uint   d3    = blue * k2;
	for (int64 j = ymin; j <= ymax; j++, xx1 += dxdy, xx2 += dxdy) // цикл по линиям
	{
		int64 xx1i = (int)xx1;
		int64 xx2i = (int)xx2;
		if (xx1i < xmin) xx1i = xmin;
		if (xx2i > xmax) xx2i = xmax;
		if (xx2i < xx1i) continue;
		double rr = (dy * xx1i - dx * j + temp1) * ll_1;
		uchar* c2 = (uchar*)&(data[j * size.x + xx1i]);
		for (int64 i = xx1i; i <= xx2i; i++, rr += drr, c2 += 4)
		{
			double rra = ((rr < 0) ? -rr : rr) - 0.5;
			if (rra >= l) continue; // пиксель за границей точки
			if (rra + 1.0 <= l)     // пиксель полностью внутри линии
			{
				c2[0] = (c2[0] * kk + d1) >> 8;
				c2[1] = (c2[1] * kk + d2) >> 8;
				c2[2] = (c2[2] * kk + d3) >> 8;
				continue;
			}
			uint kk2 = (uint)(k2 * ((rra >= -l) ? (l - rra) : (2.0 * l))); // частично внутри, частично снаружи
			uint kkk = 256 - kk2;
			c2[0]    = (c2[0] * kkk + red * kk2) >> 8;
			c2[1]    = (c2[1] * kkk + green * kk2) >> 8;
			c2[2]    = (c2[2] * kkk + blue * kk2) >> 8;
		}
	}
}

void _picture::stretch_draw_speed(_picture* bm, int64 nXDest, int64 nYDest, double m)
{
	if (bm->size.x * bm->size.y == 0) return;
	int64 nWidth  = (int)(bm->size.x * m + 0.5);
	int64 nHeight = (int)(bm->size.y * m + 0.5);
	if ((nWidth == bm->size.x) && (nHeight == bm->size.y))
	{
		draw({ nXDest, nYDest }, *bm);
		return;
	}
	int64 nXSrc = 0;
	int64 nYSrc = 0;
	int64 bmrx  = nWidth;
	int64 bmry  = nHeight;
	if (nXDest < area.x.min)
	{
		nWidth += nXDest - area.x.min;
		nXSrc -= nXDest - area.x.min;
		nXDest = area.x.min;
	}
	if (nYDest < area.y.min)
	{
		nHeight += nYDest - area.y.min;
		nYSrc -= nYDest - area.y.min;
		nYDest = area.y.min;
	}
	if (nXSrc < 0)
	{
		nWidth += nXSrc;
		nXDest -= nXSrc;
		nXSrc = 0;
	}
	if (nYSrc < 0)
	{
		nHeight += nYSrc;
		nYDest -= nYSrc;
		nYSrc = 0;
	}
	if ((nXDest >= area.x.max) || (nYDest >= area.y.max)) return;
	if ((nXSrc >= bmrx) || (nYSrc >= bmry)) return;
	if (nXDest + nWidth > area.x.max) nWidth = area.x.max - nXDest;
	if (nYDest + nHeight > area.y.max) nHeight = area.y.max - nYDest;
	if (nXSrc + nWidth > bmrx) nWidth = bmrx - nXSrc;
	if (nYSrc + nHeight > bmry) nHeight = bmry - nYSrc;
	if ((nWidth <= 0) || (nHeight <= 0)) return;
	double mm = 1.0 / m;
	for (int64 j = 0; j < nHeight; j++)
	{
		uchar* s1  = (uchar*)(data + ((nYDest + j) * size.x + nXDest));
		uint*  ss2 = (bm->data + (int64)(((nYSrc + j) * mm)) * bm->size.x);
		for (int64 i = 0; i < nWidth; i++)
		{
			uchar* s2_ = (uchar*)(ss2 + (int64)((nXSrc + i) * mm));
			uint pp2 = s2_[3];
			uint pp1 = 256 - pp2;
			s1[0] = (s1[0] * pp1 + s2_[0] * pp2) >> 8;
			s1[1] = (s1[1] * pp1 + s2_[1] * pp2) >> 8;
			s1[2] = (s1[2] * pp1 + s2_[2] * pp2) >> 8;
			s1 += 4;
		}
	}
}

void _picture::stretch_draw(_picture* bm, int64 x, int64 y, double m)
{
	if (bm->size.x * bm->size.y == 0) return;
	int rx2 = (int)(bm->size.x * m + 0.5);
	int ry2 = (int)(bm->size.y * m + 0.5);
	if ((rx2 == bm->size.x) && (ry2 == bm->size.y))
	{
		draw({ x, y }, *bm);
		return;
	}
	int64 x1 = (x >= area.x.min) ? x : area.x.min;
	int64 y1 = (y >= area.y.min) ? y : area.y.min;
	int64 x2 = x + rx2 - 1;
	int64 y2 = y + ry2 - 1;
	if (x2 >= area.x.max) x2 = area.x.max - 1;
	if (y2 >= area.y.max) y2 = area.y.max - 1;
	if ((x2 < x1) || (y2 < y1)) return;
	int64 nox1 = (int64)x1 - x;
	int64 noy1 = (int64)y1 - y;
	int64 nox2 = (int64)x2 - x;
	int64 noy2 = (int64)y2 - y;
	int64 f1x  = bm->size.x;
	int64 f1y  = bm->size.y;
	int64 f1xx = rx2;
	int64 f1yy = ry2;
	for (int64 j = noy1; j <= noy2; j++)
	{
		int64  pyn  = j * f1y;
		int64  pyk  = (j + 1) * f1y - 1;
		int64  pyn2 = pyn / f1yy;
		int64  pyk2 = pyk / f1yy;
		uchar* p1   = (uchar*)&data[(j + y) * size.x + x1 - 1];
		for (int64 i = nox1; i <= nox2; i++)
		{
			p1 += 4;
			int64 pxn  = i * f1x;
			int64 pxk  = (i + 1) * f1x - 1;
			int64 pxn2 = pxn / f1xx;
			int64 pxk2 = pxk / f1xx;
			if ((pyn2 == pyk2) && (pxn2 == pxk2))
			{
				uchar* p2 = (uchar*)& bm->data[pyn2 * f1x + pxn2];
				uint pp2 = p2[3];
				uint pp1 = 256 - pp2;
				p1[0] = (p1[0] * pp1 + p2[0] * pp2) >> 8;
				p1[1] = (p1[1] * pp1 + p2[1] * pp2) >> 8;
				p1[2] = (p1[2] * pp1 + p2[2] * pp2) >> 8;
				continue;
			}
			if (pyn2 == pyk2)
			{
				uchar* p2 = (uchar*)& bm->data[pyn2 * f1x + pxn2];
				int64 k1 = (pxn2 + 1) * f1xx - pxn;
				int64 k2 = pxk - pxk2 * f1xx + 1;
				int64 S = f1x * 256;
				int64 pp2 = p2[3] * k1;
				int64 R = p2[0] * pp2;
				int64 G = p2[1] * pp2;
				int64 B = p2[2] * pp2;
				int64 A = (255 - p2[3]) * k1;
				p2 += 4;
				for (int64 x = pxn2 + 1; x < pxk2; x++)
				{
					pp2 = p2[3] * f1xx;
					R += p2[0] * pp2;
					G += p2[1] * pp2;
					B += p2[2] * pp2;
					A += (255 - p2[3]) * f1xx;
					p2 += 4;
				}
				pp2 = p2[3] * k2;
				R += p2[0] * pp2;
				G += p2[1] * pp2;
				B += p2[2] * pp2;
				A += (255 - p2[3]) * k2;
				p1[0] = (uchar)((p1[0] * A + R) / S);
				p1[1] = (uchar)((p1[1] * A + G) / S);
				p1[2] = (uchar)((p1[2] * A + B) / S);
				continue;
			}
			if (pxn2 == pxk2)
			{
				uchar* p2 = (uchar*)& bm->data[pyn2 * f1x + pxn2];
				int64 k1 = (pyn2 + 1) * f1yy - pyn;
				int64 k2 = pyk - pyk2 * f1yy + 1;
				int64 S = f1y * 256;
				int64 pp2 = p2[3] * k1;
				int64 R = p2[0] * pp2;
				int64 G = p2[1] * pp2;
				int64 B = p2[2] * pp2;
				int64 A = (255 - p2[3]) * k1;
				p2 += (4 * f1x);
				for (int64 y = pyn2 + 1; y < pyk2; y++)
				{
					pp2 = p2[3] * f1yy;
					R += p2[0] * pp2;
					G += p2[1] * pp2;
					B += p2[2] * pp2;
					A += (255 - p2[3]) * f1yy;
					p2 += (4 * f1x);
				}
				pp2 = p2[3] * k2;
				R += p2[0] * pp2;
				G += p2[1] * pp2;
				B += p2[2] * pp2;
				A += (255 - p2[3]) * k2;
				p1[0] = (uchar)((p1[0] * A + R) / S);
				p1[1] = (uchar)((p1[1] * A + G) / S);
				p1[2] = (uchar)((p1[2] * A + B) / S);
				continue;
			}
			int64 kx1 = (pxn2 + 1) * f1xx - pxn;
			int64 kx2 = pxk - pxk2 * f1xx + 1;
			int64 ky1 = (pyn2 + 1) * f1yy - pyn;
			int64 ky2 = pyk - pyk2 * f1yy + 1;
			int64 S = f1x * f1y * 256;
			uchar* p2 = (uchar*)& bm->data[pyn2 * f1x + pxn2];
			int64 pp2 = p2[3] * kx1 * ky1;
			int64 R = p2[0] * pp2;
			int64 G = p2[1] * pp2;
			int64 B = p2[2] * pp2;
			int64 A = (255 - p2[3]) * kx1 * ky1;
			p2 += 4;
			for (int64 x = pxn2 + 1; x < pxk2; x++)
			{
				pp2 = p2[3] * f1xx * ky1;
				R += p2[0] * pp2;
				G += p2[1] * pp2;
				B += p2[2] * pp2;
				A += (255 - p2[3]) * f1xx * ky1;
				p2 += 4;
			}
			pp2 = p2[3] * kx2 * ky1;
			R += p2[0] * pp2;
			G += p2[1] * pp2;
			B += p2[2] * pp2;
			A += (255 - p2[3]) * kx2 * ky1;
			p2 = (uchar*)& bm->data[pyk2 * f1x + pxn2];
			pp2 = p2[3] * kx1 * ky2;
			R += p2[0] * pp2;
			G += p2[1] * pp2;
			B += p2[2] * pp2;
			A += (255 - p2[3]) * kx1 * ky2;
			p2 += 4;
			for (int64 x = pxn2 + 1; x < pxk2; x++)
			{
				pp2 = p2[3] * f1xx * ky2;
				R += p2[0] * pp2;
				G += p2[1] * pp2;
				B += p2[2] * pp2;
				A += (255 - p2[3]) * f1xx * ky2;
				p2 += 4;
			}
			pp2 = p2[3] * kx2 * ky2;
			R += p2[0] * pp2;
			G += p2[1] * pp2;
			B += p2[2] * pp2;
			A += (255 - p2[3]) * kx2 * ky2;
			p2 = (uchar*)& bm->data[(pyn2 + 1) * f1x + pxn2];
			for (int64 y = pyn2 + 1; y < pyk2; y++)
			{
				pp2 = p2[3] * f1yy * kx1;
				R += p2[0] * pp2;
				G += p2[1] * pp2;
				B += p2[2] * pp2;
				A += (255 - p2[3]) * f1yy * kx1;
				p2 += (4 * f1x);
			}
			p2 = (uchar*)&bm->data[(pyn2 + 1) * f1x + pxk2];
			for (int64 y = pyn2 + 1; y < pyk2; y++)
			{
				pp2 = p2[3] * f1yy * kx2;
				R += p2[0] * pp2;
				G += p2[1] * pp2;
				B += p2[2] * pp2;
				A += (255 - p2[3]) * f1yy * kx2;
				p2 += (4 * f1x);
			}
			for (int64 y = pyn2 + 1; y < pyk2; y++)
			{
				p2 = (uchar*)&bm->data[y * f1x + pxn2 + 1];
				for (int64 x = pxn2 + 1; x < pxk2; x++)
				{
					pp2 = p2[3] * f1xx * f1yy;
					R += p2[0] * pp2;
					G += p2[1] * pp2;
					B += p2[2] * pp2;
					A += (255 - p2[3]) * f1xx * f1yy;
					p2 += 4;
				}
			}
			p1[0] = (uchar)((p1[0] * A + R) / S);
			p1[1] = (uchar)((p1[1] * A + G) / S);
			p1[2] = (uchar)((p1[2] * A + B) / S);
		}
	}
}

void _picture::line_vert_rep_speed(_ixy p, i8 y2, uint c)
{
	uint* c2 = &data[p.y * size.x + p.x];
	for (int64 y = p.y - y2; y <= 0; y++) { *c2 = c; c2 += size.x; }
}

void _picture::fill_rect_rep_speed(_iarea r, uint c)
{
	if (r.x.size() == 1)
	{
		line_vert_rep_speed({ r.x.min, r.y.min }, r.y.max-1, c);
		return;
	}
	uint64 cc = (((uint64)c) << 32) + c;
	for (int64 i = r.y.min; i < r.y.max; i++)
	{
		uint64* ee = (uint64*)(&(data[i * size.x + r.x.min]));
		uint64* eemax = (uint64*)(&(data[i * size.x + r.x.max-1]));
		while (ee < eemax) *ee++ = cc;
		if (ee == eemax) *((uint*)ee) = c;
	}
}

void _picture::fill_rectangle(_iarea r, uint c, bool rep)
{
	r &= area;
	if (r.empty()) return;
	uint kk = (c >> 24);
	if (rep || (kk == 0xFF)) { fill_rect_rep_speed(r, c); return; }
	if (kk == 0) return;  // полностью прозрачная
	if (transparent)
		fill_rect_transparent_speed(r, c);
	else
		fill_rect_speed(r, c);
}

void _picture::fill_rect_transparent_speed(_iarea r, uint c)
{
	int64 dx = r.x.size();
	if (dx == 1) // вертикальная линия
	{
		line({ r.x.min, r.y.min }, { r.x.min, r.y.max - 1 }, c); // !!!!!!!!!!!!!!!! вызвать нужную
		return;
	}
	uint kk = 255 - (c >> 24);
	uint k2 = 256 - kk;
	uint d1 = (c & 255) * k2;
	uint d2 = ((c >> 8) & 255) * k2;
	uint d3 = ((c >> 16) & 255) * k2;
	uint d11 = d1 * 255;
	uint d22 = d2 * 255;
	uint d33 = d3 * 255;
	for (int64 i = r.y.min; i < r.y.max; i++)
	{
		uchar* c2 = (uchar*) & (data[i * size.x + r.x.min]);
		for (int64 d = -dx; d < 0; d++)
		{
			uint kk_ = 255 - c2[3];
			uint k2_ = (256 - kk_) * kk;
			uint znam = 65536 - kk * kk_;
			c2[0] = (c2[0] * k2_ + d11) / znam;
			c2[1] = (c2[1] * k2_ + d22) / znam;
			c2[2] = (c2[2] * k2_ + d33) / znam;
			c2[3] = 255 - ((kk_ * kk) >> 8);
			c2 += 4;
		}
	}
}

void _picture::fill_rect_speed(_iarea r, uint c)
{
	int64 dx = r.x.size();
	if (dx == 1) // вертикальная линия
	{
		line({ r.x.min, r.y.min }, { r.x.min, r.y.max - 1 }, c); // !!!!!!!!!!!!!!!! вызвать нужную
		return;
	}
	uint kk = 255 - (c >> 24);
	uint k2 = 256 - kk;
	uint d1 = (c & 255) * k2;
	uint d2 = ((c >> 8) & 255) * k2;
	uint d3 = ((c >> 16) & 255) * k2;
	for (int64 i = r.y.min; i < r.y.max; i++)
	{
		uchar* c2 = (uchar*) & (data[i * size.x + r.x.min]);
		for (int64 d = -dx; d < 0; d++)
		{
			c2[0] = (c2[0] * kk + d1) >> 8;
			c2[1] = (c2[1] * kk + d2) >> 8;
			c2[2] = (c2[2] * kk + d3) >> 8;
			c2 += 4;
		}
	}
}

constexpr int lx2 = 11;
constexpr ushort font16[256][lx2] = { {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {},
   {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {1532}, {28, 0, 0, 28}, {272, 2044, 272, 272, 2044, 272},
   {560, 1096, 4092, 1160, 784}, {520, 276, 148, 584, 1312, 1296, 520}, {920, 1124, 1176, 768, 1152}, {28},
   {4088, 4100}, {4100, 4088}, {40, 16, 40}, {128, 128, 992, 128, 128}, {2048, 1024}, {128, 128}, {1024},
   {1536, 384, 96, 28}, {1016, 1028, 1028, 1028, 1016}, {8, 8, 2044}, {1544, 1284, 1156, 1092, 1080},
   {520, 1028, 1092, 1092, 952}, {384, 352, 280, 2044, 256}, {636, 1060, 1060, 1060, 964},
   {1016, 1092, 1092, 1092, 904}, {4, 1796, 196, 52, 12}, {952, 1092, 1092, 1092, 952}, {568, 1092, 1092, 1092, 1016},
   {1056}, {2048, 1056}, {128, 320, 544, 1040}, {320, 320, 320, 320, 320}, {1040, 544, 320, 128}, {8, 4, 1412, 68, 56},
   {480, 1560, 1032, 2244, 2340, 2340, 2276, 2312, 280, 480}, {1536, 448, 304, 268, 304, 448, 1536},
   {2044, 1092, 1092, 1092, 952}, {1016, 1028, 1028, 1028, 1028, 520}, {2044, 1028, 1028, 1028, 520, 496},
   {2044, 1092, 1092, 1092, 1028}, {2044, 68, 68, 68, 4}, {1016, 1028, 1028, 1092, 580, 1992},
   {2044, 64, 64, 64, 64, 2044}, {2044}, {768, 1024, 1024, 1020}, {2044, 96, 144, 264, 516, 1024},
   {2044, 1024, 1024, 1024, 1024}, {2044, 48, 192, 768, 192, 48, 2044}, {2044, 24, 96, 128, 768, 2044},
   {1016, 1028, 1028, 1028, 1028, 1016}, {2044, 68, 68, 68, 68, 56}, {1016, 1028, 1028, 1284, 1540, 3064},
   {2044, 68, 68, 68, 68, 1976}, {568, 1092, 1092, 1092, 904}, {4, 4, 2044, 4, 4},
   {1020, 1024, 1024, 1024, 1024, 1020}, {12, 112, 384, 1536, 384, 112, 12},
   {12, 112, 384, 1536, 384, 112, 384, 1536, 384, 112, 12}, {1548, 272, 160, 64, 160, 272, 1548},
   {12, 16, 32, 1984, 32, 16, 12}, {1540, 1284, 1156, 1092, 1060, 1044, 1036}, {8188, 4100}, {28, 96, 384, 1536},
   {4100, 8188}, {8, 4, 2, 4, 8}, {4096, 4096, 4096, 4096, 4096, 4096}, {4, 8}, {768, 1184, 1184, 1184, 1984},
   {2044, 1056, 1056, 1056, 960}, {960, 1056, 1056, 1056, 576}, {960, 1056, 1056, 1056, 2044},
   {960, 1184, 1184, 1184, 704}, {2040, 36}, {5056, 5152, 5152, 5152, 4064}, {2044, 64, 32, 32, 1984}, {2020}, {8164},
   {2044, 128, 320, 544, 1024}, {2044}, {2016, 32, 32, 1984, 32, 32, 1984}, {2016, 64, 32, 32, 1984},
   {960, 1056, 1056, 1056, 960}, {8160, 1056, 1056, 1056, 960}, {960, 1056, 1056, 1056, 8160}, {2016, 32},
   {576, 1184, 1312, 576}, {1016, 1056}, {992, 1024, 1024, 512, 2016}, {96, 384, 1536, 384, 96},
   {480, 1536, 384, 96, 384, 1536, 480}, {1632, 384, 384, 1632}, {4096, 4576, 3584, 512, 480},
   {1568, 1312, 1184, 1120}, {64, 1980, 2050}, {4092}, {2050, 1980, 64}, {16, 8, 8, 16, 16, 8}, {1008, 1008, 992},
   {4, 4, 4, 2044, 68, 1604, 1092, 896}, {2044, 4, 6, 5, 4}, {2048, 1024}, {2016, 32, 40, 36},
   {2048, 1024, 2048, 1024}, {1024, 0, 1024, 0, 1024}, {32, 32, 2040, 32, 32}, {544, 544, 2040, 544, 544},
   {160, 1016, 1188, 1060, 1028, 792}, {520, 276, 148, 584, 1312, 1296, 520, 516, 1280, 1280, 512},
   {1024, 1016, 4, 4, 4, 2044, 1088, 1088, 1088, 896}, {256, 640, 1088},
   {2044, 64, 64, 64, 64, 2044, 1088, 1088, 1088, 896}, {2044, 96, 146, 265, 516, 1024},
   {4, 4, 4, 2044, 132, 68, 68, 1920}, {2044, 1024, 7168, 1024, 2044}, {8, 2044, 72, 4136, 4128, 4032}, {8, 16},
   {16, 8}, {4, 8, 4, 8}, {8, 4, 8, 4}, {64, 224, 224, 64}, {64, 64, 64}, {64, 64, 64, 64, 64, 64}, {},
   {4, 28, 4, 0, 28, 4, 8, 4, 28}, {1024, 960, 32, 32, 2016, 1152, 1152, 1152, 768}, {1088, 640, 256},
   {2016, 128, 128, 128, 2016, 1152, 1152, 1152, 768}, {2016, 128, 328, 548, 1024}, {8, 2044, 72, 40, 40, 1984},
   {2016, 1024, 7168, 1024, 2016}, {}, {636, 1153, 1154, 1154, 1153, 1020}, {4580, 4616, 4616, 4068},
   {768, 1024, 1024, 1020}, {372, 136, 136, 136, 372}, {2044, 4, 4, 4, 6}, {8060}, {1128, 2196, 2340, 2628, 1416},
   {2044, 1093, 1092, 1093, 1028}, {112, 136, 292, 340, 340, 260, 136, 112}, {1016, 1092, 1092, 1092, 1028, 520},
   {256, 640, 1344, 640, 1088}, {128, 128, 128, 128, 384}, {64, 64}, {252, 258, 378, 298, 298, 338, 258, 252},
   {1, 2044, 1}, {8, 20, 8}, {1088, 1088, 1520, 1088, 1088}, {2044}, {2020}, {2016, 32, 32, 48},
   {32, 4064, 1024, 1056, 2016, 512}, {56, 124, 4092, 4, 4092}, {64}, {960, 1192, 1184, 1192, 704},
   {2044, 24, 96, 128, 768, 2044, 0, 144, 168, 144}, {960, 1184, 1184, 1056, 576}, {1088, 640, 1344, 640, 256}, {8164},
   {568, 1092, 1092, 1092, 904}, {576, 1184, 1312, 576}, {4, 2016, 4}, {1536, 448, 304, 268, 304, 448, 1536},
   {2044, 1092, 1092, 1092, 900}, {2044, 1092, 1092, 1092, 952}, {2044, 4, 4, 4, 4},
   {3072, 1536, 1532, 1028, 1028, 1028, 2044, 3072}, {2044, 1092, 1092, 1092, 1028},
   {1024, 516, 264, 144, 96, 2044, 96, 144, 264, 516, 1024}, {512, 1032, 1028, 1092, 1092, 952},
   {2044, 768, 128, 96, 24, 2044}, {2044, 769, 130, 98, 25, 2044}, {2044, 96, 144, 264, 516, 1024},
   {1024, 1016, 4, 4, 4, 2044}, {2044, 48, 192, 768, 192, 48, 2044}, {2044, 64, 64, 64, 64, 2044},
   {1016, 1028, 1028, 1028, 1028, 1016}, {2044, 4, 4, 4, 4, 2044}, {2044, 68, 68, 68, 68, 56},
   {1016, 1028, 1028, 1028, 1028, 520}, {4, 4, 2044, 4, 4}, {636, 1152, 1152, 1152, 1152, 1020},
   {112, 136, 136, 136, 2044, 136, 136, 136, 112}, {1548, 272, 160, 64, 160, 272, 1548},
   {2044, 1024, 1024, 1024, 2044, 3072}, {124, 128, 128, 128, 128, 2044}, {2044, 1024, 1024, 2044, 1024, 1024, 2044},
   {2044, 1024, 1024, 2044, 1024, 1024, 2044, 3072}, {12, 4, 2044, 1088, 1088, 1088, 1088, 896},
   {2044, 1088, 1088, 1088, 1088, 896, 0, 2044}, {2044, 1088, 1088, 1088, 1088, 896},
   {520, 1028, 1092, 1092, 1092, 1016}, {2044, 64, 1016, 1028, 1028, 1028, 1016}, {1592, 452, 68, 68, 68, 2044},
   {768, 1184, 1184, 1184, 1984}, {992, 1072, 1064, 1064, 964}, {2016, 1184, 1184, 1184, 832}, {2016, 32, 32, 32},
   {3072, 1984, 1056, 1056, 2016, 3072}, {960, 1184, 1184, 1184, 704}, {1056, 832, 128, 2016, 128, 832, 1056},
   {576, 1056, 1184, 1184, 832}, {2016, 512, 256, 128, 2016}, {2016, 520, 272, 136, 2016}, {2016, 128, 320, 544, 1024},
   {1024, 960, 32, 32, 2016}, {2016, 64, 128, 768, 128, 64, 2016}, {2016, 128, 128, 128, 2016},
   {960, 1056, 1056, 1056, 960}, {2016, 32, 32, 32, 2016}, {8160, 1056, 1056, 1056, 960}, {960, 1056, 1056, 1056, 576},
   {32, 32, 2016, 32, 32}, {4576, 4608, 4608, 4064}, {448, 544, 544, 4080, 544, 544, 448}, {1632, 384, 384, 1632},
   {2016, 1024, 1024, 2016, 3072}, {224, 256, 256, 2016}, {2016, 1024, 1024, 2016, 1024, 1024, 2016},
   {2016, 1024, 1024, 2016, 1024, 1024, 2016, 2048}, {96, 32, 2016, 1152, 1152, 1152, 768},
   {2016, 1152, 1152, 1152, 768, 0, 2016}, {2016, 1152, 1152, 1152, 768}, {576, 1056, 1184, 1184, 960},
   {2016, 128, 960, 1056, 1056, 1056, 960}, {1216, 800, 288, 288, 2016} };

_isize  _picture::size_text16(std::string_view s, int64 n)
{
	i8 l = 0;
	i8 probel = 0;
	for (auto c : s)
	{
		probel = (c == ' ') ? 4 : 1;
		l += probel;
		const ushort* ss = font16[(uchar)(c)];
		int           lx = lx2;
		for (int j = lx - 1; j >= 0; j--)
		{
			if (ss[j]) break;
			lx--;
		}
		l += lx;
	}
	l -= probel;
	return {l*n, 13*n};
}

void _picture::text16n(int64 x, int64 y, astr s, int64 n, uint c)
{
	if (n <= 1)
	{
		if (n == 1) text16(x, y, s, c);
		return;
	}
	constexpr int ly = 13;
	if ((y >= area.y.max) || (y + ly * n <= area.y.min)) return;
	uint kk = 255 - (c >> 24);
	if (kk == 0xFF) return; // полностью прозрачная
	uint k2  = 256 - kk;
	uint d1  = (c & 255) * k2;
	uint d2  = ((c >> 8) & 255) * k2;
	uint d3  = ((c >> 16) & 255) * k2;
	uint d11 = d1 * 255;
	uint d22 = d2 * 255;
	uint d33 = d3 * 255;

	int64 j0 = 0;
	int64 j1 = ly;
	if (y < area.y.min) j0 = (area.y.min + n - 1 - y) / n;
	if (y + ly * n > area.y.max) j1 = (area.y.max - y) / n;
	while ((*s) && (*s != '\n') && (x < area.x.max))
	{
		int64         probel = (*s == 32) ? 4 : 1;
		const ushort* ss     = font16[(uchar)(*s++)];
		int64         lx     = lx2;
		uchar         r      = 0;
		for (int64 j = lx - 1; j >= 0; j--)
		{
			if (ss[j]) break;
			lx--;
		}
		if (x + lx * n <= 0)
		{
			x += (lx + probel) * n;
			continue;
		}
		int64 i0 = 0;
		int64 i1_ = lx;
		if (x < area.x.min) i0 = (area.x.min + n - 1 - x) / n;
		if (x + lx * n > area.x.max) i1_ = (area.x.max - x) / n;
		for (int64 j = j0; j < j1; j++)
		{
			ushort mask = (ushort(1) << j);
			for (int64 jj = 0; jj < n; jj++)
			{
				const ushort* sss = ss;
				if (kk == 0)
				{
					uint* c2 = &data[(y + j * n + jj) * size.x + x + i0 * n];
					for (int64 i = i0; i < i1_; i++)
					{
						if (*sss & mask)
						{
							for (int ii = 0; ii < n; ii++)
							{
								*c2 = c;
								c2++;
							}
						}
						else
							c2 += n;
						sss++;
					}
				}
				else
				{
					uchar* c2 = (uchar*)&data[(y + j * n + jj) * size.x + x + i0 * n];
					if (!transparent)
					{
						for (int64 i = i0; i < i1_; i++)
						{
							if (*sss & mask)
							{
								for (int ii = 0; ii < n; ii++)
								{
									c2[0] = (c2[0] * kk + d1) >> 8;
									c2[1] = (c2[1] * kk + d2) >> 8;
									c2[2] = (c2[2] * kk + d3) >> 8;
									c2 += 4;
								}
							}
							else
								c2 += 4uLL * n;
							sss++;
						}
					}
					else
					{
						for (int64 i = i0; i < i1_; i++)
						{
							if (*sss & mask)
							{
								for (int ii = 0; ii < n; ii++)
								{
									uint kk_ = 255 - c2[3];
									uint k2_ = (256 - kk_) * kk;
									uint znam = 65536 - kk * kk_;
									c2[0] = (c2[0] * k2_ + d11) / znam;
									c2[1] = (c2[1] * k2_ + d22) / znam;
									c2[2] = (c2[2] * k2_ + d33) / znam;
									c2[3] = 255 - ((kk_ * kk) >> 8);
									c2 += 4;
								}
							}
							else
								c2 += 4uLL * n;
							sss++;
						}
					}
				}
			}
		}
		x += (lx + probel) * n;
	}
}

void _picture::text16(int64 x, int64 y, astr s, uint c)
{
	constexpr int ly = 13;
	if ((y >= area.y.max) || (y + ly <= area.y.min)) return;
	uint kk = 255 - (c >> 24);
	if (kk == 0xFF) return; // полностью прозрачная
	uint k2  = 256 - kk;
	uint d1  = (c & 255) * k2;
	uint d2  = ((c >> 8) & 255) * k2;
	uint d3  = ((c >> 16) & 255) * k2;
	uint d11 = d1 * 255;
	uint d22 = d2 * 255;
	uint d33 = d3 * 255;

	int64 j0 = 0;
	int64 j1 = ly;
	if (y < area.y.min) j0 = area.y.min - y;
	if (y + ly > area.y.max) j1 = area.y.max - y;
	while ((*s) && (*s != '\n') && (x < area.x.max))
	{
		int64         probel = (*s == 32) ? 4 : 1;
		const ushort* ss     = font16[(uchar)(*s++)];
		int64         lx     = lx2;
		uchar         r      = 0;
		for (int64 j = lx - 1; j >= 0; j--)
		{
			if (ss[j]) break;
			lx--;
		}
		if (x + lx <= 0)
		{
			x += lx + probel;
			continue;
		}
		int64 i0 = 0;
		int64 i1_ = lx;
		if (x < area.x.min) i0 = area.x.min - x;
		if (x + lx > area.x.max) i1_ = area.x.max - x;
		for (int64 j = j0; j < j1; j++)
		{
			ushort        mask = (ushort(1) << j);
			const ushort* sss  = ss;
			if (kk == 0)
			{
				uint* c2 = &data[(y + j) * size.x + x + i0];
				for (int64 i = i0; i < i1_; i++)
				{
					if (*sss++ & mask) *c2 = c;
					c2++;
				}
			}
			else
			{
				uchar* c2 = (uchar*)&data[(y + j) * size.x + x + i0];
				if (!transparent)
				{
					for (int64 i = i0; i < i1_; i++)
					{
						if (*sss++ & mask)
						{
							c2[0] = (c2[0] * kk + d1) >> 8;
							c2[1] = (c2[1] * kk + d2) >> 8;
							c2[2] = (c2[2] * kk + d3) >> 8;
						}
						c2 += 4;
					}
				}
				else
				{
					for (int64 i = i0; i < i1_; i++)
					{
						if (*sss++ & mask)
						{
							uint kk_ = 255 - c2[3];
							uint k2_ = (256 - kk_) * kk;
							uint znam = 65536 - kk * kk_;
							c2[0] = (c2[0] * k2_ + d11) / znam;
							c2[1] = (c2[1] * k2_ + d22) / znam;
							c2[2] = (c2[2] * k2_ + d33) / znam;
							c2[3] = 255 - ((kk_ * kk) >> 8);
						}
						c2 += 4;
					}
				}
			}
		}
		x += lx + probel;
	}
}

//  1 -      1
//  2 -      3
//  3 -     11
//  4 -     43
//  5 -    171
//  6 -    683
//  7 -   2731
//  8 -  10923
//  9 -  43691
// 10 - 174763
// 11 - 699051
void _picture::froglif(_xy p, double r, uchar* f, int rf, uint c, uint c2)
{
	if (r < 1) return; // нечего рисовать
	if (((c | c2) >> 24) == 0x00) return; // полностью прозрачный
	uint kk = 255 - (c >> 24);
	uint k2 = 256 - kk;
	uint c_1 = (c & 255);
	uint c_2 = ((c >> 8) & 255);
	uint c_3 = ((c >> 16) & 255);
	uint d1  = c_1 * k2;
	uint d2  = c_2 * k2;
	uint d3  = c_3 * k2;

	double k_a = 0.75; // коэффициент уменьшения толщины линий х.з какой лучше

	int64 x1 = (int64)p.x;
	int64 x2 = (int64)(p.x + r);
	int64 y1 = (int64)p.y;
	int64 y2 = (int64)(p.y + r);

	if (x1 < area.x.min) x1 = area.x.min;
	if (y1 < area.y.min) y1 = area.y.min;
	if (x2 >= area.x.max) x2 = area.x.max - 1;
	if (y2 >= area.y.max) y2 = area.y.max - 1;
	if ((x2 < x1) || (y2 < y1)) return;
	fill_rectangle({ {x1 - 1, x2 + 2}, {y1 - 1, y2 + 2} }, c2);

	double aa    = 1.0625;    // минимальная толщина линии
	double bb    = 1.0625;    // минимальное пустое место
	double kk_aa = 1.0 / k_a; // коэффициент увеличения линии
	int    rf0   = 0;
	double sl1   = 2 * aa + aa / (kk_aa - 2);
	double sl2   = bb - aa / (kk_aa - 2);
	double ll    = aa;
	while (sl1 * kk_aa + sl2 * 2 <= r)
	{
		rf0++;
		sl1 *= kk_aa;
		sl2 *= 2;
		ll *= kk_aa;
	}
	if (rf0 > rf) rf0 = rf;

	struct xxxyyy
	{
		int    ii;     // разреженные биты
		int    sm1;    // смещение ii до 1 на конце
		double l1, l2; // границы столбика
		uchar* f;      // начало нужного куска
		int64 x1, x2;    // реальные координаты начала-конца не выходящие за рамки
		uint kk_1, kk_2;                         // коэффициенты границ
		uint d1_1, d2_1, d3_1, d1_2, d2_2, d3_2; // коэффициенты границ
	};

	int rr   = (1 << rf0);
	int rr_1 = rr - 1;

	const int Nxx = 17;
	if (rr + 1 > Nxx) return;
	static xxxyyy xx[Nxx]; // вектор размера rr+1
	static xxxyyy yy[Nxx]; // вектор размера rr+1

	int drf2 = (rf - rf0) * 2;

	for (int i = 0; i <= rr; i++)
	{
		xx[i].ii = ((i & 0x1) + ((i & 0x2) << 1) + ((i & 0x4) << 2) + ((i & 0x8) << 3) + ((i & 0x10) << 4) +
		            ((i & 0x20) << 5) + ((i & 0x40) << 6) + ((i & 0x80) << 7) + ((i & 0x100) << 8) +
		            ((i & 0x200) << 9) + ((i & 0x400) << 10) + ((i & 0x800) << 11) + ((i & 0x1000) << 12) +
		            ((i & 0x2000) << 13) + ((i & 0x4000) << 14))
		           << drf2;
	}

	int sm1 = rf * 2;

	xx[0].l1    = (double)p.x;
	xx[0].l2    = p.x + ll;
	xx[0].sm1   = sm1;
	xx[0].f     = f;
	xx[0].x1    = (int)xx[0].l1;
	xx[0].x2    = (int)xx[0].l2;
	xx[rr].l1   = p.x + r - ll;
	xx[rr].l2   = p.x + r;
	xx[rr].sm1  = sm1;
	xx[rr].f    = f;
	xx[rr].x1   = (int)xx[rr].l1;
	xx[rr].x2   = (int)xx[rr].l2;
	xxxyyy* xxx = &(xx[0]);
	if ((xxx->x1 <= x2) && (xxx->x2 >= x1))
	{
		if (xxx->l1 < x1)
		{
			xxx->x1   = x1;
			xxx->kk_1 = kk;
			xxx->d1_1 = d1;
			xxx->d2_1 = d2;
			xxx->d3_1 = d3;
		}
		else
		{
			uint k3   = (uint)(k2 * ((xxx->x1 + 1.0) - xxx->l1));
			xxx->kk_1 = 256 - k3;
			xxx->d1_1 = c_1 * k3;
			xxx->d2_1 = c_2 * k3;
			xxx->d3_1 = c_3 * k3;
		}
		if (xxx->x2 > x2)
		{
			xxx->x2   = x2;
			xxx->kk_2 = kk;
			xxx->d1_2 = d1;
			xxx->d2_2 = d2;
			xxx->d3_2 = d3;
		}
		else
		{
			uint k3   = (uint)(k2 * (xxx->l2 - xxx->x2));
			xxx->kk_2 = 256 - k3;
			xxx->d1_2 = c_1 * k3;
			xxx->d2_2 = c_2 * k3;
			xxx->d3_2 = c_3 * k3;
		}
	}
	xxx = &(xx[rr]);
	if ((xxx->x1 <= x2) && (xxx->x2 >= x1))
	{
		if (xxx->l1 < x1)
		{
			xxx->x1   = x1;
			xxx->kk_1 = kk;
			xxx->d1_1 = d1;
			xxx->d2_1 = d2;
			xxx->d3_1 = d3;
		}
		else
		{
			uint k3   = (uint)(k2 * ((xxx->x1 + 1.0) - xxx->l1));
			xxx->kk_1 = 256 - k3;
			xxx->d1_1 = c_1 * k3;
			xxx->d2_1 = c_2 * k3;
			xxx->d3_1 = c_3 * k3;
		}
		if (xxx->x2 > x2)
		{
			xxx->x2   = x2;
			xxx->kk_2 = kk;
			xxx->d1_2 = d1;
			xxx->d2_2 = d2;
			xxx->d3_2 = d3;
		}
		else
		{
			uint k3   = (uint)(k2 * (xxx->l2 - xxx->x2));
			xxx->kk_2 = 256 - k3;
			xxx->d1_2 = c_1 * k3;
			xxx->d2_2 = c_2 * k3;
			xxx->d3_2 = c_3 * k3;
		}
	}

	int    step = (rr >> 1);
	uchar* ff   = f;
	int    dff  = 1;
	while (step)
	{
		sm1 -= 2;
		ll *= k_a;
		for (int i = step; i <= rr; i += step * 2)
		{
			double  ce  = (xx[i - step].l2 + xx[i + step].l1) * 0.5;
			xxxyyy* xxx = &(xx[i]);
			xxx->l1     = ce - ll * 0.5;
			xxx->l2     = ce + ll * 0.5;
			xxx->sm1    = sm1;
			xxx->f      = ff;
			xxx->x1     = (int)xxx->l1;
			xxx->x2     = (int)xxx->l2;
			if ((xxx->x1 > x2) || (xxx->x2 < x1)) continue;
			if (xxx->l1 < x1)
			{
				xxx->x1   = x1;
				xxx->kk_1 = kk;
				xxx->d1_1 = d1;
				xxx->d2_1 = d2;
				xxx->d3_1 = d3;
			}
			else
			{
				uint k3   = (uint)(k2 * ((xxx->x1 + 1.0) - xxx->l1));
				xxx->kk_1 = 256 - k3;
				xxx->d1_1 = c_1 * k3;
				xxx->d2_1 = c_2 * k3;
				xxx->d3_1 = c_3 * k3;
			}
			if (xxx->x2 > x2)
			{
				xxx->x2   = x2;
				xxx->kk_2 = kk;
				xxx->d1_2 = d1;
				xxx->d2_2 = d2;
				xxx->d3_2 = d3;
			}
			else
			{
				uint k3   = (uint)(k2 * (xxx->l2 - xxx->x2));
				xxx->kk_2 = 256 - k3;
				xxx->d1_2 = c_1 * k3;
				xxx->d2_2 = c_2 * k3;
				xxx->d3_2 = c_3 * k3;
			}
		}
		ff += dff;
		if (dff == 1)
			dff = 2;
		else
			dff *= 4;
		step >>= 1;
	}
	double dy = (double)(p.y - p.x);
	for (int i = 0; i <= rr; i++)
	{
		yy[i].f   = xx[i].f;
		yy[i].ii  = (xx[i].ii << 1);
		yy[i].sm1 = xx[i].sm1;
		yy[i].l1  = xx[i].l1 + dy;
		yy[i].l2  = xx[i].l2 + dy;
	}
	yy[0].l1  = (double)p.y;     // из-за маленькой дельточки возникла ошибка!
	yy[rr].l2 = p.y + r; // на всякий случай тоже

	int ay = 0;
	while (ay < rr)
	{
		if (y1 < yy[ay].l2) break;
		ay++;
	}
	int ax1 = 0;
	while (ax1 < rr)
	{
		if (x1 < xx[ax1].l2) break;
		ax1++;
	}

	xxxyyy* yyay_1 = &(yy[ay]);
	xxxyyy* yyay   = &(yy[ay]);
	for (int64 y = y1; y <= y2; y++)
	{
		if (y >= yyay->l2)
		{
			ay++;
			if (ay > rr) break;
			yyay_1 = yyay;
			yyay   = &(yy[ay]);
		}
		int ax = ax1;
		if (y + 1.0 <= yyay->l1) // рисуются только вертикальные линии
		{
			while (true)
			{
				// поиск рисуемой палочки
				while (ax <= rr)
				{
					uint ii;
					if (ax & (rr_1))
						ii = (xx[ax].ii + yy[ay - 1].ii) >> xx[ax].sm1; // ay не может быть равно 0
					else
						ii = 4 + 2 * (ax >> rf0); // первая и последняя палочка
					if (xx[ax].f[ii >> 3] & (uchar(1) << (ii & 7))) break;
					ax++;
				}
				if (ax > rr) break;
				xxxyyy* xxax = &(xx[ax]);
				int64 x11  = xxax->x1;
				if (x11 > x2) break;
				int64 x22 = xxax->x2;
				uchar* cc  = (uchar*)&(data[y * size.x + x11]);
				cc[0]      = (cc[0] * xxax->kk_1 + xxax->d1_1) >> 8;
				cc[1]      = (cc[1] * xxax->kk_1 + xxax->d2_1) >> 8;
				cc[2]      = (cc[2] * xxax->kk_1 + xxax->d3_1) >> 8;
				cc += 4;
				int64 d = x11 - x22 + 2;
				if (d <= 1)
				{
					while (d++ <= 0)
					{
						cc[0] = (cc[0] * kk + d1) >> 8;
						cc[1] = (cc[1] * kk + d2) >> 8;
						cc[2] = (cc[2] * kk + d3) >> 8;
						cc += 4;
					}
					cc[0] = (cc[0] * xxax->kk_2 + xxax->d1_2) >> 8;
					cc[1] = (cc[1] * xxax->kk_2 + xxax->d2_2) >> 8;
					cc[2] = (cc[2] * xxax->kk_2 + xxax->d3_2) >> 8;
				}
				ax++;
			}
			continue;
		}
		int stepy = 1 << ((yyay->sm1 - drf2) >> 1);
		int ayx   = ax - 1;
		if (ayx < 0) ayx = 0;
		ayx &= (0xFFFFFFFF << ((yyay->sm1 - drf2) >> 1));
		if ((y == (int)yyay->l1) && (yyay->l1 > 0))
		{
			uint k3  = (uint)(k2 * ((y + 1.0) - yyay->l1));
			uint kkw = 256 - k3;
			uint d1w = c_1 * k3;
			uint d2w = c_2 * k3;
			uint d3w = c_3 * k3;
			while (true)
			{
				// поиск рисуемой палочки
				while (ayx < rr)
				{
					uint ii;
					if (ay & (rr_1))
						ii = (((xx[ayx].ii + yyay->ii) >> yyay->sm1) - 1) & 0xFFFFFFFE;
					else
						ii = 5 + 2 * (ay >> rf0); // первая и последняя палочка
					if (yyay->f[ii >> 3] & (uchar(1) << (ii & 7))) break;
					ayx += stepy;
				}
				int ayx_next = ayx + stepy; // следующая после серии палочек
				while (ayx_next < rr)
				{
					uint ii;
					if (ay & (rr_1))
						ii = (((xx[ayx_next].ii + yyay->ii) >> yyay->sm1) - 1) & 0xFFFFFFFE;
					else
						ii = 5 + 2 * (ay >> rf0); // первая и последняя палочка
					if ((yyay->f[ii >> 3] & (uchar(1) << (ii & 7))) == 0) break;
					ayx_next += stepy;
				}
				bool nac = true;
				bool kon = true;
				int64 x11, x22;
				if (ayx >= rr)
				{
					x11 = x2 + 1;
					x22 = x2; // ?? не нужно ??
				}
				else
				{
					x11 = xx[ayx].x1;
					x22 = xx[ayx_next].x2;
					if (x11 < x1)
					{
						x11 = x1;
						nac = false;
					}
					if (x22 > x2)
					{
						x22 = x2;
						kon = false;
					}
				}
				if (x11 > x1) // вертикальные хвостики
				{
					while (true)
					{
						// поиск рисуемой палочки
						bool vepa = false;
						while (ax <= rr)
						{
							xxxyyy* xxax = &(xx[ax]);
							uint    ii, iii;
							if (ax & rr_1)
							{
								if (ay & rr_1)
								{
									ii  = (xxax->ii + yyay_1->ii) >> xxax->sm1;
									iii = (xxax->ii + yyay->ii) >> xxax->sm1;
								}
								else
								{
									if (ay)
										ii = (xxax->ii + yyay_1->ii) >> xxax->sm1;
									else
										ii = (xxax->ii + yyay->ii) >> xxax->sm1;
									iii = ii;
								}
							}
							else
							{
								ii  = 4 + 2 * (ax >> rf0); // первая и последняя палочка
								iii = ii;
							}
							if (xxax->f[ii >> 3] & (uchar(1) << (ii & 7)))
							{
								if (ay) vepa = true;
								break;
							}
							if (xxax->f[iii >> 3] & (uchar(1) << (iii & 7))) break;
							ax++;
						}
						if (ax > rr) break;
						xxxyyy* xxax = &(xx[ax]);
						int64 xx11 = xxax->x1;
						if ((xx11 >= x11) || (xx11 > x2)) break;
						int64 xx22 = xxax->x2;
						uchar* cc   = (uchar*)&(data[y * size.x + xx11]);
						if (vepa)
						{
							cc[0] = (cc[0] * xxax->kk_1 + xxax->d1_1) >> 8;
							cc[1] = (cc[1] * xxax->kk_1 + xxax->d2_1) >> 8;
							cc[2] = (cc[2] * xxax->kk_1 + xxax->d3_1) >> 8;
							cc += 4;
							int64 d = xx11 - xx22 + 2;
							if (d <= 1)
							{
								while (d++ <= 0)
								{
									cc[0] = (cc[0] * kk + d1) >> 8;
									cc[1] = (cc[1] * kk + d2) >> 8;
									cc[2] = (cc[2] * kk + d3) >> 8;
									cc += 4;
								}
								cc[0] = (cc[0] * xxax->kk_2 + xxax->d1_2) >> 8;
								cc[1] = (cc[1] * xxax->kk_2 + xxax->d2_2) >> 8;
								cc[2] = (cc[2] * xxax->kk_2 + xxax->d3_2) >> 8;
							}
						}
						else
						{
							uint k3   = ((256 - xxax->kk_1) * (256 - kkw) / k2);
							uint kkw2 = 256 - k3;
							cc[0]     = (cc[0] * kkw2 + c_1 * k3) >> 8;
							cc[1]     = (cc[1] * kkw2 + c_2 * k3) >> 8;
							cc[2]     = (cc[2] * kkw2 + c_3 * k3) >> 8;
							cc += 4;
							int64 d = xx11 - xx22 + 2;
							if (d <= 1)
							{
								while (d++ <= 0)
								{
									cc[0] = (cc[0] * kkw + d1w) >> 8;
									cc[1] = (cc[1] * kkw + d2w) >> 8;
									cc[2] = (cc[2] * kkw + d3w) >> 8;
									cc += 4;
								}
								k3    = ((256 - xxax->kk_2) * (256 - kkw) / k2);
								kkw2  = 256 - k3;
								cc[0] = (cc[0] * kkw2 + c_1 * k3) >> 8;
								cc[1] = (cc[1] * kkw2 + c_2 * k3) >> 8;
								cc[2] = (cc[2] * kkw2 + c_3 * k3) >> 8;
							}
						}
						ax++;
					}
				}
				if (x11 > x2) break;
				uchar* cc = (uchar*)&(data[y * size.x + x11]);
				while (x11 <= x22)
				{
					// поиск рисуемой палочки
					if (ay == 0)
						ax = ayx_next + 1;
					else
						while (ax <= ayx_next)
						{
							xxxyyy* xxax = &(xx[ax]);
							uint    ii;
							if (ax & rr_1)
								ii = (xxax->ii + yyay_1->ii) >> xxax->sm1;
							else
								ii = 4 + 2 * (ax >> rf0); // первая и последняя палочка
							if (xxax->f[ii >> 3] & (uchar(1) << (ii & 7))) break;
							ax++;
						}
					xxxyyy* xxax = 0;
					int64 xx11, xx22;
					if (ax <= rr)
					{
						xxax = &(xx[ax]);
						xx11 = xxax->x1;
						if (xx11 > x22) xx11 = x22 + 1;
						xx22 = xxax->x2;
					}
					else
					{
						xx11 = x22 + 1;
					}
					if (xx11 > x11)
					{
						int64 d = x11 - xx11 + 1;
						if (nac)
						{
							nac = false;
							d++;
							uint k3   = ((256 - xx[ayx].kk_1) * (256 - kkw) / k2);
							uint kkw2 = 256 - k3;
							cc[0]     = (cc[0] * kkw2 + c_1 * k3) >> 8;
							cc[1]     = (cc[1] * kkw2 + c_2 * k3) >> 8;
							cc[2]     = (cc[2] * kkw2 + c_3 * k3) >> 8;
							cc += 4;
						}
						if (xx11 <= x22)
						{
							while (d++ <= 0)
							{
								cc[0] = (cc[0] * kkw + d1w) >> 8;
								cc[1] = (cc[1] * kkw + d2w) >> 8;
								cc[2] = (cc[2] * kkw + d3w) >> 8;
								cc += 4;
							}
						}
						else
						{
							if (d <= 0)
							{
								if (kon) d++;
								while (d++ <= 0)
								{
									cc[0] = (cc[0] * kkw + d1w) >> 8;
									cc[1] = (cc[1] * kkw + d2w) >> 8;
									cc[2] = (cc[2] * kkw + d3w) >> 8;
									cc += 4;
								}
								if (kon)
								{
									uint k3   = ((256 - xx[ayx_next].kk_2) * (256 - kkw) / k2);
									uint kkw2 = 256 - k3;
									cc[0]     = (cc[0] * kkw2 + c_1 * k3) >> 8;
									cc[1]     = (cc[1] * kkw2 + c_2 * k3) >> 8;
									cc[2]     = (cc[2] * kkw2 + c_3 * k3) >> 8;
								}
							}
							break;
						}
					}
					uint sl1, sl2, k3, kkw2;
					sl2 = 256 - kkw;
					if (nac)
					{
						nac   = false;
						cc[0] = (cc[0] * xxax->kk_1 + xxax->d1_1) >> 8;
						cc[1] = (cc[1] * xxax->kk_1 + xxax->d2_1) >> 8;
						cc[2] = (cc[2] * xxax->kk_1 + xxax->d3_1) >> 8;
					}
					else
					{
						sl1   = 256 - xxax->kk_1;
						k3    = sl1 + sl2 - sl1 * sl2 / k2;
						kkw2  = 256 - k3;
						cc[0] = (cc[0] * kkw2 + c_1 * k3) >> 8;
						cc[1] = (cc[1] * kkw2 + c_2 * k3) >> 8;
						cc[2] = (cc[2] * kkw2 + c_3 * k3) >> 8;
					}
					cc += 4;
					int64 d = xx11 - xx22 + 2;
					if (d <= 1)
					{
						while (d++ <= 0)
						{
							cc[0] = (cc[0] * kk + d1) >> 8;
							cc[1] = (cc[1] * kk + d2) >> 8;
							cc[2] = (cc[2] * kk + d3) >> 8;
							cc += 4;
						}
						if ((xx22 >= x22) && kon)
						{
							cc[0] = (cc[0] * xxax->kk_2 + xxax->d1_2) >> 8;
							cc[1] = (cc[1] * xxax->kk_2 + xxax->d2_2) >> 8;
							cc[2] = (cc[2] * xxax->kk_2 + xxax->d3_2) >> 8;
						}
						else
						{
							sl1   = 256 - xxax->kk_2;
							k3    = sl1 + sl2 - sl1 * sl2 / k2;
							kkw2  = 256 - k3;
							cc[0] = (cc[0] * kkw2 + c_1 * k3) >> 8;
							cc[1] = (cc[1] * kkw2 + c_2 * k3) >> 8;
							cc[2] = (cc[2] * kkw2 + c_3 * k3) >> 8;
						}
						cc += 4;
					}
					x11 = xx22 + 1;
					ax++;
				}
				if (x22 == x2) break;
				ayx = ayx_next;
			}
			continue;
		}
		if (y == (int)yyay->l2)
		{
			uint k3  = (uint)(k2 * (yyay->l2 - y));
			uint kkw = 256 - k3;
			uint d1w = c_1 * k3;
			uint d2w = c_2 * k3;
			uint d3w = c_3 * k3;
			while (true)
			{
				// поиск рисуемой палочки
				while (ayx < rr)
				{
					uint ii;
					if (ay & (rr_1))
						ii = (((xx[ayx].ii + yyay->ii) >> yyay->sm1) - 1) & 0xFFFFFFFE;
					else
						ii = 5 + 2 * (ay >> rf0); // первая и последняя палочка
					if (yyay->f[ii >> 3] & (uchar(1) << (ii & 7))) break;
					ayx += stepy;
				}
				int ayx_next = ayx + stepy; // следующая после серии палочек
				while (ayx_next < rr)
				{
					uint ii;
					if (ay & (rr_1))
						ii = (((xx[ayx_next].ii + yyay->ii) >> yyay->sm1) - 1) & 0xFFFFFFFE;
					else
						ii = 5 + 2 * (ay >> rf0); // первая и последняя палочка
					if ((yyay->f[ii >> 3] & (uchar(1) << (ii & 7))) == 0) break;
					ayx_next += stepy;
				}
				bool nac = true;
				bool kon = true;
				int64 x11, x22;
				if (ayx >= rr)
				{
					x11 = x2 + 1;
					x22 = x2; // ?? не нужно ??
				}
				else
				{
					x11 = xx[ayx].x1;
					x22 = xx[ayx_next].x2;
					if (x11 < x1)
					{
						x11 = x1;
						nac = false;
					}
					if (x22 > x2)
					{
						x22 = x2;
						kon = false;
					}
				}
				if (x11 > x1) // вертикальные хвостики
				{
					while (true)
					{
						// поиск рисуемой палочки
						bool nipa = false;
						while (ax <= rr)
						{
							xxxyyy* xxax = &(xx[ax]);
							uint    ii, iii;
							if (ax & rr_1)
							{
								if (ay & rr_1)
								{
									ii  = (xxax->ii + yyay_1->ii) >> xxax->sm1;
									iii = (xxax->ii + yyay->ii) >> xxax->sm1;
								}
								else
								{
									if (ay)
										ii = (xxax->ii + yyay_1->ii) >> xxax->sm1;
									else
										ii = (xxax->ii + yyay->ii) >> xxax->sm1;
									iii = ii;
								}
							}
							else
							{
								ii  = 4 + 2 * (ax >> rf0); // первая и последняя палочка
								iii = ii;
							}
							if (xxax->f[iii >> 3] & (uchar(1) << (iii & 7)))
							{
								if (ay < rr) nipa = true;
								break;
							}
							if (xxax->f[ii >> 3] & (uchar(1) << (ii & 7))) break;
							ax++;
						}
						if (ax > rr) break;
						xxxyyy* xxax = &(xx[ax]);
						int64 xx11 = xxax->x1;
						if ((xx11 >= x11) || (xx11 > x2)) break;
						int64 xx22 = xxax->x2;
						uchar* cc   = (uchar*)&(data[y * size.x + xx11]);
						if (nipa)
						{
							cc[0] = (cc[0] * xxax->kk_1 + xxax->d1_1) >> 8;
							cc[1] = (cc[1] * xxax->kk_1 + xxax->d2_1) >> 8;
							cc[2] = (cc[2] * xxax->kk_1 + xxax->d3_1) >> 8;
							cc += 4;
							int64 d = xx11 - xx22 + 2;
							if (d <= 1)
							{
								while (d++ <= 0)
								{
									cc[0] = (cc[0] * kk + d1) >> 8;
									cc[1] = (cc[1] * kk + d2) >> 8;
									cc[2] = (cc[2] * kk + d3) >> 8;
									cc += 4;
								}
								cc[0] = (cc[0] * xxax->kk_2 + xxax->d1_2) >> 8;
								cc[1] = (cc[1] * xxax->kk_2 + xxax->d2_2) >> 8;
								cc[2] = (cc[2] * xxax->kk_2 + xxax->d3_2) >> 8;
							}
						}
						else
						{
							uint k3   = ((256 - xxax->kk_1) * (256 - kkw) / k2);
							uint kkw2 = 256 - k3;
							cc[0]     = (cc[0] * kkw2 + c_1 * k3) >> 8;
							cc[1]     = (cc[1] * kkw2 + c_2 * k3) >> 8;
							cc[2]     = (cc[2] * kkw2 + c_3 * k3) >> 8;
							cc += 4;
							int64 d = xx11 - xx22 + 2;
							if (d <= 1)
							{
								while (d++ <= 0)
								{
									cc[0] = (cc[0] * kkw + d1w) >> 8;
									cc[1] = (cc[1] * kkw + d2w) >> 8;
									cc[2] = (cc[2] * kkw + d3w) >> 8;
									cc += 4;
								}
								k3    = ((256 - xxax->kk_2) * (256 - kkw) / k2);
								kkw2  = 256 - k3;
								cc[0] = (cc[0] * kkw2 + c_1 * k3) >> 8;
								cc[1] = (cc[1] * kkw2 + c_2 * k3) >> 8;
								cc[2] = (cc[2] * kkw2 + c_3 * k3) >> 8;
							}
						}
						ax++;
					}
				}
				if (x11 > x2) break;
				uchar* cc = (uchar*)&(data[y * size.x + x11]);
				while (x11 <= x22)
				{
					// поиск рисуемой палочки
					if (ay == rr)
						ax = ayx_next + 1;
					else
						while (ax <= ayx_next)
						{
							xxxyyy* xxax = &(xx[ax]);
							uint    ii;
							if (ax & rr_1)
								ii = (xxax->ii + yyay->ii) >> xxax->sm1;
							else
								ii = 4 + 2 * (ax >> rf0); // первая и последняя палочка
							if (xxax->f[ii >> 3] & (uchar(1) << (ii & 7))) break;
							ax++;
						}
					xxxyyy* xxax = 0;
					int64 xx11, xx22 = 0; // для параноии компилятора
					if (ax <= rr)
					{
						xxax = &(xx[ax]);
						xx11 = xxax->x1;
						if (xx11 > x22) xx11 = x22 + 1;
						xx22 = xxax->x2;
					}
					else
					{
						xx11 = x22 + 1;
					}
					if (xx11 > x11)
					{
						int64 d = x11 - xx11 + 1;
						if (nac)
						{
							nac = false;
							d++;
							uint k3   = ((256 - xx[ayx].kk_1) * (256 - kkw) / k2);
							uint kkw2 = 256 - k3;
							cc[0]     = (cc[0] * kkw2 + c_1 * k3) >> 8;
							cc[1]     = (cc[1] * kkw2 + c_2 * k3) >> 8;
							cc[2]     = (cc[2] * kkw2 + c_3 * k3) >> 8;
							cc += 4;
						}
						if (xx11 <= x22)
						{
							while (d++ <= 0)
							{
								cc[0] = (cc[0] * kkw + d1w) >> 8;
								cc[1] = (cc[1] * kkw + d2w) >> 8;
								cc[2] = (cc[2] * kkw + d3w) >> 8;
								cc += 4;
							}
						}
						else
						{
							if (d <= 0)
							{
								if (kon) d++;
								while (d++ <= 0)
								{
									cc[0] = (cc[0] * kkw + d1w) >> 8;
									cc[1] = (cc[1] * kkw + d2w) >> 8;
									cc[2] = (cc[2] * kkw + d3w) >> 8;
									cc += 4;
								}
								if (kon)
								{
									uint k3   = ((256 - xx[ayx_next].kk_2) * (256 - kkw) / k2);
									uint kkw2 = 256 - k3;
									cc[0]     = (cc[0] * kkw2 + c_1 * k3) >> 8;
									cc[1]     = (cc[1] * kkw2 + c_2 * k3) >> 8;
									cc[2]     = (cc[2] * kkw2 + c_3 * k3) >> 8;
								}
							}
							break;
						}
					}
					uint sl1, sl2, k3, kkw2;
					sl2 = 256 - kkw;
					if (xxax == nullptr) break; // для параноии компилятора
					if (nac)
					{
						nac   = false;
						cc[0] = (cc[0] * xxax->kk_1 + xxax->d1_1) >> 8;
						cc[1] = (cc[1] * xxax->kk_1 + xxax->d2_1) >> 8;
						cc[2] = (cc[2] * xxax->kk_1 + xxax->d3_1) >> 8;
					}
					else
					{
						sl1   = 256 - xxax->kk_1;
						k3    = sl1 + sl2 - sl1 * sl2 / k2;
						kkw2  = 256 - k3;
						cc[0] = (cc[0] * kkw2 + c_1 * k3) >> 8;
						cc[1] = (cc[1] * kkw2 + c_2 * k3) >> 8;
						cc[2] = (cc[2] * kkw2 + c_3 * k3) >> 8;
					}
					cc += 4;
					int64 d = xx11 - xx22 + 2;
					if (d <= 1)
					{
						while (d++ <= 0)
						{
							cc[0] = (cc[0] * kk + d1) >> 8;
							cc[1] = (cc[1] * kk + d2) >> 8;
							cc[2] = (cc[2] * kk + d3) >> 8;
							cc += 4;
						}
						if ((xx22 >= x22) && kon)
						{
							cc[0] = (cc[0] * xxax->kk_2 + xxax->d1_2) >> 8;
							cc[1] = (cc[1] * xxax->kk_2 + xxax->d2_2) >> 8;
							cc[2] = (cc[2] * xxax->kk_2 + xxax->d3_2) >> 8;
						}
						else
						{
							sl1   = 256 - xxax->kk_2;
							k3    = sl1 + sl2 - sl1 * sl2 / k2;
							kkw2  = 256 - k3;
							cc[0] = (cc[0] * kkw2 + c_1 * k3) >> 8;
							cc[1] = (cc[1] * kkw2 + c_2 * k3) >> 8;
							cc[2] = (cc[2] * kkw2 + c_3 * k3) >> 8;
						}
						cc += 4;
					}
					x11 = xx22 + 1;
					ax++;
				}
				if (x22 == x2) break;
				ayx = ayx_next;
			}
			continue;
		}
		while (true)
		{
			// поиск рисуемой палочки
			while (ayx < rr)
			{
				uint ii;
				if (ay & (rr_1))
					ii = (((xx[ayx].ii + yyay->ii) >> yyay->sm1) - 1) & 0xFFFFFFFE;
				else
					ii = 5 + 2 * (ay >> rf0); // первая и последняя палочка
				if (yyay->f[ii >> 3] & (uchar(1) << (ii & 7))) break;
				ayx += stepy;
			}
			int ayx_next = ayx + stepy; // следующая после серии палочек
			while (ayx_next < rr)
			{
				uint ii;
				if (ay & (rr_1))
					ii = (((xx[ayx_next].ii + yyay->ii) >> yyay->sm1) - 1) & 0xFFFFFFFE;
				else
					ii = 5 + 2 * (ay >> rf0); // первая и последняя палочка
				if ((yyay->f[ii >> 3] & (uchar(1) << (ii & 7))) == 0) break;
				ayx_next += stepy;
			}
			bool nac = true;
			bool kon = true;
			int64 x11, x22;
			if (ayx >= rr)
			{
				x11 = x2 + 1;
				x22 = x2; // ?? не нужно ??
			}
			else
			{
				x11 = xx[ayx].x1;
				x22 = xx[ayx_next].x2;
				if (x11 < x1)
				{
					x11 = x1;
					nac = false;
				}
				if (x22 > x2)
				{
					x22 = x2;
					kon = false;
				}
			}
			if (x11 > x1) // вертикальные хвостики x222
			{
				while (true)
				{
					// поиск рисуемой палочки
					while (ax <= rr)
					{
						xxxyyy* xxax = &(xx[ax]);
						uint    ii, iii;
						if (ax & rr_1)
						{
							if (ay & rr_1)
							{
								ii  = (xxax->ii + yyay_1->ii) >> xxax->sm1;
								iii = (xxax->ii + yyay->ii) >> xxax->sm1;
							}
							else
							{
								if (ay)
									ii = (xxax->ii + yyay_1->ii) >> xxax->sm1;
								else
									ii = (xxax->ii + yyay->ii) >> xxax->sm1;
								iii = ii;
							}
						}
						else
						{
							ii  = 4 + 2 * (ax >> rf0); // первая и последняя палочка
							iii = ii;
						}
						if (xxax->f[ii >> 3] & (uchar(1) << (ii & 7))) break;
						if (xxax->f[iii >> 3] & (uchar(1) << (iii & 7))) break;
						ax++;
					}
					if (ax > rr) break;
					xxxyyy* xxax = &(xx[ax]);
					int64 xx11 = xxax->x1;
					if ((xx11 >= x11) || (xx11 > x2)) break;
					int64 xx22 = xxax->x2;
					uchar* cc   = (uchar*)&(data[y * size.x + xx11]);
					cc[0]       = (cc[0] * xxax->kk_1 + xxax->d1_1) >> 8;
					cc[1]       = (cc[1] * xxax->kk_1 + xxax->d2_1) >> 8;
					cc[2]       = (cc[2] * xxax->kk_1 + xxax->d3_1) >> 8;
					cc += 4;
					int64 d = xx11 - xx22 + 2;
					if (d <= 1)
					{
						while (d++ <= 0)
						{
							cc[0] = (cc[0] * kk + d1) >> 8;
							cc[1] = (cc[1] * kk + d2) >> 8;
							cc[2] = (cc[2] * kk + d3) >> 8;
							cc += 4;
						}
						cc[0] = (cc[0] * xxax->kk_2 + xxax->d1_2) >> 8;
						cc[1] = (cc[1] * xxax->kk_2 + xxax->d2_2) >> 8;
						cc[2] = (cc[2] * xxax->kk_2 + xxax->d3_2) >> 8;
					}
					ax++;
				}
			}
			if (x11 > x2) break;
			uchar*  cc   = (uchar*)&(data[y * size.x + x11]);
			int64 d = x11 - x22 + 1;
			xxxyyy* xxax = &(xx[ayx]);
			if (nac)
			{
				d++;
				cc[0] = (cc[0] * xxax->kk_1 + xxax->d1_1) >> 8;
				cc[1] = (cc[1] * xxax->kk_1 + xxax->d2_1) >> 8;
				cc[2] = (cc[2] * xxax->kk_1 + xxax->d3_1) >> 8;
				cc += 4;
			}
			if (d <= 1)
			{
				if (!kon) d--;
				while (d++ <= 0)
				{
					cc[0] = (cc[0] * kk + d1) >> 8;
					cc[1] = (cc[1] * kk + d2) >> 8;
					cc[2] = (cc[2] * kk + d3) >> 8;
					cc += 4;
				}
				if (kon)
				{
					xxax  = &(xx[ayx_next]);
					cc[0] = (cc[0] * xxax->kk_2 + xxax->d1_2) >> 8;
					cc[1] = (cc[1] * xxax->kk_2 + xxax->d2_2) >> 8;
					cc[2] = (cc[2] * xxax->kk_2 + xxax->d3_2) >> 8;
				}
			}
			if (x22 == x2) break;
			ayx = ayx_next;
			ax  = ayx + 1;
		}
	}
}

void _picture::rectangle(_iarea oo, uint c)
{
	if (oo.empty()) return;
	line({ oo.x.min, oo.y.min }, { oo.x.max - 1, oo.y.min }, c);
	if (oo.y.max - oo.y.min == 1) return;
	line({ oo.x.min, oo.y.max - 1 }, { oo.x.max - 1, oo.y.max - 1 }, c);
	if (oo.y.max - oo.y.min > 2)
	{ // такие заморочки из за углов
		line({ oo.x.min, oo.y.min + 1 }, { oo.x.min, oo.y.max - 2 }, c);
		line({ oo.x.max - 1, oo.y.min + 1 }, { oo.x.max - 1, oo.y.max - 2 }, c);
	}
}

_stack& operator<<(_stack& o, _picture const& p)
{
	o << p.size << p.transparent;
	o.push_data(p.data, 4 * p.size.square());
	return o;
}

_stack& operator>>(_stack& o, _picture& p)
{
	_isize r;
	o >> r >> p.transparent;
	p.resize(r);
	o.pop_data(p.data, 4 * p.size.square());
	return o;
}

void _picture::fill_rect_d(double x1, double y1, double x2, double y2, uint c)
{
	uint kk = 255 - (c >> 24);
	if (kk == 0xFF) return; // полностью прозрачная
	if (x1 > x2) std::swap(x1, x2);
	if (y1 > y2) std::swap(y1, y2);
	if (x1 < area.x.min) x1 = (double)area.x.min;
	if (y1 < area.y.min) y1 = (double)area.y.min;
	if (x2 >= area.x.max) x2 = area.x.max - 0.0001;
	if (y2 >= area.y.max) y2 = area.y.max - 0.0001;
	if ((x2 - x1 < 0.004) || (y2 - y1 < 0.004)) return; // слишком тонкий или за пределами

	int64 xx1 = (int64)x1;
	int64 xx2 = (int64)x2;
	int64 yy1 = (int64)y1;
	int64 yy2 = (int64)y2;
	uint r1  = (c & 255);
	uint r2  = ((c >> 8) & 255);
	uint r3  = ((c >> 16) & 255);

	if (yy1 == yy2) // горизонтальная линия
	{
		uchar* c2 = px(xx1, yy1);
		if (xx1 == xx2) // точка
		{
			uint k2 = (uint)((256 - kk) * (x2 - x1) * (y2 - y1));
			kk      = 256 - k2;
			c2[0]   = (c2[0] * kk + r1 * k2) >> 8;
			c2[1]   = (c2[1] * kk + r2 * k2) >> 8;
			c2[2]   = (c2[2] * kk + r3 * k2) >> 8;
			return;
		}
		uint k2 = (uint)((256 - kk) * (y2 - y1));

		uint kk2 = (uint)(k2 * (xx1 + 1.0 - x1));
		uint kkk = 256 - kk2;
		c2[0]    = (c2[0] * kkk + r1 * kk2) >> 8;
		c2[1]    = (c2[1] * kkk + r2 * kk2) >> 8;
		c2[2]    = (c2[2] * kkk + r3 * kk2) >> 8;
		c2 += 4;

		kk      = 256 - k2;
		uint d1 = r1 * k2;
		uint d2 = r2 * k2;
		uint d3 = r3 * k2;
		int64 d  = xx1 - xx2 + 2;
		while (d <= 0)
		{
			c2[0] = (c2[0] * kk + d1) >> 8;
			c2[1] = (c2[1] * kk + d2) >> 8;
			c2[2] = (c2[2] * kk + d3) >> 8;
			c2 += 4;
			d++;
		}

		kk2   = (uint)(k2 * (x2 - xx2));
		kkk   = 256 - kk2;
		c2[0] = (c2[0] * kkk + r1 * kk2) >> 8;
		c2[1] = (c2[1] * kkk + r2 * kk2) >> 8;
		c2[2] = (c2[2] * kkk + r3 * kk2) >> 8;

		return;
	}
	if (xx1 == xx2) // вертикальная линия
	{
		uint k2 = (uint)((256 - kk) * (x2 - x1));

		uchar* c2  = px(xx1, yy1);
		uint   kk2 = (uint)(k2 * (yy1 + 1.0 - y1));
		uint   kkk = 256 - kk2;
		c2[0]      = (c2[0] * kkk + r1 * kk2) >> 8;
		c2[1]      = (c2[1] * kkk + r2 * kk2) >> 8;
		c2[2]      = (c2[2] * kkk + r3 * kk2) >> 8;

		kk      = 256 - k2;
		uint d1 = r1 * k2;
		uint d2 = r2 * k2;
		uint d3 = r3 * k2;
		for (int64 y = yy1 + 1; y < yy2; y++)
		{
			c2    = px(xx1, y);
			c2[0] = (c2[0] * kk + d1) >> 8;
			c2[1] = (c2[1] * kk + d2) >> 8;
			c2[2] = (c2[2] * kk + d3) >> 8;
		}

		c2    = px(xx1, yy2);
		kk2   = (uint)(k2 * (y2 - yy2));
		kkk   = 256 - kk2;
		c2[0] = (c2[0] * kkk + r1 * kk2) >> 8;
		c2[1] = (c2[1] * kkk + r2 * kk2) >> 8;
		c2[2] = (c2[2] * kkk + r3 * kk2) >> 8;

		return;
	}
	uint64 cc = (((uint64)c) << 32) + c;
	for (int64 y = yy1; y <= yy2; y++)
	{
		uchar* c2 = px(xx1, y);

		uint k2 = (256 - kk);
		if (y == yy1) k2 = (uint)(k2 * (yy1 + 1.0 - y1));
		if (y == yy2) k2 = (uint)(k2 * (y2 - yy2));

		uint kk2 = (uint)(k2 * (xx1 + 1.0 - x1));
		uint kkk = 256 - kk2;
		c2[0]    = (c2[0] * kkk + r1 * kk2) >> 8;
		c2[1]    = (c2[1] * kkk + r2 * kk2) >> 8;
		c2[2]    = (c2[2] * kkk + r3 * kk2) >> 8;
		c2 += 4;

		if (k2 >= 255)
		{
			uint64* ee    = (uint64*)c2;
			uint64* eemax = (uint64*)px(xx2 - 1, y);
			while (ee < eemax) *ee++ = cc;
			if (ee == eemax) *((uint*)ee) = c;
		}
		else
		{
			kkk     = 256 - k2;
			uint d1 = r1 * k2;
			uint d2 = r2 * k2;
			uint d3 = r3 * k2;
			int64  d  = xx1 - xx2 + 2;
			while (d <= 0)
			{
				c2[0] = (c2[0] * kkk + d1) >> 8;
				c2[1] = (c2[1] * kkk + d2) >> 8;
				c2[2] = (c2[2] * kkk + d3) >> 8;
				c2 += 4;
				d++;
			}
		}

		c2    = px(xx2, y);
		kk2   = (uint)(k2 * (x2 - xx2));
		kkk   = 256 - kk2;
		c2[0] = (c2[0] * kkk + r1 * kk2) >> 8;
		c2[1] = (c2[1] * kkk + r2 * kk2) >> 8;
		c2[2] = (c2[2] * kkk + r3 * kk2) >> 8;
	}
}

void _picture::fill_ring(_xy p, double r, double d, uint c, uint c2)
{
	if (r < 0.5) return; // слишком маленький
	if (c == c2)
	{
		fill_circle(p, r, c);
		return;
	}
	uint kk_ = 255 - (c2 >> 24);
	if (kk_ == 0xFF) // полностью прозрачная внутренность
	{
		ring(p, r, d, c);
		return;
	}
	double r2 = r - d;
	if (r2 < 0) r2 = 0;
	int64 y1 = (int64)(p.y - r);
	y1     = std::max(area.y.min, y1);
	int64 y2 = (int64)(p.y + r);
	y2     = std::min(area.y.max - 1, y2);
	int64 x1 = (int64)(p.x - r);
	x1     = std::max(area.x.min, x1);
	int64 x2 = (int64)(p.x + r);
	x2     = std::min(area.x.max - 1, x2);
	if ((x2 < x1) || (y2 < y1)) return;
	double rrmin = (r - 0.5) * (r - 0.5);
	double rrmax = (r + 0.5) * (r + 0.5);
	double drr   = rrmax - rrmin;
	double ddmin = (r2 - 0.5) * (r2 - 0.5);
	double ddmax = (r2 + 0.5) * (r2 + 0.5);
	double ddd   = ddmax - ddmin;
	p.x -= 0.5;
	p.y -= 0.5;
	double dxdx0 = (x1 - p.x) * (x1 - p.x);
	double ab0 = 2 * (x1 - p.x) + 1;
	uint kk = 255 - (c >> 24);
	uint k2 = 256 - kk;
	uint c_0 = (c & 255);
	uint c_1 = ((c >> 8) & 255);
	uint c_2 = ((c >> 16) & 255);
	uint d1 = c_0 * k2;
	uint d2 = c_1 * k2;
	uint d3 = c_2 * k2;
	uint k2_ = 256 - kk_;
	uint c2_0 = (c2 & 255);
	uint c2_1 = ((c2 >> 8) & 255);
	uint c2_2 = ((c2 >> 16) & 255);
	uint dd1 = c2_0 * k2_;
	uint dd2 = c2_1 * k2_;
	uint dd3 = c2_2 * k2_;
	if ((kk == 0) && (kk_ == 0))
	{
		for (int64 i = y1; i <= y2; i++)
		{
			double dd = (i - p.y) * (i - p.y) + dxdx0;
			double ab = ab0;
			uchar* cc = px(x1, i);
			int64    d  = x1 - x2;
			while (d++ <= 0)
			{
				if (dd < rrmax)
				{
					if (dd <= ddmin) { *((uint*)cc) = c2; }
					else if (dd > rrmin)
					{
						if (dd >= ddmax)
						{
							uint k22 = (uint)(k2 * (rrmax - dd) / drr);
							uint kk2 = 256 - k22;
							cc[0]    = (cc[0] * kk2 + c_0 * k22) >> 8;
							cc[1]    = (cc[1] * kk2 + c_1 * k22) >> 8;
							cc[2]    = (cc[2] * kk2 + c_2 * k22) >> 8;
						}
						else
						{ // тонкое кольцо
							double m1  = (ddmax - dd) / ddd;
							uint   k22 = (uint)(k2_ * m1);
							uint   k33 = (uint)(k2 * ((rrmax - dd) / drr - m1));
							uint   kk2 = 256 - k22 - k33;
							cc[0]      = (cc[0] * kk2 + c2_0 * k22 + c_0 * k33) >> 8;
							cc[1]      = (cc[1] * kk2 + c2_1 * k22 + c_1 * k33) >> 8;
							cc[2]      = (cc[2] * kk2 + c2_2 * k22 + c_2 * k33) >> 8;
						}
					}
					else if (dd >= ddmax)
					{
						*((uint*)cc) = c;
					}
					else
					{
						double m1  = (ddmax - dd) / ddd;
						uint   k22 = (uint)(k2_ * m1);
						uint   k33 = (uint)(k2 * (1 - m1));
						uint   kk2 = 256 - k22 - k33;
						cc[0]      = (cc[0] * kk2 + c2_0 * k22 + c_0 * k33) >> 8;
						cc[1]      = (cc[1] * kk2 + c2_1 * k22 + c_1 * k33) >> 8;
						cc[2]      = (cc[2] * kk2 + c2_2 * k22 + c_2 * k33) >> 8;
					}
				}
				cc += 4;
				dd += ab;
				ab += 2;
			}
		}
		return;
	}
	for (int64 i = y1; i <= y2; i++)
	{
		double dd = (i - p.y) * (i - p.y) + dxdx0;
		double ab = ab0;
		uchar* cc = px(x1, i);
		int64 d  = x1 - x2;
		while (d++ <= 0)
		{
			if (dd < rrmax)
			{
				if (dd <= ddmin)
				{
					cc[0] = (cc[0] * kk_ + dd1) >> 8;
					cc[1] = (cc[1] * kk_ + dd2) >> 8;
					cc[2] = (cc[2] * kk_ + dd3) >> 8;
				}
				else if (dd > rrmin)
				{
					if (dd >= ddmax)
					{
						uint k22 = (uint)(k2 * (rrmax - dd) / drr);
						uint kk2 = 256 - k22;
						cc[0]    = (cc[0] * kk2 + c_0 * k22) >> 8;
						cc[1]    = (cc[1] * kk2 + c_1 * k22) >> 8;
						cc[2]    = (cc[2] * kk2 + c_2 * k22) >> 8;
					}
					else
					{ // тонкое кольцо
						double m1  = (ddmax - dd) / ddd;
						uint   k22 = (uint)(k2_ * m1);
						uint   k33 = (uint)(k2 * ((rrmax - dd) / drr - m1));
						uint   kk2 = 256 - k22 - k33;
						cc[0]      = (cc[0] * kk2 + c2_0 * k22 + c_0 * k33) >> 8;
						cc[1]      = (cc[1] * kk2 + c2_1 * k22 + c_1 * k33) >> 8;
						cc[2]      = (cc[2] * kk2 + c2_2 * k22 + c_2 * k33) >> 8;
					}
				}
				else if (dd >= ddmax)
				{
					cc[0] = (cc[0] * kk + d1) >> 8;
					cc[1] = (cc[1] * kk + d2) >> 8;
					cc[2] = (cc[2] * kk + d3) >> 8;
				}
				else
				{
					double m1  = (ddmax - dd) / ddd;
					uint   k22 = (uint)(k2_ * m1);
					uint   k33 = (uint)(k2 * (1 - m1));
					uint   kk2 = 256 - k22 - k33;
					cc[0]      = (cc[0] * kk2 + c2_0 * k22 + c_0 * k33) >> 8;
					cc[1]      = (cc[1] * kk2 + c2_1 * k22 + c_1 * k33) >> 8;
					cc[2]      = (cc[2] * kk2 + c2_2 * k22 + c_2 * k33) >> 8;
				}
			}
			cc += 4;
			dd += ab;
			ab += 2;
		}
	}
}

void _picture::ring(_xy p, double r, double d, uint c)
{
	if (r < 0.5) return; // слишком маленький
	double r2 = r - d;
	if (r2 < 0) r2 = 0;
	int64 y1 = (int)(p.y - r);
	y1     = std::max(area.y.min, y1);
	int64 y2 = (int)(p.y + r);
	y2     = std::min(area.y.max - 1, y2);
	int64 x1 = (int)(p.x - r);
	x1     = std::max(area.x.min, x1);
	int64 x2 = (int)(p.x + r);
	x2     = std::min(area.x.max - 1, x2);
	if ((x2 < x1) || (y2 < y1)) return;

	double rrmin = (r - 0.5) * (r - 0.5);
	double rrmax = (r + 0.5) * (r + 0.5);
	double drr   = rrmax - rrmin;
	double ddmin = (r2 - 0.5) * (r2 - 0.5);
	double ddmax = (r2 + 0.5) * (r2 + 0.5);
	double ddd   = ddmax - ddmin;

	double xxx2 = (size.x / 2 - p.x) * (size.x / 2 - p.x) + (size.y / 2 - p.y) * (size.y / 2 - p.y);
	double yyy2 = 0.25 * size.x * size.x + 0.25 * size.y * size.y;
	if (xxx2 + yyy2 + 2 * sqrt(xxx2 * yyy2) < ddmin) return; // экран внутри кольца

	p.x -= 0.5;
	p.y -= 0.5;
	double dxdx0 = (x1 - p.x) * (x1 - p.x);
	double ab0 = 2 * (x1 - p.x) + 1;
	uint kk = 255 - (c >> 24);
	uint k2 = 256 - kk;
	uint c_0 = (c & 255);
	uint c_1 = ((c >> 8) & 255);
	uint c_2 = ((c >> 16) & 255);
	uint d1 = c_0 * k2;
	uint d2 = c_1 * k2;
	uint d3 = c_2 * k2;
	if (kk == 0)
	{
		for (int64 i = y1; i <= y2; i++)
		{
			double dd = (i - p.y) * (i - p.y) + dxdx0;
			double ab = ab0;
			uchar* cc = px(x1, i);
			int64 d  = x1 - x2;
			while (d++ <= 0)
			{
				if ((dd < rrmax) && (dd > ddmin))
				{
					if (dd > rrmin)
					{
						if (dd >= ddmax)
						{
							uint k22 = (uint)(k2 * (rrmax - dd) / drr);
							uint kk2 = 256 - k22;
							cc[0]    = (cc[0] * kk2 + c_0 * k22) >> 8;
							cc[1]    = (cc[1] * kk2 + c_1 * k22) >> 8;
							cc[2]    = (cc[2] * kk2 + c_2 * k22) >> 8;
						}
						else
						{ // тонкое кольцо
							uint k22 = (uint)(k2 * ((rrmax - dd) / drr - (ddmax - dd) / ddd));
							uint kk2 = 256 - k22;
							cc[0]    = (cc[0] * kk2 + c_0 * k22) >> 8;
							cc[1]    = (cc[1] * kk2 + c_1 * k22) >> 8;
							cc[2]    = (cc[2] * kk2 + c_2 * k22) >> 8;
						}
					}
					else if (dd >= ddmax)
					{
						*((uint*)cc) = c;
					}
					else
					{
						uint k22 = (uint)(k2 * (dd - ddmin) / ddd);
						uint kk2 = 256 - k22;
						cc[0]    = (cc[0] * kk2 + c_0 * k22) >> 8;
						cc[1]    = (cc[1] * kk2 + c_1 * k22) >> 8;
						cc[2]    = (cc[2] * kk2 + c_2 * k22) >> 8;
					}
				}
				cc += 4;
				dd += ab;
				ab += 2;
			}
		}
		return;
	}
	for (int64 i = y1; i <= y2; i++)
	{
		double dd = (i - p.y) * (i - p.y) + dxdx0;
		double ab = ab0;
		uchar* cc = px(x1, i);
		int64 d  = x1 - x2;
		while (d++ <= 0)
		{
			if ((dd < rrmax) && (dd > ddmin))
			{
				if (dd > rrmin)
				{
					if (dd >= ddmax)
					{
						uint k22 = (uint)(k2 * (rrmax - dd) / drr);
						uint kk2 = 256 - k22;
						cc[0]    = (cc[0] * kk2 + c_0 * k22) >> 8;
						cc[1]    = (cc[1] * kk2 + c_1 * k22) >> 8;
						cc[2]    = (cc[2] * kk2 + c_2 * k22) >> 8;
					}
					else
					{ // тонкое кольцо
						uint k22 = (uint)(k2 * ((rrmax - dd) / drr - (ddmax - dd) / ddd));
						uint kk2 = 256 - k22;
						cc[0]    = (cc[0] * kk2 + c_0 * k22) >> 8;
						cc[1]    = (cc[1] * kk2 + c_1 * k22) >> 8;
						cc[2]    = (cc[2] * kk2 + c_2 * k22) >> 8;
					}
				}
				else if (dd >= ddmax)
				{
					cc[0] = (cc[0] * kk + d1) >> 8;
					cc[1] = (cc[1] * kk + d2) >> 8;
					cc[2] = (cc[2] * kk + d3) >> 8;
				}
				else
				{
					uint k22 = (uint)(k2 * (dd - ddmin) / ddd);
					uint kk2 = 256 - k22;
					cc[0]    = (cc[0] * kk2 + c_0 * k22) >> 8;
					cc[1]    = (cc[1] * kk2 + c_1 * k22) >> 8;
					cc[2]    = (cc[2] * kk2 + c_2 * k22) >> 8;
				}
			}
			cc += 4;
			dd += ab;
			ab += 2;
		}
	}
}

void _picture::fill_circle(_xy p, double r, uint c)
{
	if (r < 0.5) return; // слишком маленький
	int64 y1 = (int64)(p.y - r);
	y1     = std::max(area.y.min, y1);
	i8 y2 = (int64)(p.y + r);
	y2     = std::min(area.y.max - 1, y2);
	int64 x1 = (int64)(p.x - r);
	x1     = std::max(area.x.min, x1);
	i8 x2 = (int64)(p.x + r);
	x2     = std::min(area.x.max - 1, x2);
	if ((x2 < x1) || (y2 < y1)) return;

	double rrmin = (r - 0.5) * (r - 0.5);
	double rrmax = (r + 0.5) * (r + 0.5);
	double drr   = rrmax - rrmin;
	p.x -= 0.5;
	p.y -= 0.5;
	double dxdx0 = (x1 - p.x) * (x1 - p.x);
	double ab0 = 2 * (x1 - p.x) + 1;
	uint kk = 255 - (c >> 24);
	uint k2 = 256 - kk;
	uint c_0 = (c & 255);
	uint c_1 = ((c >> 8) & 255);
	uint c_2 = ((c >> 16) & 255);
	uint d1 = c_0 * k2;
	uint d2 = c_1 * k2;
	uint d3 = c_2 * k2;
	if (kk == 0)
	{
		for (int64 i = y1; i <= y2; i++)
		{
			double dd = (i - p.y) * (i - p.y) + dxdx0;
			double ab = ab0;
			uchar* cc = px(x1, i);
			int64 d  = x1 - x2;
			while (d++ <= 0)
			{
				if (dd < rrmax)
				{
					if (dd > rrmin)
					{
						uint k22 = (uint)(k2 * (rrmax - dd) / drr);
						uint kk2 = 256 - k22;
						cc[0]    = (cc[0] * kk2 + c_0 * k22) >> 8;
						cc[1]    = (cc[1] * kk2 + c_1 * k22) >> 8;
						cc[2]    = (cc[2] * kk2 + c_2 * k22) >> 8;
					}
					else
						*((uint*)cc) = c;
				}
				cc += 4;
				dd += ab;
				ab += 2;
			}
		}
		return;
	}
	for (int64 i = y1; i <= y2; i++)
	{
		double dd = (i - p.y) * (i - p.y) + dxdx0;
		double ab = ab0;
		uchar* cc = px(x1, i);
		int64 d  = x1 - x2;
		while (d++ <= 0)
		{
			if (dd < rrmax)
			{
				if (dd > rrmin)
				{
					uint k22 = (uint)(k2 * (rrmax - dd) / drr);
					uint kk2 = 256 - k22;
					cc[0]    = (cc[0] * kk2 + c_0 * k22) >> 8;
					cc[1]    = (cc[1] * kk2 + c_1 * k22) >> 8;
					cc[2]    = (cc[2] * kk2 + c_2 * k22) >> 8;
				}
				else
				{
					cc[0] = (cc[0] * kk + d1) >> 8;
					cc[1] = (cc[1] * kk + d2) >> 8;
					cc[2] = (cc[2] * kk + d3) >> 8;
				}
			}
			cc += 4;
			dd += ab;
			ab += 2;
		}
	}
}

void _hsva::operator=(uint c)
{
	double r  = ((c >> 16) & 0xFF) / 255.0;
	double g  = ((c >> 8) & 0xFF) / 255.0;
	double b  = (c & 0xFF) / 255.0;
	double ma = std::max(r, std::max(g, b));
	double mi = std::min(r, std::min(g, b));
	if (ma == mi)
		h = 180;
	else if (ma == r)
		h = 60.0 * (g - b) / (ma - mi) + 360.0 * (g < b);
	else if (ma == g)
		h = 60.0 * (b - r) / (ma - mi) + 120.0;
	else
		h = 60.0 * (r - g) / (ma - mi) + 240.0;
	if (ma == 0)
		s = 0;
	else
		s = 1.0 - mi / ma;
	v = ma;
	a = (c >> 24) / 255.0;
}

_hsva::operator uint()
{
	uint   hi   = ((int)(h / 60.0)) % 6;
	double vmin = (1.0 - s) * v;
	double aa   = (v - vmin) * ((((int)h) % 60) / 60.0);
	double vinc = vmin + aa;
	double vdec = v - aa;
	double r, g, b;
	switch (hi)
	{
	case 0:
		r = v;
		g = vinc;
		b = vmin;
		break;
	case 1:
		r = vdec;
		g = v;
		b = vmin;
		break;
	case 2:
		r = vmin;
		g = v;
		b = vinc;
		break;
	case 3:
		r = vmin;
		g = vdec;
		b = v;
		break;
	case 4:
		r = vinc;
		g = vmin;
		b = v;
		break;
	case 5:
		r = v;
		g = vmin;
		b = vdec;
		break;
	default: r = g = b = 1.0;
	}
	uint rr  = (uint)(r * 255);
	uint gg  = (uint)(g * 255);
	uint bb  = (uint)(b * 255);
	uint prr = (uint)(a * 255);
	return (prr << 24) + bb + (gg << 8) + (rr << 16);
}

uint brighten(uint c)
{
	uchar* cc = (uchar*)(&c);
	cc[0]     = (cc[0] <= 85) ? (cc[0] * 2) : (128 + cc[0] / 2);
	cc[1]     = (cc[1] <= 85) ? (cc[1] * 2) : (128 + cc[1] / 2);
	cc[2]     = (cc[2] <= 85) ? (cc[2] * 2) : (128 + cc[2] / 2);
	return c;
}
