#include "renderer.h"

#include <algorithm>
#include <cstring>

_renderer& _renderer::pixel(int x, int y, _color c)
{
	if (x < 0 || x >= size.x || y < 0 || y >= size.y) return *this;
	if (c.a == 255)
		px(x, y) = c;
	else
		px(x, y).blend(c);
	return *this;
}

_renderer& _renderer::clear(_color c)
{
	if (_2x_dv)
	{
		const auto bgn = (uint*)begin();
		std::fill(bgn, bgn + _size_p / 2, c);
		memcpy(bgn + _size_p / 2, bgn, _size_b / 2);
	}
	else
		std::fill(begin(), end(), c);

	return *this;
}

_renderer& _renderer::fill(_color c)
{
	if (c.a == 255)
		clear(c);
	else
	{
		auto cc = c.begin_blend();
		std::for_each(begin(), end(), [cc](auto&& v) { v.blend(cc); });
	}
	return *this;
}

void _renderer::_fill_rectJ(vec2<int> p1, vec2<int> p2, _color c)
{
	const int  d    = p2.x + 1 - p1.x;
	const auto endp = &px(p1.x, p2.y + 1);
	if (c.a == 255)
		for (auto it = &px(p1.x, p1.y); it != endp; it += size.x) std::fill(it, it + d, c);
	else
	{
		auto cc = c.begin_blend();
		for (auto it = &px(p1.x, p1.y); it != endp; it += size.x)
			std::for_each(it, it + d, [cc](auto&& v) { v.blend(cc); });
	}
}

void _renderer::x_lineF(int beginx, int beginy, int endx, _color c)
{
	if (endx - beginx < 0) return;
	std::fill(&px(beginx, beginy), &px(endx + 1, beginy), c);
}
void _renderer::x_line(int beginx, int beginy, int endx, _color c)
{
	if (endx - beginx < 0) return;
	const auto cc  = c.begin_blend();
	const auto pix = &px(beginx, beginy);
	std::for_each(pix, pix + ((int64)endx + 1LL - (int64)beginx), [cc](auto&& v) { v.blend(cc); });
}
void _renderer::x_line(int beginx, int beginy, int endx, _color::rgb_cache cc)
{
	if (endx - beginx < 0) return;
	const auto pix = &px(beginx, beginy);
	std::for_each(pix, pix + ((int64)endx + 1LL - (int64)beginx), [cc](auto&& v) { v.blend(cc); });
}

void _renderer::y_lineF(int beginx, int beginy, int endy, _color c)
{
	if (endy - beginy < 0) return;
	const auto endp = &px(beginx, endy + 1);
	for (auto it = &px(beginx, beginy); it != endp; it += size.x) *it = c;
}
void _renderer::y_line(int beginx, int beginy, int endy, _color c)
{
	if (endy - beginy < 0) return;
	const auto cc      = c.begin_blend();
	const auto pix     = &px(beginx, beginy);
	const auto pix_end = pix + ((int64)endy + 1LL - (int64)beginy) * size.x;
	for (auto it = pix; it != pix_end; it += size.x) it->blend(cc);
}
void _renderer::y_line(int beginx, int beginy, int endy, _color::rgb_cache cc)
{
	if (endy - beginy < 0) return;
	const auto pix     = &px(beginx, beginy);
	const auto pix_end = pix + ((int64)endy + 1LL - (int64)beginy) * size.x;
	for (auto it = pix; it != pix_end; it += size.x) it->blend(cc);
}

