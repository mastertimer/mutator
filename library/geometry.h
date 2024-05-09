#pragma once

#include "basic.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct _ixy // индекс, номер
{
	i64 x;
	i64 y;

	_ixy(int x_,    int y_)    : x(x_), y(y_) {}
	_ixy(i64 x_,    i64 y_)    : x(x_), y(y_) {}
	_ixy(i64 x_,    double y_) : x(x_), y((i64)y_) { if ((y_ < 0) && (y != y_)) y--; }
	_ixy(double x_, i64 y_)    : x((i64)x_), y(y_) { if ((x_ < 0) && (x != x_)) x--; }
	_ixy(double x_, double y_) : x((i64)x_), y((i64)y_)
	{ 
		if ((x_ < 0) && (x != x_)) x--;
		if ((y_ < 0) && (y != y_)) y--;
	}
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct _xy
{
	double x;
	double y;

	_xy(double x_, double y_) : x(x_), y(y_) {}
	_xy(const _ixy b) : x(b.x + 0.5), y(b.y + 0.5) {}

	operator _ixy()               const { return { x, y }; }

	_xy operator-()               const { return { -x,  -y }; }

	_xy operator+(const _xy b)    const { return { x + b.x, y + b.y }; }
	_xy operator-(const _xy b)    const { return { x - b.x, y - b.y }; }
	_xy operator*(const double b) const { return { x * b, y * b }; }
	_xy operator/(const double b) const { double k = 1.0 / b; return { x * k, y * k }; }

	void operator+=(const _xy b)        { x += b.x; y += b.y; }
	void operator-=(const _xy b)        { x -= b.x; y -= b.y; }
	void operator*=(const double b)     { x *= b; y *= b; }
	void operator/=(const double b)     { double k = 1.0 / b; x *= k; y *= k; }

	bool operator!=(const _xy b)  const { return (x != b.x) || (y != b.y); }

	double len()                  const { return sqrt(x * x + y * y); }
	double len2()                 const { return x * x + y * y; }

	double scalar(_xy b)          const { return x * b.x + y * b.y; }

	_xy rotation(double b)        const;
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
	[[nodiscard]] _isize extended(_ixy delta) const;

	bool operator==(const _isize s) const { return ((x == s.x) && (y == s.y)) || (empty() && s.empty()); }
	bool operator!=(const _isize s) const { return !(*this == s); }

	void operator|=(const _isize& b);
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct _size // [0...x]), [0...y])
{
	double x = 0;
	double y = 0;
	bool   empty = true;
	bool   x_closed = true;
	bool   y_closed = true;

};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct _interval;

struct _iinterval // [...)
{
	i64 min = 0;
	i64 max = 0;

	_iinterval() = default;
	_iinterval(i64 x) : min(x), max(x + 1) {}
	_iinterval(int min_, int max_) : min(min_), max(max_) {}
	_iinterval(i64 min_, i64 max_) : min(min_), max(max_) {}
	_iinterval(i64 min_, double max_) : min(min_), max((i64)max_) { if ((max_ > 0) || (max == max_)) max++; }
	_iinterval(double min_, i64 max_) : min((i64)min_), max(max_) { if ((min_ < 0) && (min != min_)) min--; }
	_iinterval(double min_, double max_);

	void operator=(i64 b) { min = b; max = b + 1; }

	bool operator==(const _iinterval b) const;
	bool operator!=(const _iinterval b) const { return !(*this == b); }

	void operator|=(const _iinterval b);
	void operator&=(const _iinterval b) { if (b.min > min) min = b.min; if (b.max < max) max = b.max; }
	_iinterval operator&(const _iinterval b) const { return { std::max(min, b.min), std::min(max, b.max) }; }

	_interval operator/(const _iinterval b);

	bool empty()  const { return (max <= min); }
	i64  center() const { i64 s = min + max; if (s < 0) s--; return s >> 1; }
	i64  length() const { return (min < max) ? (max - min) : 0; }
	_iinterval& operator << (i64 x);
	bool test(i64 x) const { return (x >= min) && (x < max); }
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct _iarea
{
	_iinterval x, y; // [...) [...)

	_iarea() = default;
	_iarea(_isize b) : x{ 0LL, b.x }, y{ 0LL, b.y } {}
	_iarea(_iinterval x_, _iinterval y_) : x(x_), y(y_) {}
	_iarea(_ixy b) : x(b.x), y(b.y) {}

	void operator|=(const _iarea& b);
	void operator&=(const _iarea& b);

	bool operator!=(_isize b) const;

	_iarea operator&( _iarea b) const { b &= *this; return b; }

	_ixy top_left()     const { return { x.min, y.min }; } // верхний левый угол
	_ixy top_right()    const { return { x.max, y.min }; } // верхний правый угол
	_ixy bottom_left()  const { return { x.min, y.max }; } // нижний левый угол
	_ixy bottom_right() const { return { x.max, y.max }; } // нижний правый угол
	bool empty() const;
	_isize size() const { if (empty()) return { 0,0 }; return { x.max - x.min, y.max - y.min }; }
	bool test(_ixy b) const;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct _interval // [...])
{
	double min          = 1.0;
	double max          = 0.0;
	bool   empty        = true;
	bool   right_closed = true;

	_interval() = default;
	_interval(double x) : min(x), max(x), empty(false) {}
	_interval(double min_, double max_) : min(min_), max(max_), empty(max < min) {}
	_interval(_iinterval b) : min((double)b.min), max((double)b.max), empty(max <= min), right_closed(false) {}

	operator _iinterval() const;

	bool operator==(const _interval& b) const;
	bool operator<=(const _interval& b) const;

	void operator|=(const _interval& b);
	void operator&=(const _interval& b);
	_interval operator&(const _interval& b) const { _interval res(b); res &= *this; return res; }

	_interval operator/(const _interval& b) const;

	double length() const { return empty ? 0 : (max - min); }
	_interval& operator << (double x);
	bool test(double b) const; // принадлежит ли точка области
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
	_interval x;
	_interval y;

	_area() = default;
	_area(_interval x_, _interval y_);
	_area(_isize b);
	_area(_iarea b);
	_area(_xy b);

	operator _iarea() const;

	bool operator==(const _area& b) const;
	bool operator<=(const _area& b) const;
	bool operator< (const _area& b) const;

	void operator&=(const _area& b);
	void operator|=(const _area& b);

	_area operator&(      _area  b) const { b &= *this; return b; }
	_area operator|(      _area  b) const { b |= *this; return b; }

	bool empty() const { return x.empty || y.empty; }

	_area expansion(double b) const; // расширенная область во все стороны на b

	_xy center()       const { return { (x.max + x.min) * 0.5, (y.max + y.min) * 0.5 }; }
	_xy top_left()     const { return { x.min, y.min }; } // верхний левый угол
	_xy top_right()    const { return { x.max, y.min }; } // верхний правый угол
	_xy bottom_left()  const { return { x.min, y.max }; } // нижний левый угол
	_xy bottom_right() const { return { x.max, y.max }; } // нижний правый угол

	double min_length() const;

	bool test(_xy b) const;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct _trans
{
	double scale = 1.0;
	_xy offset = { 0.0, 0.0 };

	_area  operator()(const _area& b)  const;
	_xy    operator()(const _xy    b)  const;
	double operator()(const double b)  const;

	_trans operator* (const _trans& b) const;
	_trans operator/ (const _trans& b) const;

	void   operator*=(const _trans& b);
	void   operator/=(const _trans& b);

	bool   operator!=(const _trans& b) const;

	_trans inverse()                   const;
	_xy    inverse(_xy b)              const;
	_area  inverse(const _area& b)     const;

	void scale_up(_xy b, double m); // промасштабировать вокруг точки
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

uint test_line(_xy p1, _xy p2, _xy b);

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

_iarea _isize::move(_ixy d) const
{
	return { {d.x, d.x + x}, {d.y, d.y + y} };
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
