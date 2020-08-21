#pragma once

#include "mbasic.h"

#include <ostream>

// Цвет в формате bgra.
struct _color
{
	uint8 b, g, r, a;

	constexpr _color(uint8 r, uint8 g, uint8 b, uint8 a = 255) : b(b), g(g), r(r), a(a) {}
	constexpr _color(uint i = 0xff000000) : b(i), g(i >> 8), r(i >> 16), a(i >> 24) {}

	constexpr operator uint() const { return *(uint*)this; }

	_color(const _color& copy) = default;
	_color& operator=(const _color& copy) = default;

	[[nodiscard]] _color sub_a(double k) const { return {r, g, b, uchar(a * k)}; } // умножить прозрачность на [0..1]
	[[nodiscard]] _color sub_a(uint8 k) const { return {r, g, b, uchar((a * k) >> 8)}; } // умножить прозрачность на [0..255]

	[[nodiscard]] constexpr _color with_r(uint8 v) const { return {v, g, b, a}; }
	[[nodiscard]] constexpr _color with_g(uint8 v) const { return {r, v, b, a}; }
	[[nodiscard]] constexpr _color with_b(uint8 v) const { return {r, g, v, a}; }
	[[nodiscard]] constexpr _color with_a(uint8 v) const { return {r, g, b, v}; }

	void blend(_color cc) // смешать наложением
	{
		int kk = 256 - cc.a;

		b = (cc.b * cc.a + b * kk) >> 8;
		g = (cc.g * cc.a + g * kk) >> 8;
		r = (cc.r * cc.a + r * kk) >> 8;
	}

	struct rgb_cache
	{ // кеш смешивания
		int r, g, b, k;
	};
	[[nodiscard]] rgb_cache begin_blend() const // кешировать цвет для более быстрого смешивания
	{
		return { r * a, g * a, b * a, 256 - a };
	}
	void blend(rgb_cache cc) // смешать наложением (используя кеш)
	{
		b = (b * cc.k + cc.b) >> 8;
		g = (g * cc.k + cc.g) >> 8;
		r = (r * cc.k + cc.r) >> 8;
	}

	struct rgba_cache
	{ // кеш смешивания, на прозрачном холсте
		int r, g, b, k;
	};
	void blend_rgba(_color cc) // смешать наложением, на прозрачном холсте
	{
		int kk = 255 - cc.a;
		int k2 = 256 - kk;
		int b2 = cc.b * k2 * 255;
		int g2 = cc.g * k2 * 255;
		int r2 = cc.r * k2 * 255;

		int kk_ = 255 - a;
		int k2_ = (256 - kk_) * kk;
		int znam = 65536 - kk * kk_;
		b = (b * k2_ + b2) / znam;
		g = (g * k2_ + g2) / znam;
		r = (r * k2_ + r2) / znam;
		a = 255 - ((kk_ * kk) >> 8);
	}
	[[nodiscard]] rgba_cache begin_blend_rgba() const // кешировать цвет для более быстрого смешивания, на прозрачном холсте
	{
		int kk = 255 - a;
		int k2 = (256 - kk) * 255;
		return { r * k2, g * k2, b * k2, kk };
	}
	void blend(rgba_cache cc) // смешать наложением (используя кеш), на прозрачном холсте
	{
		int k = 255 - a;
		int k2 = (256 - k) * cc.k;
		int znam = 65536 - cc.k * k;
		b = (b * k2 + cc.b) / znam;
		g = (g * k2 + cc.g) / znam;
		r = (r * k2 + cc.r) / znam;
		a = 255 - ((k * cc.k) >> 8);
	}
};

[[nodiscard]] inline constexpr _color operator*(_color& c, double k) // безопасно умножить на коэффицент
{
	const auto fix = [](double v) -> uchar { return uchar(v) > 255 ? 255 : uchar(v); };
	return {fix(c.r * k), fix(c.g * k), fix(c.b * k), c.a};
}

inline std::ostream& operator<<(std::ostream& o, _color c)
{
	return o << int(c.r) << ", " << int(c.g) << ", " << int(c.b) << ", " << int(c.a);
}
inline std::wostream& operator<<(std::wostream& o, _color c)
{
	return o << int(c.r) << L", " << int(c.g) << L", " << int(c.b) << L", " << int(c.a);
}