void _renderer::line(vec2f p1, vec2f p2, _color c, double w)
{
	if (c.a == 0) return;

	if (p1.y > p2.y) std::swap(p1, p2);
	w *= 0.5; // половина толщины
	int xmin = int(std::min(p1.x, p2.x) - w);
	int xmax = int(std::max(p1.x, p2.x) + w);
	int ymin = int(p1.y - w);
	int ymax = int(p2.y + w);

	if (xmin < 0) xmin = 0;
	if (ymin < 0) ymin = 0;
	if (xmax >= size.x) xmax = size.x - 1;
	if (ymax >= size.y) ymax = size.y - 1;
	if ((xmax < xmin) || (ymax < ymin)) return; // активная область за экраном

	const double dx = p2.x - p1.x;
	double       dy = p2.y - p1.y;
	if (std::abs(dy) < 0.001) dy = 0.001; // костыль против горизонтальной линии
	const double ll_1 = 1.0 / sqrt(dx * dx + dy * dy);
	if (ll_1 > 2) return; // слишком короткая линия

	const double x2y1_y2x1 = p2.x * p1.y - p2.y * p1.x;
	double       rcentrscreen =
	    (dy * size.x * 0.5 - dx * size.y * 0.5 + x2y1_y2x1) * ll_1; // расстояние от линии до центра эрана
	if (rcentrscreen < 0) rcentrscreen = -rcentrscreen;
	if (rcentrscreen - w > ((double)size.x + size.y) * 0.5) return; // линия не задевает экран

	const double dxdy = dx / dy;
	const double db   = w * sqrt(dxdy * dxdy + 1.0);
	double       xx1  = p1.x + (ymin - p1.y) * dxdy; // х верхней точки пересечения
	double       xx2  = xx1 + dxdy;                  // x нижней точки пересечения
	if (dxdy < 0) std::swap(xx1, xx2);
	xx1 -= db;
	xx2 += db;

	const double drr   = dy * ll_1;
	const double temp1 = dy * 0.5 - dx * 0.5 + x2y1_y2x1;
	const auto   cc    = c.begin_blend();

	for (int j = ymin; j <= ymax; j++, xx1 += dxdy, xx2 += dxdy)
	{ // цикл по линиям
		int xx1i = (int)xx1;
		int xx2i = (int)xx2;
		if (xx1i < xmin) xx1i = xmin;
		if (xx2i > xmax) xx2i = xmax;
		if (xx2i < xx1i) continue;
		double rr = (dy * xx1i - dx * j + temp1) * ll_1;
		auto   c2 = &px(xx1i, j);
		for (int i = xx1i; i <= xx2i; i++, rr += drr, ++c2)
		{
			const double rra = ((rr < 0) ? -rr : rr) - 0.5;
			if (rra >= w) continue; // пиксель за границей точки
			if (rra + 1.0 <= w)     // пиксель полностью внутри линии
				c2->blend(cc);
			else
				c2->blend(c.sub_a((rra >= -w) ? (w - rra) : (2.0 * w)));
		}
	}
}

void _renderer::lineF(vec2<int> p1, vec2<int> p2, _color c)
{
	if (c.a == 0) return; // полностью прозрачная
	if (p1.y == p2.y)     // горизонтальная линия
	{
		if (p1.x > p2.x) std::swap(p1.x, p2.x);
		if (p1.x < 0) p1.x = 0;
		if (p2.x >= size.x) p2.x = size.x - 1;

		if ((p1.x > p2.x) || (p1.y < 0) || (p1.y >= size.y)) return;

		if (c.a == 0xFF)
			x_lineF(p1.x, p1.y, p2.x, c);
		else
			x_line(p1.x, p1.y, p2.x, c);
		return;
	}
	if (p1.x == p2.x) // вертикальная линия
	{
		if (p1.y > p2.y) std::swap(p1.y, p2.y);
		if (p1.y < 0) p1.y = 0;
		if (p2.y >= size.y) p2.y = size.y - 1;
		if ((p1.y > p2.y) || (p1.x < 0) || (p1.x >= size.x)) return;

		if (c.a == 0xFF)
			y_lineF(p1.x, p1.y, p2.y, c);
		else
			y_line(p1.x, p1.y, p2.y, c);
		return;
	}
	const int d = (abs(p1.x - p2.x) >= abs(p1.y - p2.y)) ? abs(p1.x - p2.x) : abs(p1.y - p2.y);
	if (d == 0)
	{
		if ((p1.x >= 0) && (p1.x < size.x) && (p1.y >= 0) && (p1.y < size.y)) data[p1.y * size.x + p1.x] = c;
		return;
	}
	const int64 dx = (((int64)p2.x - p1.x) << 32) / d;
	const int64 dy = (((int64)p2.y - p1.y) << 32) / d;
	int64       x  = ((p1.x * 2LL + 1LL) << 31) - dx;
	int64       y  = ((p1.y * 2LL + 1LL) << 31) - dy;
	int         n  = 0;
	int         k  = d;
	if (p1.x < 0)
	{
		if (dx <= 0) return;
		n = (int)(-x / dx);
	}
	else if (p1.x >= size.x)
	{
		if (dx >= 0) return;
		n = (int)((((size.x - 0LL) << 32) - x) / dx);
	}
	if (p2.x < 0)
	{
		if (dx >= 0) return;
		k = (int)(-x / dx - 1);
	}
	else if (p2.x >= size.x)
	{
		if (dx <= 0) return;
		k = (int)((((size.x - 1LL) << 32) - x) / dx);
	}
	if (p1.y < 0)
	{
		if (dy <= 0) return;
		const int n2 = (int)(-y / dy);
		if (n2 > n) n = n2;
	}
	else if (p1.y >= size.y)
	{
		if (dy >= 0) return;
		const int n2 = (int)((((size.y - 0LL) << 32) - y) / dy);
		if (n2 > n) n = n2;
	}
	if (p2.y < 0)
	{
		if (dy >= 0) return;
		const int k2 = (int)(-y / dy - 1);
		if (k2 < k) k = k2;
	}
	else if (p2.y >= size.y)
	{
		if (dy <= 0) return;
		const int k2 = (int)((((size.y - 1LL) << 32) - y) / dy);
		if (k2 < k) k = k2;
	}
	x += dx * n;
	y += dy * n;
	if (c.a == 0xFF)
		for (int i = k - n; i >= 0; i--) px((x += dx) >> 32, (y += dy) >> 32) = c;
	else
		for (int i = k - n; i >= 0; i--) px((x += dx) >> 32, (y += dy) >> 32).blend(c);
}

