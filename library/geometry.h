﻿#pragma once

#include "basic.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

constexpr double de_i = 0.001; // для больших чисел - не правильно!

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct _xy;

struct _ixy // индекс, номер
{
	i64 x;
	i64 y;

	_ixy(i64 x_,    i64 y_)    : x(x_), y(y_) {}
	_ixy(i64 x_,    double y_) : x(x_), y(y_) { if ((y_ < 0) && (y != y_)) y--; }
	_ixy(double x_, i64 y_)    : x(x_), y(y_) { if ((x_ < 0) && (x != x_)) x--; }
	_ixy(double x_, double y_) : x(x_), y(y_) {	if ((x_ < 0) && (x != x_)) x--;	if ((y_ < 0) && (y != y_)) y--;	}

	inline operator _xy() const;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct _xy
{
	double x;
	double y;

	operator _ixy()         const { return { x, y }; }

	_xy operator-()         const { return { -x,  -y }; }

	_xy operator-(_xy b)    const { return { x - b.x, y - b.y }; }
	_xy operator+(_xy b)    const { return { x + b.x, y + b.y }; }

	_xy operator*(double b) const { return { x * b, y * b }; }

	void operator+=(_xy b)        { x += b.x; y += b.y; }
	void operator-=(_xy b)        { x -= b.x; y -= b.y; }

	void operator*=(double b)     { x *= b; y *= b; }
	void operator/=(double b)     { x /= b; y /= b; }

	double len()            const { return sqrt(x * x + y * y); }
	double len2()           const { return x * x + y * y; }

	double scalar(_xy b)    const { return x * b.x + y * b.y; }

	_xy rotation(double b)  const;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct _iarea;

struct _isize // [0...x), [0...y)
{
	i64 x = 0;
	i64 y = 0;

	bool empty()  const { return ((x <= 0) || (y <= 0)); }
	i64  square() const { return (empty()) ? 0 : (x * y); }
	_xy  center() const { return { x * 0.5, y * 0.5 }; }
	inline _iarea move(_ixy d) const;

	bool operator==(const _isize s) const { return ((x == s.x) && (y == s.y)) || (empty() && s.empty()); }
	bool operator!=(const _isize s) const { return !(*this == s); }
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct _interval;

struct _iinterval // [...)
{
	i64 min = 0;
	i64 max = 0;

	_iinterval() = default;
	_iinterval(i64 x) : min(x), max(x + 1) {}
	_iinterval(i64 min_, i64 max_) : min(min_), max(max_) {}
	_iinterval(i64 min_, double max_) : min(min_), max(max_) { if ((max_ > 0) || (max == max_)) max++; }
	_iinterval(double min_, i64 max_) : min(min_), max(max_) { if ((min_ < 0) && (min != min_)) min--; }
	_iinterval(double min_, double max_);

	void operator=(i64 b) { min = b; max = b + 1; }

	bool operator==(_iinterval b) const;
	bool operator!=(_iinterval b) const { return !(*this == b); }

	void operator&=(const _iinterval b) { if (b.min > min) min = b.min; if (b.max < max) max = b.max; }
	_iinterval operator&(const _iinterval b) const { return { std::max(min, b.min), std::min(max, b.max) }; }

	_interval operator/(const _iinterval b);

	i64  size()   const { return (min < max) ? (max - min) : 0; }
	bool empty()  const { return (max <= min); }
	i64  center() const { i64 s = min + max; if (s < 0) s--; return s >> 1; }
	i64  length() const { i64 r = max - min; return (r < 0) ? 0 : r; }
	_iinterval& operator << (i64 x);
	bool contains(i64 x) const { return (x >= min) && (x < max); }

	void clear() { min = max = 0; }
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct _iarea
{
	_iinterval x, y; // [...) [...)

	_iarea() = default;
	_iarea(_isize b) : x{ 0LL, b.x }, y{ 0LL, b.y } {}
	_iarea(_iinterval x_, _iinterval y_) : x(x_), y(y_) {}

	bool operator!=(_isize b) const;

	void operator&=(const _iarea& b) { x &= b.x; y &= b.y; }

	_iarea operator&(const _iarea& b) const { _iarea c(*this); c &= b; return c; }

	bool empty() const { return x.empty() || y.empty(); }
	void clear() { x.clear(); }
	_isize size() const { if (empty()) return { 0,0 }; return { x.max - x.min, y.max - y.min }; }

	_iarea move(_ixy d) const { return { {x.min + d.x, x.max + d.x}, {y.min + d.y, y.max + d.y} }; }
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct _interval // [...]
{
	double min = 1;
	double max = 0;

	_interval() = default;
	_interval(double x) : min(x), max(x) {}
	_interval(double min_, double max_) : min(min_), max(max_) {}
	_interval(_iinterval b) : min(b.min), max(b.max - de_i) {}

	operator _iinterval() const { return _iinterval(min, max); }

	void operator&=(const _interval& b) { if (b.min > min) min = b.min; if (b.max < max) max = b.max; }
	_interval operator&(const _interval& b) const { return { std::max(min, b.min), std::min(max, b.max) }; }

	double length() const { return (max > min) ? (max - min) : 0; }
	_interval& operator << (double x);
	bool empty() const { return (max < min); }
};

inline double operator*(const double a, const _interval interv)
{
	return interv.min + (interv.max - interv.min) * a;
}

inline double operator/(const double a, const _interval interv)
{
	return (a - interv.min) / (interv.max - interv.min);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct _area
{
	_interval x, y; // [...] [...]

	_area() = default;
	_area(const _area&) = default;
	_area(_interval x_, _interval y_) : x(x_), y(y_) {}
	_area(_isize b) : x{ 0.0, b.x - de_i }, y{ 0.0, b.y - de_i } {}
	_area(_iarea b) : x(b.x), y(b.y) {}
	_area(_xy b) : x{ b.x, b.x }, y{ b.y, b.y } {}

	void operator=(const _isize b) { x = { 0, b.x - de_i }; y = { 0, b.y - de_i }; }

	operator _iarea() const { return { x, y }; }

	bool operator==(const _area& b) const noexcept;
	bool operator<=(const _area& b) const noexcept;
	bool operator<(const _area& b)  const noexcept; // внутри, грани могут касаться, но не равно
	bool inside(const _area& b)     const noexcept; // внутри, грани не касаются!

	void operator&=(const _area& b) { x &= b.x; y &= b.y; }
	void operator+=(const _area& b) noexcept;

	_area operator&(const _area& b) const { _area c(*this); c &= b;	return c; }
	_area operator+(const _area& b) const { _area c(*this); c += b; return c; }

	bool empty() const { return (x.min > x.max) || (y.min > y.max); }
	void clear() { x = { 1.0, 0.0 }; }

	_area expansion(double b) const noexcept; // расширенная область во все стороны на b
	_area scaling(double b) const noexcept; // промасштабированная область во все стороны в b

	_xy center()       const { return { (x.max + x.min) * 0.5, (y.max + y.min) * 0.5 }; }
	_xy top_left()     const { return { x.min, y.min }; } // верхний левый угол
	_xy top_right()    const { return { x.max, y.min }; } // верхний правый угол
	_xy bottom_left()  const { return { x.min, y.max }; } // нижний левый угол
	_xy bottom_right() const { return { x.max, y.max }; } // нижний правый угол

	double radius(); // радиус вписанной окружности
	double min_length() { if (empty()) return 0.0; return std::min(x.max - x.min, y.max - y.min); } // минимальный размер

	_area move(_xy d) const { return { {x.min + d.x, x.max + d.x}, {y.min + d.y, y.max + d.y} }; }

	bool test(_xy b); // принадлежит ли точка области
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct _trans
{
	double scale = 1.0;
	_xy offset = { 0.0, 0.0 };

	_area operator()(const _area& b) const noexcept; // применение трансформации
	_xy  operator()(const _xy& b)    const noexcept; // применение трансформации
	double operator()(double b)      const { return scale * b; } // применение трансформации

	_trans operator*(_trans tr)      const noexcept; // сместить и промасштабировать
	_trans operator/(_trans tr)      const noexcept; // обратно сместить и промасштабировать

	void operator*=(_trans tr) { offset += tr.offset * scale; scale *= tr.scale; }
	void operator/=(_trans tr); // обратно сместить и промасштабировать
	bool operator!=(const _trans& b) const noexcept;

	_trans inverse()                 const noexcept; // обратная трансформация
	_xy inverse(_xy b)               const noexcept;
	_area inverse(const _area& b)    const noexcept;

	void MasToch(_xy b, double m); // промасштабировать вокруг точки
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct _segment
{
	_xy p1;
	_xy p2;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

uint test_line(_xy p1, _xy p2, _xy b);

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

_ixy::operator _xy() const
{
	return { x + 0.5, y + 0.5 };
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

_iarea _isize::move(_ixy d) const
{
	return { {d.x, d.x + x}, {d.y, d.y + y} };
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////