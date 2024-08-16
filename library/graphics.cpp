#include "graphics.h" // 2732

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct _picture_functions : public _picture
{
	t_t void fill_rectangle3(_iarea r, _color c);
	template <typename _t, typename _b> void fill_circle3(_iarea area, _xy p, double r, _color c);
	template <typename _t, typename _b> void ring3(_iarea area, _xy p, double r, double r2, _color c);
	template <typename _t, typename _b> void fill_ring3(_iarea area, _xy p, double r, double r2, _color c, _color c2);
};

struct _color_substitution
{
	_color cc;
	_color_substitution(_color c): cc(c) {}
	void mix(_color& c) const { c = cc; }
	static void mix2(const _color cc, _color& c) { c = cc; }
};

struct _color_overlay
{
	uint kk;
	uint d_b;
	uint d_g;
	uint d_r;

	_color_overlay(_color c)
	{
		kk = 255 - c.a;
		uint k2 = uint(c.a) + 1;
		d_b = c.b * k2;
		d_g = c.g * k2;
		d_r = c.r * k2;
	}

	void mix(_color& c) const
	{
		c.b = (c.b * kk + d_b) >> 8;
		c.g = (c.g * kk + d_g) >> 8;
		c.r = (c.r * kk + d_r) >> 8;
	}

	static void mix2(const _color cc, _color& c)
	{
		uint kk = 255 - cc.a;
		uint k2 = uint(cc.a) + 1;
		c.b = (c.b * kk + cc.b * k2) >> 8;
		c.g = (c.g * kk + cc.g * k2) >> 8;
		c.r = (c.r * kk + cc.r * k2) >> 8;
	}

	static void mixmix2(const _color cc1, const _color cc2, _color& c)
	{
		uint k1 = cc1.a;
		uint k2 = cc2.a;
		uint kk = 256 - cc1.a - cc2.a;
		c.b = (c.b * kk + cc1.b * k1 + cc2.b * k2) >> 8;
		c.g = (c.g * kk + cc1.g * k1 + cc2.g * k2) >> 8;
		c.r = (c.r * kk + cc1.r * k1 + cc2.r * k2) >> 8;
	}
};

struct _color_mixing
{
	uint kk;
	uint d_b;
	uint d_g;
	uint d_r;

	_color_mixing(_color c)
	{
		kk = 255 - c.a;
		uint k2 = (uint(c.a) + 1) * 255;
		d_b = c.b * k2;
		d_g = c.g * k2;
		d_r = c.r * k2;
	}

	void mix(_color& c) const
	{
		uint kk_ = 255 - c.a;
		uint k2_ = (uint(c.a) + 1) * kk;
		uint znam = 65535 - kk * kk_;
		c.b = (c.b * k2_ + d_b) / znam;
		c.g = (c.g * k2_ + d_g) / znam;
		c.r = (c.r * k2_ + d_r) / znam;
		c.a = 255 - ((kk_ * kk) >> 8);
	}

	static void mix2(const _color cc, _color& c)
	{
		uint kk = 255 - cc.a;
		uint k2 = (uint(cc.a) + 1) * 255;
		uint kk_ = 255 - c.a;
		uint k2_ = (uint(c.a) + 1) * kk;
		uint znam = 65535 - kk * kk_;
		c.b = (c.b * k2_ + cc.b * k2) / znam;
		c.g = (c.g * k2_ + cc.g * k2) / znam;
		c.r = (c.r * k2_ + cc.r * k2) / znam;
		c.a = 255 - ((kk_ * kk) >> 8);
	}

