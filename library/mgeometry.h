#pragma once

#include "mbasic.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct _ixy // индекс, номер
{
	i64 x, y;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct _xy
{
	double x, y;

	operator _ixy() const noexcept
	{
		_ixy res{ (i64)x, (i64)y };
		if (x < 0.0) if (x != res.x) res.x--;
		if (y < 0.0) if (y != res.y) res.y--;
		return res;
	}

	_xy operator-()         const noexcept { return { -x,  -y }; }

	_xy operator-(_xy b)    const noexcept { return { x - b.x, y - b.y }; }
	_xy operator+(_xy b)    const noexcept { return { x + b.x, y + b.y }; }

	_xy operator*(double b) const noexcept { return { x * b, y * b }; }

	void operator+=(_xy b)        noexcept { x += b.x; y += b.y; }

	void operator*=(double b)     noexcept { x *= b; y *= b; }
	void operator/=(double b)     noexcept { x /= b; y /= b; }

	double len()            const noexcept { return sqrt(x * x + y * y); }  // длина вектора
	double len2()           const noexcept { return x * x + y * y; } // квадрат длины вектора

	double scalar(_xy b)    const noexcept { return x * b.x + y * b.y; } // скалярное произведение

	_xy rotation(double b)  const noexcept; // поворот
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct _iarea;

struct _isize // [0...x), [0...y)
{
	i64 x = 0, y = 0;

	bool   empty()   const noexcept { return ((x <= 0) || (y <= 0)); }
	i64     square()  const noexcept { return ((x <= 0) || (y <= 0)) ? 0 : (x * y); }
	_xy    center()  const noexcept { return { x * 0.5, y * 0.5 }; }
	_isize correct() const noexcept { if ((x <= 0) || (y <= 0)) return { 0, 0 }; return { x, y }; } // для удобства

	bool operator==(_isize s) const noexcept { return (x == s.x) && (y == s.y); }
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/*struct _size2 // [0...x], [0...y]
{
	_size x = 0.0, y = 0.0;

	bool empty()   const noexcept { return (x < 0) || (y < 0); }
	_coo2 center() const noexcept { return { x * 0.5, y * 0.5 }; }
};*/

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct _iinterval // [...)
{
	i64 min = 0;
	i64 max = 0;

	void operator&=(const _iinterval& b) noexcept { if (b.min > min) min = b.min; if (b.max < max) max = b.max; }

	i64 size()    const noexcept { return (min < max) ? (max - min) : 0; }
	bool empty() const noexcept { return (max <= min); }
};

struct _iarea
{
	_iinterval x, y; // [...) [...)

	_iarea() = default;
	_iarea(_isize b) : x{ 0, b.x }, y{ 0, b.y } {}
	_iarea(_iinterval x_, _iinterval y_) : x(x_), y(y_) {}

	void operator&=(const _iarea& b) noexcept { x &= b.x; y &= b.y; }

	_iarea operator&(const _iarea& b) const noexcept { _iarea c(*this); c &= b; return c; }

	bool empty() const noexcept { return (x.min >= x.max) || (y.min >= y.max); }
	_isize size() const noexcept { if (empty()) return { 0,0 }; return { x.max - x.min, y.max - y.min }; }

	_iarea move(_ixy d) const noexcept { return { {x.min + d.x, x.max + d.x}, {y.min + d.y, y.max + d.y} }; }
};

inline _iarea move(_isize b, _ixy d) { return { {d.x, b.x + d.x}, {d.y, b.y + d.y} }; }

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct _interval // [...]
{
	double min = 1;
	double max = 0;

	operator _iinterval() const noexcept
	{
		_iinterval res{ (i64)min, (i64)max };
		if (min < 0.0) if (min != res.min) res.min--;
		if ((max >= 0.0)||(max == res.max)) res.max++;
		return res;
	}

	double operator()(double k) const noexcept { return min + (max - min) * k; }; // [0..1 -> min..max]
	double get_k(double a) const noexcept { return (a - min) / (max - min); } // [min..max -> 0..1]

	void operator&=(const _interval& b) noexcept { if (b.min > min) min = b.min; if (b.max < max) max = b.max; }

	double length() { return (max > min) ? (max - min) : 0; }
};

constexpr double de_i = 0.0001; // для больших чисел - не правильно!

struct _area
{
	_interval x, y; // [...] [...]

	_area() = default;
	_area(const _area&) = default;
	_area(_interval x_, _interval y_) : x(x_), y(y_) {}
	_area(_isize b) : x{ 0.0, b.x - de_i }, y{ 0.0, b.y - de_i } {}
	_area(_xy b) : x{ b.x, b.x }, y{ b.y, b.y } {}

	void operator=(const _isize b) noexcept { x = { 0, b.x - de_i }; y = { 0, b.y - de_i }; }

	operator _iarea() const noexcept { return { x, y }; }

	bool operator<=(const _area& b) const noexcept;
	bool operator<(const _area& b) const noexcept; // внутри, грани могут касаться, но не равно
	bool inside(const _area& b) const noexcept;    // внутри, грани не касаются!

	void operator&=(const _area& b) noexcept { x &= b.x; y &= b.y; }
	void operator+=(const _area& b) noexcept;

	_area operator&(const _area& b) const noexcept { _area c(*this); c &= b;	return c; }
	_area operator+(const _area& b) const noexcept { _area c(*this); c += b; return c; }

	bool empty() const noexcept { return (x.min > x.max) || (y.min > y.max); }
	void clear() noexcept { x = { 1.0, 0.0 }; }

	_area expansion(double b) const noexcept; // расширенная область во все стороны

	_xy center()       const noexcept { return { (x.max + x.min) * 0.5, (y.max + y.min) * 0.5 }; }
	_xy top_left()     const noexcept { return { x.min, y.min }; } // верхний левый угол
	_xy top_right()    const noexcept { return { x.max, y.min }; } // верхний правый угол
	_xy bottom_left()  const noexcept { return { x.min, y.max }; } // нижний левый угол
	_xy bottom_right() const noexcept { return { x.max, y.max }; } // нижний правый угол

	double radius(); // радиус вписанной окружности
	double min_length() { if (empty()) return 0.0; return std::min(x.max - x.min, y.max - y.min); } // минимальный размер

	_area move(_xy d) const noexcept { return { {x.min + d.x, x.max + d.x}, {y.min + d.y, y.max + d.y} }; }

	bool test(_xy b); // принадлежит ли точка области
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct _trans
{
	double scale = 1.0;
	_xy offset = { 0.0, 0.0 };

	_area operator()(const _area& b) const noexcept; // применение трансформации
	_xy  operator()(const _xy& b) const noexcept; // применение трансформации
	double operator()(double b) const noexcept { return scale * b; } // применение трансформации

	_trans operator*(_trans tr) const noexcept;  // сместить и промасштабировать
	_trans operator/(_trans tr) const noexcept;  // обратно сместить и промасштабировать

	void operator*=(_trans tr) noexcept { offset += tr.offset * scale; scale *= tr.scale; }
	void operator/=(_trans tr); // обратно сместить и промасштабировать
	bool operator!=(const _trans& b) const noexcept;

	_trans inverse() const noexcept;   // обратная трансформация
	_xy inverse(_xy b) const noexcept;
	_area inverse(const _area& b) const noexcept;

	void MasToch(_xy b, double m); // промасштабировать вокруг точки
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

uint test_line(_xy p1, _xy p2, _xy b);

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