_renderer& _renderer::rect(vec2i p1, vec2i p2, _color c)
{
	if (p1.y > p2.y) std::swap(p1.y, p2.y);
	lineF(p1, p2.with_y(p1), c);
	if (p2.y - p1.y > 0)
	{
		lineF(p1.with_y(p2), p2, c);
		if (p2.y - p1.y > 1)
		{
			lineF({p1.x, p1.y + 1}, {p1.x, p2.y - 1}, c);
			lineF({p2.x, p1.y + 1}, {p2.x, p2.y - 1}, c);
		}
	}

	return *this;
}
_renderer& _renderer::rect(_areai_old r, _color c) { return rect({r.x.min, r.y.min}, {r.x.max, r.x.max}, c); }

_renderer& _renderer::rectD(vec2f p1, vec2f p2, _color c)
{
	// TODO: углы
	if (p1 == p1.drop_round() && p2 == p2.drop_round()) return rect(p1, p2, c);
	auto ma_x = [this](int i) { return i > size.x ? size.x : i; };
	auto ma_y = [this](int i) { return i > size.y ? size.y : i; };
	auto mi   = [](int i) { return i < 0 ? 0 : i; };

	vec2<double>::sort(p1, p2);
	if (p2.x < 0 || p2.y < 0 || p1.x >= size.x || p1.y >= size.y) return *this;

	const vec2f ti1 = p1 - p1.round_min();
	const vec2f to2 = p2 - p2.round_min();
	const vec2f to1 = 1.0 - ti1;
	const vec2f ti2 = 1.0 - to2;

	const vec2i pi1 = p1 + 1.0;
	const vec2i pi2 = p2;
	const vec2i po1 = p1;
	const vec2i po2 = p2 + 1.0;

	if (po1.y >= 0) x_line(mi(po1.x + 1), mi(po1.y), ma_x(po2.x - 1), c.sub_a(to1.y));
	if (pi1.y >= 0) x_line(mi(pi1.x + 1), mi(pi1.y), ma_x(pi2.x - 1), c.sub_a(ti1.y));
	if (po2.y < size.y) x_line(mi(po1.x + 1), mi(po2.y), ma_x(po2.x - 1), c.sub_a(to2.y));
	if (pi2.y < size.y) x_line(mi(pi1.x + 1), mi(pi2.y), ma_x(pi2.x - 1), c.sub_a(ti2.y));

	if (po1.x >= 0) y_line(mi(po1.x), mi(po1.y + 1), ma_y(po2.y - 1), c.sub_a(to1.x));
	if (pi1.x >= 0) y_line(mi(pi1.x), mi(pi1.y), ma_y(pi2.y), c.sub_a(ti1.x));
	if (po2.x < size.x) y_line(mi(po2.x), mi(po1.y + 1), ma_y(po2.y - 1), c.sub_a(to2.x));
	if (pi2.x < size.x) y_line(mi(pi2.x), mi(pi1.y), ma_y(pi2.y), c.sub_a(ti2.x));

	pixel(po1.x, po1.y, c.sub_a(to1.x).sub_a(to1.y));
	pixel(po1.x, po2.y, c.sub_a(to1.x).sub_a(to2.y));
	pixel(po2.x, po1.y, c.sub_a(to2.x).sub_a(to1.y));
	pixel(po2.x, po2.y, c.sub_a(to2.x).sub_a(to2.y));

	return *this;
}
_renderer& _renderer::rectD(_area_old r, _color c) { return rectD({r.x.min, r.y.min}, {r.x.max, r.x.max}, c); }

_renderer& _renderer::fill_rect(vec2i p1, vec2i p2, _color c)
{
	vec2<int>::sort(p1, p2);
	if (p2.x < 0 || p2.y < 0 || p1.x >= size.x || p1.y >= size.y) return *this;
	p1.bound_lt(0, 0);
	p2.bound_rd(size.x - 1, size.y - 1);

	_fill_rectJ(p1, p2, c);
	return *this;
}

