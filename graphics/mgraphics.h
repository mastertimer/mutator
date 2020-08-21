﻿#pragma once

#define NOMINMAX
#include <windows.h>

#include "color.h"
#include "mgeometry.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct _picture
{
	uint* data;
	_size2i size;
	bool transparent = false; // как бы от него избавиться???

	explicit _picture(int rx3 = 0, int ry3 = 0);
	_picture(const _picture&) = delete;
	_picture(_picture&& move) noexcept;
	virtual ~_picture() { delete[] data; }

	_picture& operator=(_picture&& move) noexcept;

	_color& px(int64 x, int64 y) { return ((_color*)data)[y * size.x + x]; }

	void set_area(const _area2i &q) { area = q & size; }
	bool resize(int64 w, int64 h);
	void set_transparent(); // узнать, есть ли прозрачные пиксели
	void invert_alpha(); // инвертировать альфа канал

	void clear(uint c = 0xFF000000);
	void line(_number2 p1, _number2 p2, uint c, bool rep = false); // линия rep - полное замещение цвета
	void lines(_xy p1, _xy p2, double l, uint c); // точная линия заданной толщины
	void text16(int64 x, int64 y, const char* s, uint c); // простой текст высотой 16
	void text16n(int64 x, int64 y, const char* s, int64 n, uint c); // простой текст высотой 16*n
	static _size2i size_text16(std::string_view s, int64 n = 1); // размер текста *n
	void froglif(_coordinate2 p, double r, uchar* f, int rf, uint c, uint c2 = 0);

	void fill_circle(double x, double y, double r, uint c);
	void fill_ring(_xy p, double r, double d, uint c, uint c2);
	void ring(_xy p, double r, double d, uint c);
	void fill_ellipse(_area_old o, double d, uint c, uint c2);
	void fill_ellipse2(_area_old o, double d, uint c, uint c2);

	void fill_rectangle(_area2i r, uint c, bool rep = false);
	void fill_rect_d(double x1, double y1, double x2, double y2, uint c); // полупрозрачный пр-к на !!непр-й!! подложке

	void rectangle(_area2i oo, uint c);

	void draw(int64 nXDest, int64 nYDest, int64 nWidth, int64 nHeight, _picture* bm, int64 nXSrc = 0, int64 nYSrc = 0);
	void stretch_draw(_picture* bm, int64 x, int64 y, double m); // правильное растягивание, но без прозрачности!!
	void stretch_draw2(_picture* bm, int64 nXDest, int64 nYDest, double m); // тупое масштабирование, зато с прозрачностью!!
	void stretch_draw3(_picture* bm, int64 x, int64 y, double m); // нарисовать с масштабированием

//	void text0(int x, int y, std::string_view s, int h, uint c, uint bg);

protected:
	_area2i area; // разрешенная область для рисования

	void line_vert_rep_speed(_number2 p, _number y2, uint c); // вертикальная линия замещения без проверок диапазона

	void fill_rect_rep_speed(_area2i r, uint c); // прямоугольник - просто замена цвета без проверок диапазона
	void fill_rect_transparent_speed(_area2i r, uint c);
	void fill_rect_speed(_area2i r, uint c);
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct _bitmap: public _picture
{
#if TARGET_SYSTEM_WINDOWS
	HBITMAP bitmap2;
	HDC hdc = 0;

	LOGFONT font;
	bool izm_font = true;// шрифт был изменен
	HFONT hfont = 0;
	uint f_c = 0; // цвет шрифта
	uint f_cf = 0; // цвет фона шрифта

	void podg_font(int r); // подготовка шрифта в выводу
	void podg_cc(uint c, uint cf); // подготовка цветов к выводу
#endif

	explicit _bitmap(int rx3 = 0, int ry3 = 0);
	~_bitmap();

	bool resize(int64 w, int64 h);

	void text(int x, int y, std::wstring_view s, int h, _color c, _color bg);
	void text(int x, int y, std::string_view s, int h, _color c, _color bg);
	_size2i size_text(std::wstring_view s, int h);
	_size2i size_text(std::string_view s, int h);

private:
#if TARGET_SYSTEM_LINUX
	Display*      d           = 0;
	int32         screen      = 0;

	struct _font
	{
		XFontSet      set = 0;
		XFontStruct** a   = 0;
		int32         c   = 0;
	};
	std::map<int, _font> fonts;
	_font font;

	int32         h = 16;

	x::_shmpixmap img;
	GC           gc = {};

	void update_font(int h);             // подготовка шрифта в выводу

	bool need_uptext = true;
	void inittext();
#endif
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct _hsva // цвет в формате hsv
{
	double h; // цветовой тон
	double s; // насыщенность
	double v; // яркость
	double a; // прозрачность

	_hsva() = default;
	_hsva(uint c) { *this = c; }
	_hsva(const _hsva& c) = default;
	
	void operator = (uint c);
	_hsva& operator = (const _hsva& c) = default;
	operator uint();
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

extern _bitmap temp_bmp;

_stack& operator<<(_stack& o, _picture const& p);
_stack& operator>>(_stack& o, _picture& p);

inline uint cclow(uint c) {	return ((c >> 2) & 0x3F3F3F) + (c & 0xFF000000); }
uint brighten(uint c);

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