	static void mixmix2(const _color cc1, const _color cc2, _color& c)
	{
		uint kk = 256 - cc1.a - cc2.a;
		uint k1 = uint(cc1.a) * 255;
		uint k2 = uint(cc2.a) * 255;
		uint kk_ = 255 - c.a;
		uint k0 = (uint(c.a) + 1) * kk;
		uint znam = 65535 - kk * kk_;
		c.b = (c.b * k0 + cc1.b * k1 + cc2.b * k2) / znam;
		c.g = (c.g * k0 + cc1.g * k1 + cc2.g * k2) / znam;
		c.r = (c.r * k0 + cc1.r * k1 + cc2.r * k2) / znam;
		c.a = 255 - ((kk_ * kk) >> 8);
	}
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool _picture::set_from_text(std::string_view s, _color c00, _color cc1)
{
	if (s.size() != 144) return false; // сжатие кнопок 24x24 (cc1, c00) в будущем сдалать универсальное сжатие картинок
	resize({ 24, 24 });
	clear(c00);
	uchar a[72];
	if (!string_to_mem(s, a, 72)) return false;
	for (i64 i = 0; i < 576; i++)
		if (a[i >> 3] & (1 << (i & 7)))
			data[i] = cc1;
	return true;
}

bool _picture::load_from_file(const std::filesystem::path& file_name)
{
	_stack mem;
	if (!mem.load_from_file(file_name)) return false;
	char b, m;
	uint bfSize, bfReserved, bfOffBits;
	uint biSize, biCompression, biSizeImage, biClrUsed, biClrImportant;
	int biWidth, biHeigh, biXPelsPerMeter, biYPelsPerMeter;
	ushort biPlanes, biBitCount;
	// BITMAPFILEHEADER
	mem >> b >> m >> bfSize >> bfReserved >> bfOffBits;
	// BITMAPINFOHEADER
	mem >> biSize >> biWidth >> biHeigh >> biPlanes >> biBitCount >> biCompression >> biSizeImage >> biXPelsPerMeter;
	mem >> biYPelsPerMeter >> biClrUsed >> biClrImportant;
	if (b != 'B' || m != 'M' || bfOffBits < 54 || biWidth <= 0 || biHeigh == 0) return false;
	bool upend = biHeigh > 0;
	biHeigh = abs(biHeigh);
	if (bfOffBits + biHeigh * biWidth * (biBitCount / 8) > mem.size) return false;
	mem.adata = bfOffBits;
	resize({ biWidth, biHeigh });
	if (biBitCount == 24)
	{
		i64 finish = biWidth % 4;
		for (i64 y = 0; y < biHeigh; y++)
		{
			auto yy = (upend) ? biHeigh - 1 - y : y;
			_color* c = &data[yy * size.x];
			for (i64 x = 0; x < biWidth; x++)
			{
				ushort a;
				uchar aa;
				mem >> a >> aa;
				uint cc = 0xff000000 + a + (uint(aa) << 16);
				c[x] = { cc };
			}
			char a;
			for (i64 i = 0; i < finish; i++) mem >> a;
		}
		transparent = false;
		return true;
	}
	if (biBitCount == 32)
	{
		mem.pop_data(data, size.square() * 4);
		set_transparent();
		return true;
	}
	return false;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

_color* _picture::scan_line(i64 y) const
{
	return (y >= 0 && y < size.y) ? &data[y * size.x] : nullptr;
}

bool _picture::save_to_file(const std::filesystem::path& file_name)
{
	_stack mem;
	// BITMAPFILEHEADER
	mem << 'B' << 'M';   // bfType
	mem << uint(size.square() * 4 + 54); // bfSize
	mem << uint(0);      // bfReserved1 bfReserved2
	mem << uint(54);     // bfOffBits
	// BITMAPINFOHEADER
	mem << uint(40);     // biSize
	mem << int(size.x);  // biWidth
	mem << int(-size.y); // biHeigh
	mem << ushort(1);    // biPlanes
	mem << ushort(32);   // biBitCount
	mem << uint(0);      // biCompression
	mem << uint(0);      // biSizeImage
	mem << uint(0);      // biXPelsPerMeter
	mem << uint(0);      // biYPelsPerMeter
	mem << uint(0);      // biClrUsed
	mem << uint(0);      // biClrImportant
	mem.push_data(data, size.square() * 4);

	return mem.save_to_file(file_name);
}

void _picture::set_drawing_area(const _iarea& q)
{ 
	drawing_area = q & size;
}

_picture::~_picture()
{
	delete[] data;
}

_picture::_picture(const _picture& copy) : size(copy.size), transparent(copy.transparent), drawing_area(copy.size)
{
	if (size.empty()) return;
	data = (_color*)(new uint[size.square()]);
	memcpy(data, copy.data, size.square() * 4);
}

_picture::_picture(_isize r)
{
	drawing_area = size = r;
	if (!size.empty()) data = (_color*)(new uint[size.square()]);
}

_picture::_picture(_isize r, _color c)
{
	drawing_area = size = r;
	if (!size.empty())	data = (_color*)(new uint[size.square()]);
	clear(c);
}

_picture::_picture(_picture&& move) noexcept : data(move.data), size(move.size), transparent(move.transparent),
drawing_area(move.drawing_area)
{
	move.data = nullptr;
	move.drawing_area = move.size = { 0,0 };
}

bool _picture::operator==(const _picture& pic) const
{
	if (size != pic.size) return false;
	auto r = size.square();
	for (auto i = 0; i < r; i++) if (data[i].c != pic.data[i].c) return false;
	return true;
}

_picture& _picture::operator=(const _picture& copy)
{
	if (&copy == this) return *this;
	resize(copy.size);
	transparent = copy.transparent;
	memcpy(data, copy.data, size.square() * 4);
	return *this;
}

_picture& _picture::operator=(_picture&& move) noexcept
{
	if (&move == this) return *this;
	delete[] data;
	data = move.data;
	size = move.size;
	transparent = move.transparent;
	drawing_area = move.drawing_area;
	move.data = nullptr;
	move.drawing_area = move.size = { 0,0 };
	return *this;
}

void _picture::set_transparent()
{ // *
	i64 r = size.square();
	for (i64 i = 0; i < r; i++) if (data[i].a != 0xff) { transparent = true; return; }
	transparent = false;
}

void _picture::draw(_ixy r, const _picture& bm)
{
	_iarea b = bm.size.move(r) & drawing_area;
	if (b.empty()) return;
	if (!bm.transparent)
	{
		size_t len_line = b.x.length() * 4;
		for (i64 y = b.y.min; y < b.y.max; y++)	memcpy(&pixel(b.x.min, y), &bm.pixel(b.x.min - r.x, y - r.y), len_line);
		return;
	}
	for (i64 y = b.y.min; y < b.y.max; y++)
	{
		_color* c1 = &pixel(b.x.min, y);
		const _color* c2 = &bm.pixel(b.x.min - r.x, y - r.y);
		if (transparent)
			for (i64 x = b.x.min; x < b.x.max; x++, c1++, c2++) _color_mixing::mix2(*c2, *c1);
		else
			for (i64 x = b.x.min; x < b.x.max; x++, c1++, c2++) _color_overlay::mix2(*c2, *c1);
	}
}

bool _picture::resize(_isize wh)
{
	if (size == wh) return false;
	size = wh;
	delete[] data;
	data = (size.empty()) ? nullptr : (_color*)(new uint[size.square()]);
	drawing_area = size;
	transparent = false;
	return true;
}

void _picture::clear(_color c)
{ // *
	if (drawing_area != size)
		fill_rectangle(size, c, true);
	else
	{
		transparent = c.a != 0xff;
		memset32((uint*)data, c.c, size.square());
	}
}

t_t void _picture::line5_x_compact(_ixy p1, _ixy p2, _color c)
{
	double y = p1.y + 0.5;
	double dy_dx = double(p2.y - p1.y) / (p2.x - p1.x);
	if (p1.x < drawing_area.x.min)
	{
		y += (drawing_area.x.min - p1.x) * dy_dx;
		p1.x = drawing_area.x.min;
	}
	if (p2.x >= drawing_area.x.max) p2.x = drawing_area.x.max - 1;
	if (p2.x < p1.x) return;
	_t cc(c);
	for (i64 x = p1.x; x <= p2.x; x++, y += dy_dx) cc.mix(pixel(x, (i64)y));
}

t_t void _picture::line5_y_compact(_ixy p1, _ixy p2, _color c)
{
	double x = p1.x + 0.5;
	double dx_dy = double(p2.x - p1.x) / (p2.y - p1.y);
	if (p1.y < drawing_area.y.min)
	{
		x += (drawing_area.y.min - p1.y) * dx_dy;
		p1.y = drawing_area.y.min;
	}
	if (p2.y >= drawing_area.y.max) p2.y = drawing_area.y.max - 1;
	if (p2.y < p1.y) return;
	_t cc(c);
	for (i64 y = p1.y; y <= p2.y; y++, x += dx_dy) cc.mix(pixel((i64)x, y));
}

t_t void _picture::line5_x(_ixy p1, _ixy p2, _color c)
{
	if (c.a == 0) return;
	if (std::is_same<_color_substitution, _t>::value) transparent |= c.a != 0xff;
	if (p1.x > p2.x) std::swap(p1, p2);
	if (drawing_area.y.test(p1.y) && drawing_area.y.test(p2.y))
	{
		line5_x_compact<_t>(p1, p2, c);
		return;
	}
	double dy_dx = double(p2.y - p1.y) / (p2.x - p1.x);
	double dx_dy = 1.0 / dy_dx;
	double y = p1.y + 0.5;
	double y2 = p2.y + 0.5;
	if (p1.x < drawing_area.x.min)
	{
		y += (drawing_area.x.min - p1.x) * dy_dx;
		p1.x = drawing_area.x.min;
	}
	if (p2.x >= drawing_area.x.max)
	{
		y2 -= (p2.x - drawing_area.x.max + 1) * dy_dx;
		p2.x = drawing_area.x.max - 1;
	}
	if (p2.x < p1.x) return;
	if (y < drawing_area.y.min)
	{
		if (y2 < drawing_area.y.min) return;
		i64 dx = (i64)((drawing_area.y.min - y) * dx_dy);
		p1.x += dx;
		y += dx * dy_dx;
		while (y < drawing_area.y.min) { p1.x++; y += dy_dx; }
	}
	else if (y >= drawing_area.y.max)
	{
		if (y2 >= drawing_area.y.max) return;
		i64 dx = (i64)((drawing_area.y.max - y) * dx_dy);
		p1.x += dx;
		y += dx * dy_dx;
		while (y >= drawing_area.y.max) { p1.x++; y += dy_dx; }
	}
	if (p2.x < p1.x) return;
	_t cc(c);
	if (dy_dx > 0)
		for (i64 x = p1.x; (x <= p2.x) && (y < drawing_area.y.max); x++, y += dy_dx) cc.mix(pixel(x, (i64)y));
	else
		for (i64 x = p1.x; (x <= p2.x) && (y >= drawing_area.y.min); x++, y += dy_dx) cc.mix(pixel(x, (i64)y));
}

t_t void _picture::line5_y(_ixy p1, _ixy p2, _color c)
{
	if (c.a == 0) return;
	if (std::is_same<_color_substitution, _t>::value) transparent |= c.a != 0xff;
	if (p1.y > p2.y) std::swap(p1, p2);
	if (drawing_area.x.test(p1.x) && drawing_area.x.test(p2.x))
	{
		line5_y_compact<_t>(p1, p2, c);
		return;
	}
	double dx_dy = double(p2.x - p1.x) / (p2.y - p1.y);
	double dy_dx = 1.0 / dx_dy;
	double x = p1.x + 0.5;
	double x2 = p2.x + 0.5;
	if (p1.y < drawing_area.y.min)
	{
		x += (drawing_area.y.min - p1.y) * dx_dy;
		p1.y = drawing_area.y.min;
	}
	if (p2.y >= drawing_area.y.max)
	{
		x2 -= (p2.y - drawing_area.y.max + 1) * dx_dy;
		p2.y = drawing_area.y.max - 1;
	}
	if (p2.y < p1.y) return;
	if (x < drawing_area.x.min)
	{
		if (x2 < drawing_area.x.min) return;
		i64 dy = (i64)((drawing_area.x.min - x) * dy_dx);
		p1.y += dy;
		x += dy * dx_dy;
		while (x < drawing_area.x.min) { p1.y++; x += dx_dy; }
	}
	else if (x >= drawing_area.x.max)
	{
		if (x2 >= drawing_area.x.max) return;
		i64 dy = (i64)((drawing_area.x.max - x) * dy_dx);
		p1.y += dy;
		x += dy * dx_dy;
		while (x >= drawing_area.x.max) { p1.y++; x += dx_dy; }
	}
	if (p2.y < p1.y) return;
	_t cc(c);
	if (dx_dy > 0)
		for (i64 y = p1.y; (y <= p2.y) && (x < drawing_area.x.max); y++, x += dx_dy) cc.mix(pixel((i64)x, y));
	else
		for (i64 y = p1.y; (y <= p2.y) && (x >= drawing_area.x.min); y++, x += dx_dy) cc.mix(pixel((i64)x, y));
}

void _picture::vertical_line(i64 x, _iinterval y, _color c)
{ // *
	if (c.a == 0xff) { vertical_line<_color_substitution>(x, y, c); return; }
	if (transparent) { vertical_line<_color_mixing>(x, y, c); return; }
	vertical_line<_color_overlay>(x, y, c);
}

void _picture::horizontal_line(_iinterval x, i64 y, _color c)
{ // *
	if (c.a == 0xff) { horizontal_line<_color_substitution>(x, y, c); return; }
	if (transparent) { horizontal_line<_color_mixing>(x, y, c); return; }
	horizontal_line<_color_overlay>(x, y, c);
}

t_t void _picture::vertical_line(i64 x, _iinterval y, _color c)
{ // ***
	if (c.a == 0) return;
	if (!drawing_area.x.test(x)) return;
	y &= drawing_area.y;
	if (y.empty()) return;
	if (std::is_same<_color_substitution, _t>::value) transparent |= c.a != 0xff;
	_t cmix(c);
	_color* cс_max = &pixel(x, y.max);
	for (auto cc = &pixel(x, y.min); cc < cс_max; cc += size.x) cmix.mix(*cc);
}

t_t void _picture::horizontal_line(_iinterval x, i64 y, _color c)
{ // **
	if (c.a == 0) return;
	if (!drawing_area.y.test(y)) return;
	x &= drawing_area.x;
	if (x.empty()) return;
	if (std::is_same<_color_substitution, _t>::value) transparent |= c.a != 0xff;
	_t cmix(c);
	_color* cс_max = &pixel(x.max, y);
	for (auto cc = &pixel(x.min, y); cc < cс_max; cc++) cmix.mix(*cc);
}

t_t void _picture::line(_ixy p1, _ixy p2, _color c)
{
	if (p1.y == p2.y) { horizontal_line<_t>(_iinterval(p1.x) += p2.x, p1.y, c); return; }
	if (p1.x == p2.x) { vertical_line<_t>(p1.x, _iinterval(p1.y) += p2.y, c); return; }
	if (abs(p1.x - p2.x) >= abs(p1.y - p2.y)) line5_x<_t>(p1, p2, c); else line5_y<_t>(p1, p2, c);
}

void _picture::line(_ixy p1, _ixy p2, _color c)
{
	if (c.a == 0xff) { line<_color_substitution>(p1, p2, c); return; }
	if (transparent) { line<_color_mixing>(p1, p2, c); return; }
	line<_color_overlay>(p1, p2, c);
}

void _picture::line(_xy p1, _xy p2, _color c, double l)
{
	uint kk = 255 - (c.c >> 24);
	if (kk == 0xFF) return; // полностью прозрачная
	if (p1.y > p2.y) std::swap(p1, p2);
	l *= 0.5; // половина толщины
	i64 xmin = int(std::min(p1.x, p2.x) - l);
	i64 xmax = int(std::max(p1.x, p2.x) + l);
	i64 ymin = int(p1.y - l);
	i64 ymax = int(p2.y + l);
	if (xmin < drawing_area.x.min) xmin = drawing_area.x.min;
	if (ymin < drawing_area.y.min) ymin = drawing_area.y.min;
	if (xmax >= drawing_area.x.max) xmax = drawing_area.x.max - 1;
	if (ymax >= drawing_area.y.max) ymax = drawing_area.y.max - 1;
	if ((xmax < xmin) || (ymax < ymin)) return; // активная область за экраном
	double dx = p2.x - p1.x;
	double dy = p2.y - p1.y;
	if (abs(dy) < 0.001) dy = 0.001; // костыль против горизонтальной линии
	double ll_1 = 1.0 / sqrt(dx * dx + dy * dy);
	if (ll_1 > 2) return; // слишком короткая линия
	double x2y1_y2x1 = p2.x * p1.y - p2.y * p1.x;
	double rcentrscreen = (dy * size.x * 0.5 - dx * size.y * 0.5 + x2y1_y2x1) * ll_1; // расстояние от линии до центра эрана
	if (rcentrscreen < 0) rcentrscreen = -rcentrscreen;
	if (rcentrscreen - l > ((double)size.x + size.y) * 0.5) return; // линия не задевает экран
	double dxdy = dx / dy;
	double db = l * sqrt(dxdy * dxdy + 1.0);
	double xx1 = p1.x + (ymin - p1.y) * dxdy; // х верхней точки пересечения
	double xx2 = xx1 + dxdy;                  // x нижней точки пересечения
	if (dxdy < 0) std::swap(xx1, xx2);
	xx1 -= db;
	xx2 += db;
	double drr = dy * ll_1;
	double temp1 = dy * 0.5 - dx * 0.5 + x2y1_y2x1;
	uint   k2 = 256 - kk;
	uint   red = (c.c & 255);
	uint   green = ((c.c >> 8) & 255);
	uint   blue = ((c.c >> 16) & 255);
	uint   d1 = red * k2;
	uint   d2 = green * k2;
	uint   d3 = blue * k2;
	for (i64 j = ymin; j <= ymax; j++, xx1 += dxdy, xx2 += dxdy) // цикл по линиям
	{
		i64 xx1i = (int)xx1;
		i64 xx2i = (int)xx2;
		if (xx1i < xmin) xx1i = xmin;
		if (xx2i > xmax) xx2i = xmax;
		if (xx2i < xx1i) continue;
		double rr = (dy * xx1i - dx * j + temp1) * ll_1;
		uchar* c2 = (uchar*)&(data[j * size.x + xx1i]);
		for (i64 i = xx1i; i <= xx2i; i++, rr += drr, c2 += 4)
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
			c2[0] = (c2[0] * kkk + red * kk2) >> 8;
			c2[1] = (c2[1] * kkk + green * kk2) >> 8;
			c2[2] = (c2[2] * kkk + blue * kk2) >> 8;
		}
	}
}

void _picture::stretch_draw_speed(_picture* bm, i64 nXDest, i64 nYDest, double m)
{
	if (bm->size.x * bm->size.y == 0) return;
	i64 nWidth = (int)(bm->size.x * m + 0.5);
	i64 nHeight = (int)(bm->size.y * m + 0.5);
	if ((nWidth == bm->size.x) && (nHeight == bm->size.y))
	{
		draw({ nXDest, nYDest }, *bm);
		return;
	}
	i64 nXSrc = 0;
	i64 nYSrc = 0;
	i64 bmrx = nWidth;
	i64 bmry = nHeight;
	if (nXDest < drawing_area.x.min)
	{
		nWidth += nXDest - drawing_area.x.min;
		nXSrc -= nXDest - drawing_area.x.min;
		nXDest = drawing_area.x.min;
	}
	if (nYDest < drawing_area.y.min)
	{
		nHeight += nYDest - drawing_area.y.min;
		nYSrc -= nYDest - drawing_area.y.min;
		nYDest = drawing_area.y.min;
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
	if ((nXDest >= drawing_area.x.max) || (nYDest >= drawing_area.y.max)) return;
	if ((nXSrc >= bmrx) || (nYSrc >= bmry)) return;
	if (nXDest + nWidth > drawing_area.x.max) nWidth = drawing_area.x.max - nXDest;
	if (nYDest + nHeight > drawing_area.y.max) nHeight = drawing_area.y.max - nYDest;
	if (nXSrc + nWidth > bmrx) nWidth = bmrx - nXSrc;
	if (nYSrc + nHeight > bmry) nHeight = bmry - nYSrc;
	if ((nWidth <= 0) || (nHeight <= 0)) return;
	double mm = 1.0 / m;
	for (i64 j = 0; j < nHeight; j++)
	{
		uchar* s1 = (uchar*)(data + ((nYDest + j) * size.x + nXDest));
		uint* ss2 = (uint*)(bm->data + (i64)(((nYSrc + j) * mm)) * bm->size.x);
		for (i64 i = 0; i < nWidth; i++)
		{
			uchar* s2_ = (uchar*)(ss2 + (i64)((nXSrc + i) * mm));
			uint pp2 = s2_[3];
			uint pp1 = 256 - pp2;
			s1[0] = (s1[0] * pp1 + s2_[0] * pp2) >> 8;
			s1[1] = (s1[1] * pp1 + s2_[1] * pp2) >> 8;
			s1[2] = (s1[2] * pp1 + s2_[2] * pp2) >> 8;
			s1 += 4;
		}
	}
}

void _picture::stretch_draw(_picture* bm, i64 x, i64 y, double m)
{
	if (bm->size.x * bm->size.y == 0) return;
	int rx2 = (int)(bm->size.x * m + 0.5);
	int ry2 = (int)(bm->size.y * m + 0.5);
	if ((rx2 == bm->size.x) && (ry2 == bm->size.y))
	{
		draw({ x, y }, *bm);
		return;
	}
	i64 x1 = (x >= drawing_area.x.min) ? x : drawing_area.x.min;
	i64 y1 = (y >= drawing_area.y.min) ? y : drawing_area.y.min;
	i64 x2 = x + rx2 - 1;
	i64 y2 = y + ry2 - 1;
	if (x2 >= drawing_area.x.max) x2 = drawing_area.x.max - 1;
	if (y2 >= drawing_area.y.max) y2 = drawing_area.y.max - 1;
	if ((x2 < x1) || (y2 < y1)) return;
	i64 nox1 = (i64)x1 - x;
	i64 noy1 = (i64)y1 - y;
	i64 nox2 = (i64)x2 - x;
	i64 noy2 = (i64)y2 - y;
	i64 f1x = bm->size.x;
	i64 f1y = bm->size.y;
	i64 f1xx = rx2;
	i64 f1yy = ry2;
	for (i64 j = noy1; j <= noy2; j++)
	{
		i64  pyn = j * f1y;
		i64  pyk = (j + 1) * f1y - 1;
		i64  pyn2 = pyn / f1yy;
		i64  pyk2 = pyk / f1yy;
		uchar* p1 = (uchar*)&data[(j + y) * size.x + x1 - 1];
		for (i64 i = nox1; i <= nox2; i++)
		{
			p1 += 4;
			i64 pxn = i * f1x;
			i64 pxk = (i + 1) * f1x - 1;
			i64 pxn2 = pxn / f1xx;
			i64 pxk2 = pxk / f1xx;
			if ((pyn2 == pyk2) && (pxn2 == pxk2))
			{
				uchar* p2 = (uchar*)&bm->data[pyn2 * f1x + pxn2];
				uint pp2 = p2[3];
				uint pp1 = 256 - pp2;
				p1[0] = (p1[0] * pp1 + p2[0] * pp2) >> 8;
				p1[1] = (p1[1] * pp1 + p2[1] * pp2) >> 8;
				p1[2] = (p1[2] * pp1 + p2[2] * pp2) >> 8;
				continue;
			}
			if (pyn2 == pyk2)
			{
				uchar* p2 = (uchar*)&bm->data[pyn2 * f1x + pxn2];
				i64 k1 = (pxn2 + 1) * f1xx - pxn;
				i64 k2 = pxk - pxk2 * f1xx + 1;
				i64 S = f1x * 256;
				i64 pp2 = p2[3] * k1;
				i64 R = p2[0] * pp2;
				i64 G = p2[1] * pp2;
				i64 B = p2[2] * pp2;
				i64 A = (255 - p2[3]) * k1;
				p2 += 4;
				for (i64 x_ = pxn2 + 1; x_ < pxk2; x_++)
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
				uchar* p2 = (uchar*)&bm->data[pyn2 * f1x + pxn2];
				i64 k1 = (pyn2 + 1) * f1yy - pyn;
				i64 k2 = pyk - pyk2 * f1yy + 1;
				i64 S = f1y * 256;
				i64 pp2 = p2[3] * k1;
				i64 R = p2[0] * pp2;
				i64 G = p2[1] * pp2;
				i64 B = p2[2] * pp2;
				i64 A = (255 - p2[3]) * k1;
				p2 += (4 * f1x);
				for (i64 y_ = pyn2 + 1; y_ < pyk2; y_++)
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
			i64 kx1 = (pxn2 + 1) * f1xx - pxn;
			i64 kx2 = pxk - pxk2 * f1xx + 1;
			i64 ky1 = (pyn2 + 1) * f1yy - pyn;
			i64 ky2 = pyk - pyk2 * f1yy + 1;
			i64 S = f1x * f1y * 256;
			uchar* p2 = (uchar*)&bm->data[pyn2 * f1x + pxn2];
			i64 pp2 = p2[3] * kx1 * ky1;
			i64 R = p2[0] * pp2;
			i64 G = p2[1] * pp2;
			i64 B = p2[2] * pp2;
			i64 A = (255 - p2[3]) * kx1 * ky1;
			p2 += 4;
			for (i64 x_ = pxn2 + 1; x_ < pxk2; x_++)
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
			p2 = (uchar*)&bm->data[pyk2 * f1x + pxn2];
			pp2 = p2[3] * kx1 * ky2;
			R += p2[0] * pp2;
			G += p2[1] * pp2;
			B += p2[2] * pp2;
			A += (255 - p2[3]) * kx1 * ky2;
			p2 += 4;
			for (i64 x_ = pxn2 + 1; x_ < pxk2; x_++)
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
			p2 = (uchar*)&bm->data[(pyn2 + 1) * f1x + pxn2];
			for (i64 y_ = pyn2 + 1; y_ < pyk2; y_++)
			{
				pp2 = p2[3] * f1yy * kx1;
				R += p2[0] * pp2;
				G += p2[1] * pp2;
				B += p2[2] * pp2;
				A += (255 - p2[3]) * f1yy * kx1;
				p2 += (4 * f1x);
			}
			p2 = (uchar*)&bm->data[(pyn2 + 1) * f1x + pxk2];
			for (i64 y_ = pyn2 + 1; y_ < pyk2; y_++)
			{
				pp2 = p2[3] * f1yy * kx2;
				R += p2[0] * pp2;
				G += p2[1] * pp2;
				B += p2[2] * pp2;
				A += (255 - p2[3]) * f1yy * kx2;
				p2 += (4 * f1x);
			}
			for (i64 y_ = pyn2 + 1; y_ < pyk2; y_++)
			{
				p2 = (uchar*)&bm->data[y_ * f1x + pxn2 + 1];
				for (i64 x_ = pxn2 + 1; x_ < pxk2; x_++)
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

t_t void _picture_functions::fill_rectangle3(_iarea r, _color c)
{
	if (r.x.length() == 1)
	{ 
		vertical_line<_t>(r.x.min, r.y, c);
		return;
	}
	_t cmix(c);
	for (i64 y = r.y.min; y < r.y.max; y++)
	{
		_color* cс_max = &pixel(r.x.max, y);
		for (auto cc = &pixel(r.x.min, y); cc < cс_max; cc++) cmix.mix(*cc);
	}
}

void _picture::fill_rectangle(_iarea r, _color c, bool rep)
{
	if (c.a == 0 && !rep) return;
	r &= drawing_area;
	if (r.empty()) return;
	if (rep || c.a == 0xff)
	{
		transparent |= c.a != 0xff;
		((_picture_functions*)this)->fill_rectangle3<_color_substitution>(r, c);
	}
	else if (transparent)
		((_picture_functions*)this)->fill_rectangle3<_color_mixing>(r, c);
	else
		((_picture_functions*)this)->fill_rectangle3<_color_overlay>(r, c);
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

_isize _picture::size_text16(std::string_view s, i64 n)
{
	constexpr i64 ly = 13;
	i64 l = 0;
	i64 probel = 0;
	for (auto c : s)
	{
		probel = (c == ' ') ? 4 : 1;
		l += probel;
		const ushort* ss = font16[(uchar)(c)];
		int lx = lx2;
		for (int j = lx - 1; j >= 0; j--)
		{
			if (ss[j]) break;
			lx--;
		}
		l += lx;
	}
	l -= probel;
	return { l * n, ly * n };
}

void _picture::text16n(i64 x, i64 y, std::string_view st, i64 n, uint c, uint bg)
{
	if (n < 1) return;
	if (n == 1) return text16({ x, y }, st, c, bg);
	auto text_area = size_text16(st, n).move({ x,y }) & drawing_area;
	if (text_area.empty()) return;
	fill_rectangle(text_area, bg);
	constexpr int ly = 13;
	if ((y >= drawing_area.y.max) || (y + ly * n <= drawing_area.y.min)) return;
	uint kk = 255 - (c >> 24);
	if (kk == 0xFF) return; // полностью прозрачная
	uint k2 = 256 - kk;
	uint d1 = (c & 255) * k2;
	uint d2 = ((c >> 8) & 255) * k2;
	uint d3 = ((c >> 16) & 255) * k2;
	uint d11 = d1 * 255;
	uint d22 = d2 * 255;
	uint d33 = d3 * 255;

	i64 j0 = 0;
	i64 j1 = ly;
	if (y < drawing_area.y.min) j0 = (drawing_area.y.min + n - 1 - y) / n;
	if (y + ly * n > drawing_area.y.max) j1 = (drawing_area.y.max - y) / n;

	for (auto s : st)
	{
		if (x >= drawing_area.x.max) break;
		i64 probel = (s == 32) ? 4 : 1;
		const ushort* ss = font16[(uchar)s];
		i64 lx = lx2;
		for (i64 j = lx - 1; j >= 0; j--)
		{
			if (ss[j]) break;
			lx--;
		}
		if (x + lx * n <= 0)
		{
			x += (lx + probel) * n;
			continue;
		}
		i64 i0 = 0;
		i64 i1_ = lx;
		if (x < drawing_area.x.min) i0 = (drawing_area.x.min + n - 1 - x) / n;
		if (x + lx * n > drawing_area.x.max) i1_ = (drawing_area.x.max - x) / n;
		for (i64 j = j0; j < j1; j++)
		{
			ushort mask = (ushort(1) << j);
			for (i64 jj = 0; jj < n; jj++)
			{
				const ushort* ss2 = ss;
				if (kk == 0)
				{
					uint* c2 = (uint*)&data[(y + j * n + jj) * size.x + x + i0 * n];
					for (i64 i = i0; i < i1_; i++)
					{
						if (*ss2 & mask)
						{
							for (int ii = 0; ii < n; ii++)
							{
								*c2 = c;
								c2++;
							}
						}
						else
							c2 += n;
						ss2++;
					}
				}
				else
				{
					uchar* c2 = (uchar*)&data[(y + j * n + jj) * size.x + x + i0 * n];
					if (!transparent)
					{
						for (i64 i = i0; i < i1_; i++)
						{
							if (*ss2 & mask)
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
							ss2++;
						}
					}
					else
					{
						for (i64 i = i0; i < i1_; i++)
						{
							if (*ss2 & mask)
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
							ss2++;
						}
					}
				}
			}
		}
		x += (lx + probel) * n;
	}
}

void _picture::text16(_ixy p, std::string_view st, uint c, uint bg)
{
	auto text_area = size_text16(st).move(p) & drawing_area;
	if (text_area.empty()) return;
	fill_rectangle(text_area, bg);
	constexpr int ly = 13;
	uint kk = 255 - (c >> 24);
	if (kk == 0xFF) return; // полностью прозрачная
	uint k2 = 256 - kk;
	uint d1 = (c & 255) * k2;
	uint d2 = ((c >> 8) & 255) * k2;
	uint d3 = ((c >> 16) & 255) * k2;
	uint d11 = d1 * 255;
	uint d22 = d2 * 255;
	uint d33 = d3 * 255;

	i64 j0 = 0;
	i64 j1 = ly;
	if (p.y < drawing_area.y.min) j0 = drawing_area.y.min - p.y;
	if (p.y + ly > drawing_area.y.max) j1 = drawing_area.y.max - p.y;
	for (auto s : st)
	{
		if (p.x >= drawing_area.x.max) break;
		i64 probel = (s == 32) ? 4 : 1;
		const ushort* ss = font16[(uchar)s];
		i64 lx = lx2;
		for (i64 j = lx - 1; j >= 0; j--)
		{
			if (ss[j]) break;
			lx--;
		}
		if (p.x + lx <= 0)
		{
			p.x += lx + probel;
			continue;
		}
		i64 i0 = 0;
		i64 i1_ = lx;
		if (p.x < drawing_area.x.min) i0 = drawing_area.x.min - p.x;
		if (p.x + lx > drawing_area.x.max) i1_ = drawing_area.x.max - p.x;
		for (i64 j = j0; j < j1; j++)
		{
			ushort        mask = (ushort(1) << j);
			const ushort* ss2 = ss;
			if (kk == 0)
			{
				uint* c2 = (uint*)&data[(p.y + j) * size.x + p.x + i0];
				for (i64 i = i0; i < i1_; i++)
				{
					if (*ss2++ & mask) *c2 = c;
					c2++;
				}
			}
			else
			{
				uchar* c2 = (uchar*)&data[(p.y + j) * size.x + p.x + i0];
				if (!transparent)
				{
					for (i64 i = i0; i < i1_; i++)
					{
						if (*ss2++ & mask)
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
					for (i64 i = i0; i < i1_; i++)
					{
						if (*ss2++ & mask)
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
		p.x += lx + probel;
	}
}

void _picture::rectangle(_iarea oo, _color c)
{
	if (oo.empty()) return;
	line({ oo.x.min, oo.y.min }, { oo.x.max - 1, oo.y.min }, c);
	if (oo.y.length() == 1) return;
	line({ oo.x.min, oo.y.max - 1 }, { oo.x.max - 1, oo.y.max - 1 }, c);
	if (oo.y.length() > 2)
	{ // такие заморочки из за углов
		line({ oo.x.min, oo.y.min + 1 }, { oo.x.min, oo.y.max - 2 }, c);
		line({ oo.x.max - 1, oo.y.min + 1 }, { oo.x.max - 1, oo.y.max - 2 }, c);
	}
}

_stack& operator<<(_stack& o, _picture const& p)
{
	o << p.get_size();
	o.push_data(p.scan_line(0), 4 * p.get_size().square());
	return o;
}

_stack& operator>>(_stack& o, _picture& p)
{
	_isize r;
	o >> r;
	p.resize(r);
	o.pop_data(p.scan_line(0), 4 * p.get_size().square());
	p.set_transparent();
	return o;
}

void _picture::fill_rectangle(_area r, _color c)
{
	if (c.a == 0) return;
	r &= drawing_area;
	if ((r.x.length() < 0.004) || (r.y.length() < 0.004)) return;
	_iarea a = r;
	auto ca = c.a;
	if (a.y.length() == 1) // горизонтальная линия
	{
		auto c2 = &pixel(a.x.min, a.y.min);
		if (a.x.length() == 1) // точка
		{
			c.a = uchar(c.a * (r.x.max - r.x.min) * (r.y.max - r.y.min));
			if (transparent) _color_mixing::mix2(c, *c2); else _color_overlay::mix2(c, *c2);
			return;
		}

		c.a = uchar(ca * (r.y.max - r.y.min) * (a.x.min + 1 - r.x.min));
		if (transparent) _color_mixing::mix2(c, *c2); else _color_overlay::mix2(c, *c2);
		c2++;

		c.a = uchar(ca * (r.y.max - r.y.min));
		i64 d = a.x.min - a.x.max + 3;
		if (transparent)
		{
			_color_mixing cmix(c);
			while (d <= 0) { cmix.mix(*c2); c2++; d++; }
		}
		else
		{
			_color_overlay cmix(c);
			while (d <= 0) { cmix.mix(*c2); c2++; d++; }
		}

		c.a = uchar(ca * (r.y.max - r.y.min) * (r.x.max - a.x.max + 1));
		if (transparent) _color_mixing::mix2(c, *c2); else _color_overlay::mix2(c, *c2);

		return;
	}
	if (a.x.length() == 1) // вертикальная линия
	{
		auto c2 = &pixel(a.x.min, a.y.min);

		c.a = uchar(ca * (r.x.max - r.x.min) * (a.y.min + 1 - r.y.min));
		if (transparent) _color_mixing::mix2(c, *c2); else _color_overlay::mix2(c, *c2);
		c2 += size.x;

		c.a = uchar(ca * (r.x.max - r.x.min));
		i64 d = a.y.min - a.y.max + 3;
		if (transparent)
		{
			_color_mixing cmix(c);
			while (d <= 0) { cmix.mix(*c2); c2 += size.x; d++; }
		}
		else
		{
			_color_overlay cmix(c);
			while (d <= 0) { cmix.mix(*c2); c2 += size.x; d++; }
		}

		c.a = uchar(ca * (r.x.max - r.x.min) * (r.y.max - a.y.max + 1));
		if (transparent) _color_mixing::mix2(c, *c2); else _color_overlay::mix2(c, *c2);

		return;
	}
	for (i64 y = a.y.min; y < a.y.max; y++)
	{
		auto c2 = &pixel(a.x.min, y);

		double k2 = 1;
		if (y == a.y.min) k2 = a.y.min + 1 - r.y.min;
		if (y == a.y.max - 1) k2 = r.y.max - a.y.max + 1;

		c.a = uchar(ca * k2 * (a.x.min + 1 - r.x.min));
		if (transparent) _color_mixing::mix2(c, *c2); else _color_overlay::mix2(c, *c2);
		c2++;

		c.a = uchar(ca * k2);
		if (c.a == 255)
		{
			auto d = a.x.max - a.x.min - 2;
			memset32((uint*)c2, c.c, d);
			c2 += d;
		}
		else
		{
			i64 d = a.x.min - a.x.max + 3;
			if (transparent)
			{
				_color_mixing cmix(c);
				while (d <= 0) { cmix.mix(*c2); c2++; d++; }
			}
			else
			{
				_color_overlay cmix(c);
				while (d <= 0) { cmix.mix(*c2); c2++; d++; }
			}
		}
		c.a = uchar(ca * k2 * (r.x.max - a.x.max + 1));
		if (transparent) _color_mixing::mix2(c, *c2); else _color_overlay::mix2(c, *c2);
	}
}

template <typename _t, typename _b> void _picture_functions::fill_ring3(_iarea area, _xy p, double r, double r2, _color c, _color c2)
{
	double rrmin = (r - 0.5) * (r - 0.5);
	double rrmax = (r + 0.5) * (r + 0.5);
	double drr = rrmax - rrmin;
	double ddmin = (r2 - 0.5) * (r2 - 0.5);
	double ddmax = (r2 + 0.5) * (r2 + 0.5);
	double ddd = ddmax - ddmin;
	p.x -= 0.5;
	p.y -= 0.5;
	double dxdx0 = (area.x.min - p.x) * (area.x.min - p.x);
	double ab0 = 2 * (area.x.min - p.x) + 1;
	_t cmix(c);
	_t cmix2(c2);
	auto ca = c.a;
	auto c2a = c2.a;
	i64 d_start = area.x.min - area.x.max + 1;
	for (i64 i = area.y.min; i < area.y.max; i++)
	{
		double dd = (i - p.y) * (i - p.y) + dxdx0;
		double ab = ab0;
		_color* cc = &pixel(area.x.min, i);
		i64 d_ = d_start;
		while (d_++ <= 0)
		{
			if (dd < rrmax)
			{
				if (dd <= ddmin) cmix2.mix(*cc);
				else if (dd > rrmin)
				{
					if (dd >= ddmax)
					{
						c.a = uchar(ca * (rrmax - dd) / drr);
						_b::mix2(c, *cc);
					}
					else
					{ // тонкое кольцо
						double m1 = (ddmax - dd) / ddd;
						c.a = uchar(ca * ((rrmax - dd) / drr - m1));
						c2.a = uchar(c2a * m1);
						_b::mixmix2(c, c2, *cc);
					}
				}
				else if (dd >= ddmax) cmix.mix(*cc);
				else
				{
					double m1 = (ddmax - dd) / ddd;
					c.a = uchar(ca * (1.0 - m1));
					c2.a = uchar(c2a * m1);
					_b::mixmix2(c, c2, *cc);
				}
			}
			cc++;
			dd += ab;
			ab += 2;
		}
	}

}

void _picture::fill_ring(_xy p, double r, double d, _color c, _color c2)
{
	if (r < 0.5) return;
	if (c.c == c2.c) { fill_circle(p, r, c); return; }
	if (c2.a == 0) { ring(p, r, d, c); return; }
	_iarea area(_iinterval(p.x - r, p.x + r), _iinterval(p.y - r, p.y + r));
	area &= drawing_area;
	if (area.empty()) return;
	double r2 = r - d;
	if (r2 < 0) r2 = 0;
	if (c.a == 0xff && c2.a == 0xff)
	{
		if (transparent)
			((_picture_functions*)this)->fill_ring3<_color_substitution, _color_mixing>(area, p, r, r2, c, c2);
		else
			((_picture_functions*)this)->fill_ring3<_color_substitution, _color_overlay>(area, p, r, r2, c, c2);
	}
	else if (transparent)
		((_picture_functions*)this)->fill_ring3<_color_mixing, _color_mixing>(area, p, r, r2, c, c2);
	else
		((_picture_functions*)this)->fill_ring3<_color_overlay, _color_overlay>(area, p, r, r2, c, c2);
}

template <typename _t, typename _b> void _picture_functions::ring3(_iarea area, _xy p, double r, double r2, _color c)
{
	double rrmin = (r - 0.5) * (r - 0.5);
	double rrmax = (r + 0.5) * (r + 0.5);
	double drr = 1.0 / (rrmax - rrmin);
	double ddmin = (r2 - 0.5) * (r2 - 0.5);
	double ddmax = (r2 + 0.5) * (r2 + 0.5);
	double ddd = 1.0 / (ddmax - ddmin);

	double xxx2 = (size.x / 2 - p.x) * (size.x / 2 - p.x) + (size.y / 2 - p.y) * (size.y / 2 - p.y);
	double yyy2 = 0.25 * size.x * size.x + 0.25 * size.y * size.y;
	if (xxx2 + yyy2 + 2 * sqrt(xxx2 * yyy2) < ddmin) return; // !!!! исправить на внутри области
	p.x -= 0.5;
	p.y -= 0.5;
	double dxdx0 = (area.x.min - p.x) * (area.x.min - p.x);
	double ab0 = 2 * (area.x.min - p.x) + 1;
	_t cmix(c);
	auto ca = c.a;
	i64 d_start = area.x.min - area.x.max + 1;
	for (i64 i = area.y.min; i < area.y.max; i++)
	{
		double dd = (i - p.y) * (i - p.y) + dxdx0;
		double ab = ab0;
		_color* cc = &pixel(area.x.min, i);
		i64 d_ = d_start;
		while (d_++ <= 0)
		{
			if ((dd < rrmax) && (dd > ddmin))
			{
				if (dd > rrmin)
				{
					if (dd >= ddmax)
					{
						c.a = uchar(ca * (rrmax - dd) * drr);
						_b::mix2(c, *cc);
					}
					else
					{ // тонкое кольцо! !!! исправить отриц. коэффмфиент p = {58.1263671875, 33.2783203125} r = 0.6 d = 0.4 c = 0xff4ed850
						c.a = uchar(ca * ((rrmax - dd) * drr - (ddmax - dd) * ddd));
						_b::mix2(c, *cc);
					}
				}
				else if (dd >= ddmax) cmix.mix(*cc);
				else
				{
					c.a = uchar(ca * (dd - ddmin) * ddd);
					_b::mix2(c, *cc);
				}
			}
			cc++;
			dd += ab;
			ab += 2;
		}
	}
}

void _picture::ring(_xy p, double r, double d, _color c)
{
	if (r < 0.5 || c.a == 0) return;
	_iarea area(_iinterval(p.x - r, p.x + r), _iinterval(p.y - r, p.y + r));
	area &= drawing_area;
	if (area.empty()) return;
	double r2 = r - d;
	if (r2 < 0) r2 = 0;
	if (c.a == 0xff)
	{
		if (transparent)
			((_picture_functions*)this)->ring3<_color_substitution, _color_mixing>(area, p, r, r2, c);
		else
			((_picture_functions*)this)->ring3<_color_substitution, _color_overlay>(area, p, r, r2, c);
	}
	else if (transparent)
		((_picture_functions*)this)->ring3<_color_mixing, _color_mixing>(area, p, r, r2, c);
	else
		((_picture_functions*)this)->ring3<_color_overlay, _color_overlay>(area, p, r, r2, c);
}

template <typename _t, typename _b> void _picture_functions::fill_circle3(_iarea area, _xy p, double r, _color c)
{
	double rrmin = (r - 0.5) * (r - 0.5);
	double rrmax = (r + 0.5) * (r + 0.5);
	double drr = rrmax - rrmin;
	p.x -= 0.5;
	p.y -= 0.5;
	double dxdx0 = (area.x.min - p.x) * (area.x.min - p.x);
	double ab0 = 2 * (area.x.min - p.x) + 1;
	_t cmix(c);
	auto ca = c.a;
	i64 d_start = area.x.min - area.x.max + 1;
	for (i64 i = area.y.min; i < area.y.max; i++)
	{
		double dd = (i - p.y) * (i - p.y) + dxdx0;
		double ab = ab0;
		_color* cc = &pixel(area.x.min, i);
		i64 d = d_start;
		while (d++ <= 0)
		{
			if (dd < rrmax)
			{
				if (dd > rrmin)
				{
					c.a = uchar(ca * (rrmax - dd) / drr);
					_b::mix2(c, *cc);
				}
				else cmix.mix(*cc);
			}
			cc++;
			dd += ab;
			ab += 2;
		}
	}
}

void _picture::fill_circle(_xy p, double r, _color c)
{
	if (r < 0.5 || c.a == 0) return;
	_iarea area(_iinterval(p.x - r, p.x + r), _iinterval(p.y - r, p.y + r));
	area &= drawing_area;
	if (area.empty()) return;
	if (c.a == 0xff)
	{
		if (transparent)
			((_picture_functions*)this)->fill_circle3<_color_substitution, _color_mixing>(area, p, r, c);
		else
			((_picture_functions*)this)->fill_circle3<_color_substitution, _color_overlay>(area, p, r, c);
	}
	else if (transparent)
		((_picture_functions*)this)->fill_circle3<_color_mixing, _color_mixing>(area, p, r, c);
	else
		((_picture_functions*)this)->fill_circle3<_color_overlay, _color_overlay>(area, p, r, c);
}

uint brighten(uint c)
{
	uchar* cc = (uchar*)(&c);
	cc[0] = (cc[0] <= 85) ? (cc[0] * 2) : (128 + cc[0] / 2);
	cc[1] = (cc[1] <= 85) ? (cc[1] * 2) : (128 + cc[1] / 2);
	cc[2] = (cc[2] <= 85) ? (cc[2] * 2) : (128 + cc[2] / 2);
	return c;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool _bitmap::resize(_isize wh)
{
	if (size == wh) return false;
	size = wh;
	drawing_area = size;
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
	font.lfCharSet        = DEFAULT_CHARSET; // набор символов
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
	data = nullptr;
}

void _bitmap::text(_ixy p, std::wstring_view s, int h, uint c, uint bg)
{
	auto text_area = size_text(s, h).move(p) & drawing_area;
	if (text_area.empty()) return;
	podg_font(h);
	podg_cc(c, bg);
	TextOutW(hdc, (int)p.x, (int)p.y, s.data(), (int)s.size());
}

void _bitmap::text(_ixy p, std::string_view s, int h, uint c, uint bg)
{
	auto text_area = size_text(s, h).move(p) & drawing_area;
	if (text_area.empty()) return;
	podg_font(h);
	podg_cc(c, bg);
	TextOutA(hdc, (int)p.x, (int)p.y, s.data(), (int)s.size());
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

std::wstring _bitmap::get_font_name()
{
	return font.lfFaceName;
}

void _bitmap::set_font(std::wstring_view name, bool bold)
{
	izm_font = true;
	font.lfWeight = (bold) ? 700 : 100;
	wcscpy_s(font.lfFaceName, name.data());
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

_bitmap& _bitmap::operator=(const _picture& copy)
{
	*((_picture*)this) = copy;
	return *this;
}

_bitmap& _bitmap::operator=(const _bitmap& copy)
{
	*((_picture*)this) = copy;
	return *this;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