_renderer& _renderer::fill_rectD(vec2f p1, vec2f p2, _color c)
{
	if (p1 == p1.drop_round() && p2 == p2.drop_round()) return fill_rect(p1, p2, c);

	vec2f::sort(p1, p2);
	if (p2.x < 0 || p2.y < 0 || p1.x > (double)size.x - 1.0 || p1.y > (double)size.y - 1.0) return *this;
	p1.bound_lt(0, 0);
	p2.bound_rd(double(size.x - 1LL), double(size.y - 1LL));

	const vec2<double> t1  = 1.0 - (p1 - (vec2f)(vec2i)p1);
	const vec2<double> t2  = p2 - (vec2f)(vec2i)p2;
	const vec2<int>    pi1 = p1.round_max_cast<int>();
	const vec2<int>    pi2 = p2;
	const vec2<int>    pb1 = p1;
	const vec2<int>    pb2 = p2.round_max_cast<int>();

	_fill_rectJ(pi1, pi2, c);

	if (p2.x - p1.x <= 1)
	{
		if (p2.y - p1.y <= 1)
		{
			const _color cc = c.sub_a(p2.x - p1.x).sub_a(p2.y - p1.y);
			pixel(pb1.x, pb1.y, cc);
			return *this;
		}
		const _color cc = c.sub_a(p2.x - p1.x);
		y_line(pb1.x, pb1.y, pb2.y, cc);
		return *this;
	}
	if (p2.y - p1.y <= 1)
	{
		const _color cc = c.sub_a(p2.y - p1.y);
		x_line(pb1.x, pb1.y, pb2.x, cc);
		return *this;
	}

	const bool      z1x = pi1.x != pb1.x, z1y = pi1.y != pb1.y, z2x = pi2.x != pb2.x, z2y = pi2.y != pb2.y;
	const vec2<int> pl1 = {z1x ? pb1.x + 1 : pb1.x, z1y ? pb1.y + 1 : pb1.y};
	const vec2<int> pl2 = pb2;

	const _color c_l  = c.sub_a(t1.x);
	const _color c_t  = c.sub_a(t1.y);
	const _color c_r  = c.sub_a(t2.x);
	const _color c_b  = c.sub_a(t2.y);
	const _color c_lt = c_l.sub_a(c_t.a);
	const _color c_rt = c_r.sub_a(c_t.a);
	const _color c_lb = c_l.sub_a(c_b.a);
	const _color c_rb = c_r.sub_a(c_b.a);

	if (z1x && z1y)
	{
		y_line(pb1.x, pl1.y, pl2.y, c_l);
		x_line(pl1.x, pb1.y, pl2.x, c_t);
		px(pb1.x, pb1.y).blend(c_lt);
	}
	else if (z1x)
		y_line(pb1.x, pl1.y, pl2.y, c_l);
	else if (z1y)
		x_line(pl1.x, pb1.y, pl2.x, c_t);

	if (z2x && z2y)
	{
		y_line(pb2.x, pl1.y, pl2.y, c_r);
		x_line(pl1.x, pb2.y, pl2.x, c_b);
		px(pb2.x, pb2.y).blend(c_rb);
	}
	else if (z2x)
		y_line(pb2.x, pl1.y, pl2.y, c_r);
	else if (z2y)
		x_line(pl1.x, pb2.y, pl2.x, c_b);

	if (z2x && z1y) px(pb2.x, pb1.y).blend(c_rt);
	if (z1x && z2y) px(pb1.x, pb2.y).blend(c_lb);

	return *this;
}

_renderer& _renderer::image(int x, int y, int w, int h, _color* src, vec2i src_size, bool transparent, int src_x,
                            int src_y)
{
	if (x < 0)
	{
		w += x;
		src_x -= x;
		x = 0;
	}
	if (y < 0)
	{
		h += y;
		src_y -= y;
		y = 0;
	}
	if (src_x < 0)
	{
		w += src_x;
		x -= src_x;
		src_x = 0;
	}
	if (src_y < 0)
	{
		h += src_y;
		y -= src_y;
		src_y = 0;
	}
	if ((x >= size.x) || (y >= size.y)) return *this;
	if ((src_x >= src_size.x) || (src_y >= src_size.y)) return *this;
	if (x + w > size.x) w = size.x - x;
	if (y + h > size.y) h = size.y - y;
	if (src_x + w > src_size.x) w = src_size.x - src_x;
	if (src_y + h > src_size.y) h = src_size.y - src_y;
	if ((w <= 0) || (h <= 0)) return *this;
	if (!transparent)
	{
		for (int j = 0; j < h; j++)
			memcpy(data + ((int64)y + j) * size.x + x, src + ((int64)src_y + j) * src_size.x + src_x, w * sizeof(uint));
	}
	else
	{
		for (int64 j = 0; j < h; j++)
		{
			auto       it  = &src[(src_y + j) * src_size.x + src_x];
			const auto ite = &src[(src_y + j) * src_size.x + src_x + w];
			for (auto itd = &data[(y + j) * size.x + x]; it != ite; ++it, ++itd) itd->blend(*it);
		}
	}
	return *this;
}
