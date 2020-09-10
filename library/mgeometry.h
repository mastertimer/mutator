#pragma once

#include <cmath>
#include <ostream>

#include "mbasic.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct _ixy // индекс, номер
{
	i8 x, y;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct _xy
{
	double x, y;

	operator _ixy() const noexcept
	{
		_ixy res{ (i8)x, (i8)y };
		if (x < 0.0) if (x != res.x) res.x--;
		if (y < 0.0) if (y != res.y) res.y--;
		return res;
	}

	_xy operator-()         const noexcept { return { -x,  -y }; }

	_xy operator-(_xy b)  const noexcept { return { x - b.x, y - b.y }; }
	_xy operator+(_xy b)  const noexcept { return { x + b.x, y + b.y }; }

	_xy operator*(double b) const noexcept { return { x * b, y * b }; }

	void operator+=(_xy b)        noexcept { x += b.x; y += b.y; }

	void operator*=(double b)       noexcept { x *= b; y *= b; }
	void operator/=(double b)       noexcept { x /= b; y /= b; }

	double len()              const noexcept { return sqrt(x * x + y * y); }  // длина вектора
	double len2()             const noexcept { return x * x + y * y; } // квадрат длины вектора

	double scalar(_xy b)    const noexcept { return x * b.x + y * b.y; } // скалярное произведение

	_xy rotation(double b) const noexcept; // поворот
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct _area2i;

struct _size2i // [0...x), [0...y)
{
	i8 x = 0, y = 0;

	bool    empty()   const noexcept { return ((x <= 0) || (y <= 0)); }
	i8      square()  const noexcept { return ((x <= 0) || (y <= 0)) ? 0 : (x * y); }
	_xy   center()  const noexcept { return { x * 0.5, y * 0.5 }; }
	_size2i correct() const noexcept { if ((x <= 0) || (y <= 0)) return { 0, 0 }; return { x, y }; } // для удобства

	bool operator==(_size2i s) const noexcept { return (x == s.x) && (y == s.y); }
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/*struct _size2 // [0...x], [0...y]
{
	_size x = 0.0, y = 0.0;

	bool empty()   const noexcept { return (x < 0) || (y < 0); }
	_coo2 center() const noexcept { return { x * 0.5, y * 0.5 }; }
};*/

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct _areai // [...)
{
	i8 min = 0;
	i8 max = 0;

	void operator&=(const _areai& b) noexcept { if (b.min > min) min = b.min; if (b.max < max) max = b.max; }

	i8 size() const noexcept { return (min < max) ? (max - min) : 0; }
	bool empty()  const noexcept { return (max <= min); }
};

struct _area2i
{
	_areai x, y; // [...) [...)

	_area2i() = default;
	_area2i(_size2i b) : x{ 0, b.x }, y{ 0, b.y } {}
	_area2i(_areai x_, _areai y_) : x(x_), y(y_) {}

	void operator&=(const _area2i& b) noexcept { x &= b.x; y &= b.y; }

	_area2i operator&(const _area2i& b) const noexcept { _area2i c(*this); c &= b; return c; }

	bool empty() const noexcept { return (x.min >= x.max) || (y.min >= y.max); }
	_size2i size() const noexcept { if (empty()) return { 0,0 }; return { x.max - x.min, y.max - y.min }; }

	_area2i move(_ixy d) const noexcept { return { {x.min + d.x, x.max + d.x}, {y.min + d.y, y.max + d.y} }; }
};

inline _area2i move(_size2i b, _ixy d) { return { {d.x, b.x + d.x}, {d.y, b.y + d.y} }; }

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct _area // [...]
{
	double min = 1;
	double max = 0;

	operator _areai() const noexcept
	{
		_areai res{ (i8)min, (i8)max };
		if (min < 0.0) if (min != res.min) res.min--;
		if ((max >= 0.0)||(max == res.max)) res.max++;
		return res;
	}

	double operator()(double k) const noexcept { return min + (max - min) * k; }; // [0..1 -> min..max]
	double get_k(double a) const noexcept { return (a - min) / (max - min); } // [min..max -> 0..1]

	void operator&=(const _area& b) noexcept { if (b.min > min) min = b.min; if (b.max < max) max = b.max; }

	double length() { return (max > min) ? (max - min) : 0; }
};

constexpr double de_i = 0.0001; // для больших чисел - не правильно!

struct _area2
{
	_area x, y; // [...] [...]

	_area2() = default;
	_area2(const _area2&) = default;
	_area2(_area x_, _area y_) : x(x_), y(y_) {}
	_area2(_size2i b) : x{ 0.0, b.x - de_i }, y{ 0.0, b.y - de_i } {}
	_area2(_xy b) : x{ b.x, b.x }, y{ b.y, b.y } {}

	void operator=(const _size2i b) noexcept { x = { 0, b.x - de_i }; y = { 0, b.y - de_i }; }

	operator _area2i() const noexcept { return { x, y }; }

	bool operator<=(const _area2& b) const noexcept;
	bool operator<(const _area2& b) const noexcept; // внутри, грани могут касаться, но не равно
	bool inside(const _area2& b) const noexcept;    // внутри, грани не касаются!

	void operator&=(const _area2& b) noexcept { x &= b.x; y &= b.y; }
	void operator+=(const _area2& b) noexcept;

	_area2 operator&(const _area2& b) const noexcept { _area2 c(*this); c &= b;	return c; }
	_area2 operator+(const _area2& b) const noexcept { _area2 c(*this); c += b; return c; }

	bool empty() const noexcept { return (x.min > x.max) || (y.min > y.max); }
	void clear() noexcept { x = { 1.0, 0.0 }; }

	_area2 expansion(double b) const noexcept; // расширенная область во все стороны

	_xy center()       const noexcept { return { (x.max + x.min) * 0.5, (y.max + y.min) * 0.5 }; }
	_xy top_left()     const noexcept { return { x.min, y.min }; } // верхний левый угол
	_xy top_right()    const noexcept { return { x.max, y.min }; } // верхний правый угол
	_xy bottom_left()  const noexcept { return { x.min, y.max }; } // нижний левый угол
	_xy bottom_right() const noexcept { return { x.max, y.max }; } // нижний правый угол

	double radius(); // радиус вписанной окружности
	double min_length() { if (empty()) return 0.0; return std::min(x.max - x.min, y.max - y.min); } // минимальный размер

	_area2 move(_xy d) const noexcept { return { {x.min + d.x, x.max + d.x}, {y.min + d.y, y.max + d.y} }; }

	bool test(_xy b); // принадлежит ли точка области
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct _trans
{
	double scale = 1.0;
	_xy offset = { 0.0, 0.0 };

	_area2 operator()(const _area2& b) const noexcept; // применение трансформации
	_xy  operator()(const _xy& b) const noexcept; // применение трансформации
	double operator()(double b) const noexcept { return scale * b; } // применение трансформации

	_trans operator*(_trans tr) const noexcept;  // сместить и промасштабировать
	_trans operator/(_trans tr) const noexcept;  // обратно сместить и промасштабировать

	void operator*=(_trans tr) noexcept { offset += tr.offset * scale; scale *= tr.scale; }
	void operator/=(_trans tr); // обратно сместить и промасштабировать
	bool operator!=(const _trans& b) const noexcept;

	_trans inverse() const noexcept;   // обратная трансформация
	_xy inverse(_xy b) const noexcept;
	_area2 inverse(const _area2& b) const noexcept;

	void MasToch(_xy b, double m); // промасштабировать вокруг точки
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

uint test_line(_xy p1, _xy p2, _xy b);

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
