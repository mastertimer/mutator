#pragma once

#include <cmath>
#include <ostream>

#include "mbasic.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

using _coordinate  = double;
using _size        = double;

using _coordinatei = long long;
using _sizei       = long long;
using _number      = long long;

constexpr double de_i = 0.0001; // для больших чисел - не правильно!

struct _size2;
struct _area2;

struct _coordinate2i
{
	_coordinatei x, y;
};

struct _coordinate2
{
	_coordinate x, y;

	_coordinate2 operator-(_coordinate2 b) const noexcept { return { x - b.x, y - b.y }; }
	_coordinate2 operator*(double b)       const noexcept { return { x * b, y * b }; }

	void operator+=(_coordinate2 b)              noexcept { x += b.x; y += b.y; }
};

struct _number2
{
	_number x, y;

	operator _coordinate2() { return { x + 0.5, y + 0.5 }; }
};

struct _size2 // [0...x], [0...y]
{
	_size x, y;

	bool empty() const noexcept { return (x < 0) || (y < 0); }
	_coordinate2 center() const noexcept { return { x * 0.5, y * 0.5 }; }
};

struct _size2i // [0...x), [0...y)
{
	_sizei x, y;

	_size2i(_sizei x_, _sizei y_) : x(x_), y(y_) {}
	_size2i() = default;

	bool   empty()  const noexcept { return ((x <= 0) || (y <= 0)); }
	void   clear()        noexcept { x = y = 0; }
	_sizei square() const noexcept { return ((x <= 0) || (y <= 0)) ? 0 : (x * y); }
	_coordinate2 center() const noexcept { return { x * 0.5, y * 0.5 }; }
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// min > max --> область не задана
struct _areai // [...)
{
	_coordinatei min = 0;
	_coordinatei max = 0;

//	void set_from(_coordinatei b) noexcept { max = min = b; }       // наглядный пример,
//	void set_from(_number b)          noexcept { max = (min = b) + 1; } // в чем разница,
//	void set_from(_sizei b)             noexcept { min = 0; max = b; }    // хотя все 3 - long long!

	void operator&=(const _areai& b) noexcept { if (b.min > min) min = b.min; if (b.max < max) max = b.max; }

	_sizei size() const noexcept { return (min < max) ? (max - min) : 0; }
//	bool empty()  const noexcept { return (max <= min); }
};

struct _area2i
{
	_areai x, y; // [...) [...)

	_area2i() = default;
	_area2i(const _area2i&) = default;
	_area2i(_size2i b) : x{ 0, b.x }, y{ 0, b.y } {}
	_area2i(_areai x_, _areai y_) : x(x_), y(y_) {}

	void operator&=(const _area2i& b) noexcept { x &= b.x; y &= b.y; }

	_area2i operator&(const _area2i& b) const noexcept { _area2i c(*this); c &= b; return c; }

	bool empty() const noexcept { return (x.min >= x.max) || (y.min >= y.max); }
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct _area // [...]
{
	_coordinate min = 1;
	_coordinate max = 0;

	operator _areai() const noexcept
	{
		_areai res{ (_coordinatei)min, (_coordinatei)max };
		if (min < 0.0) if (min != res.min) res.min--;
		if ((max >= 0.0)||(max == res.max)) res.max++;
		return res;
	}

	void operator&=(const _area& b) noexcept { if (b.min > min) min = b.min; if (b.max < max) max = b.max; }

	_size length() { return (max > min) ? (max - min) : 0; }
};

struct _area2
{
	_area x, y; // [...] [...]

	_area2() = default;
	_area2(const _area2&) = default;
	_area2(_area x_, _area y_) : x(x_), y(y_) {}
	_area2(_size2i b) : x{ 0.0, b.x - de_i }, y{ 0.0, b.y - de_i } {}
	_area2(_size2 b) : x{ 0.0, b.x }, y{ 0.0, b.y } {}
	_area2(_coordinate2 b) : x{ b.x, b.x }, y{ b.y, b.y } {}

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
	_coordinate2 center() const noexcept { return { (x.max + x.min) * 0.5, (y.max + y.min) * 0.5 }; }

	bool test(_coordinate2 b); // принадлежит ли точка области
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct _transformation2
{
	double scale        = 1.0;
	_coordinate2 offset = { 0.0, 0.0 };

	_area2 operator()(const _area2& b) const noexcept; // применение трансформации
	void operator*=(_transformation2 tr) noexcept { offset += tr.offset * scale; scale *= tr.scale; }
	_transformation2 operator*(_transformation2 tr) const noexcept;  // сместить и промасштабировать

	_coordinate2 inverse(_coordinate2 b) const noexcept;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// одновременно работает с x и y
t_t struct vec2
{
	_t x, y;

	vec2(_t x, _t y) : x(x), y(y) {}
	vec2(_size2i size) : x((_t)size.x), y((_t)size.y) {}
	t_b vec2(vec2<_b> size) : x((_t)size.x), y((_t)size.y) {}
	vec2() = default;

//	TB operator vec2<_b>() const noexcept { return {_b(x), _b(y)}; };
	operator _coordinate2()  const noexcept { return { (double)x, (double)y }; };
	operator _coordinate2i() const noexcept { return { (int64)x, (int64)y }; };
	operator _size2i()       const noexcept { return { (int64)x, (int64)y }; };
	operator _number2()      const noexcept { return { (int64)x, (int64)y }; };

	[[nodiscard]] vec2 operator-() const noexcept;

	[[nodiscard]] vec2 operator+(vec2 const& b) const noexcept;
	[[nodiscard]] vec2 operator-(vec2 const& b) const noexcept;
	[[nodiscard]] vec2 operator*(vec2 const& b) const noexcept;
	[[nodiscard]] vec2 operator/(vec2 const& b) const noexcept;

	[[nodiscard]] vec2 operator+(_t b) const noexcept;
	[[nodiscard]] vec2 operator-(_t b) const noexcept;
	[[nodiscard]] vec2 operator*(_t b) const noexcept;
	[[nodiscard]] vec2 operator/(_t b) const noexcept;

	void operator+=(_t b) noexcept;
	void operator-=(_t b) noexcept;
	void operator*=(_t b) noexcept;
	void operator/=(_t b) noexcept;

	void operator+=(vec2 const& b) noexcept;
	void operator-=(vec2 const& b) noexcept;
	void operator*=(vec2 const& b) noexcept;
	void operator/=(vec2 const& b) noexcept;

	[[nodiscard]] _t scalar(vec2 b) const noexcept; // скалярное произведение

	[[nodiscard]] _t len() const noexcept;  // длина вектора
	[[nodiscard]] _t len2() const noexcept; // квадрат длины вектора

	static void sort(vec2& a, vec2& b);

	void bound_lt(_t x2, _t y2); // верхняя левая граница
	void bound_rd(_t x2, _t y2); // нижняя правая граница
	void bound_lt(vec2& b);
	void bound_rd(vec2& b);

	vec2 round();      // Округление
	vec2 drop_round(); // Отбрасывание дробной части
	vec2 round_min(); // Округление к меньшему (правильно работает с отрицательными числами)
	vec2 round_max(); // Округление к большему
	t_b vec2<_b> round_max_cast(); // Округление к большему с преобразованием

	vec2 with_x(_t v) { return {v, y}; }
	vec2 with_y(_t v) { return {x, v}; }
	vec2 with_x(vec2 b) { return {b.x, y}; }
	vec2 with_y(vec2 b) { return {x, b.y}; }
};

t_t bool operator<(vec2<_t> const& a, vec2<_t> const& b);
t_t bool operator<=(vec2<_t> const& a, vec2<_t> const& b);
t_t bool operator>(vec2<_t> const& a, vec2<_t> const& b);
t_t bool operator>=(vec2<_t> const& a, vec2<_t> const& b);

t_t [[nodiscard]] vec2<_t> operator+(_t a, vec2<_t> const& b);
t_t [[nodiscard]] vec2<_t> operator-(_t a, vec2<_t> const& b);
t_t [[nodiscard]] vec2<_t> operator*(_t a, vec2<_t> const& b);
t_t [[nodiscard]] vec2<_t> operator/(_t a, vec2<_t> const& b);

t_t bool operator==(vec2<_t> const& a, vec2<_t> const& b);
t_t bool operator!=(vec2<_t> const& a, vec2<_t> const& b);

template <class _t, class TC>
std::basic_ostream<TC>& operator<<(std::basic_ostream<TC>& o, vec2<_t> vec);

t_t using __xy = vec2<_t>;

typedef __xy<double> _xy;
typedef __xy<int>    _ixy;

using vec2i = vec2<int>;
using vec2f = vec2<double>;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template <typename _t>
struct _interval
{
	_t min, max;

	[[nodiscard]] _t  length() const noexcept;
	[[nodiscard]] _t  operator()(double k) const noexcept; // [0..1 -> min..max]
	[[nodiscard]] double get_k(_t a) const noexcept;       // [min..max -> 0..1]

	t_b _interval& operator=(const _interval<_b>& a) noexcept;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

enum class _tarea { normal = 0, empty };

t_t struct __area // базовая область
{
	_interval<_t> x, y;
	_tarea        type;

	__area(_tarea t = _tarea::empty) : x(), y(), type(t) {}
	__area(_t minx, _t maxx, _t miny, _t maxy);
	__area(_size2i s);
	__area(__xy<_t> b) : x{ b.x, b.x }, y{ b.y, b.y }, type{ _tarea::normal } {}
	__area(const __area& b) = default;
	__area(const _area2& b);

	bool empty() const noexcept { return type == _tarea::empty; }

	operator _area2() const noexcept;

	void operator=(_tarea t); // задать тип
	bool operator==(const __area& b) const noexcept;
	bool operator!=(const __area& b) const noexcept;
	bool operator<(const __area& b) const noexcept;  // полностью внутри?
	bool operator<=(const __area& b) const noexcept; // полностью внутри или совпадает?
	void operator=(__xy<_t> b) noexcept;             // проинициализировать точкой

	__area expansion(_t b) const noexcept; // расширенная область во все стороны

	void   operator+=(__xy<_t> b) noexcept; // расширить границы точкой
	__area operator+(__xy<_t> b) const noexcept;
	void   operator+=(const __area& b) noexcept; // расширить границы другой областью
	__area operator/(const __area& b) const noexcept; // получится область в относит. координатах от b
	void   operator&=(const __area& b) noexcept;      // присвоить общую область
	__area operator&(const __area& b) const noexcept; // общая область

	bool test(__xy<_t> b); // лежит ли точка внутри

	__xy<_t> center();       // центр области
	__xy<_t> top_left();     // верхний левый угол
	__xy<_t> top_right();    // верхний правый угол
	__xy<_t> bottom_left();  // нижний левый угол
	__xy<_t> bottom_right(); // нижний правый угол

	_t radius();     // радиус вписанной окружности
	_t min_length(); // минимальный размер
	_t max_length(); // минимальный размер

	__area move(const __xy<_t>& d) const noexcept;
};

typedef __area<double> _area_old;
typedef __area<int>    _areai_old;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct _trans
{
	double scale  = 1.0;
	vec2f  offset = {0.0, 0.0};

	_trans operator*(_trans tr);  // сместить и промасштабировать
	_trans operator/(_trans tr);  // обратно сместить и промасштабировать
	void   operator/=(_trans tr); // обратно сместить и промасштабировать
	void   operator*=(_trans tr); // сместить и промасштабировать
	bool   operator==(_trans& b);
	bool   operator!=(const _trans& b);

	void   MasToch(vec2f b, double m); // промасштабировать вокруг точки
	_trans inverse() const noexcept;   // обратная трансформация
	vec2f  inverse(vec2f b) const noexcept;
	_area_old  inverse(const _area_old& b) const noexcept;

	_area_old  operator()(const _area_old& b) const noexcept; // применение трансформации
	vec2f  operator()(const vec2f& b) const noexcept; // применение трансформации
	double operator()(double b) const noexcept;       // применение трансформации
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

t_t vec2<_t> vec2<_t>::operator-() const noexcept { return {-x, -y}; }

t_t vec2<_t> vec2<_t>::operator+(vec2 const& b) const noexcept { return {x + b.x, y + b.y}; }
t_t vec2<_t> vec2<_t>::operator-(vec2 const& b) const noexcept { return {x - b.x, y - b.y}; }
t_t vec2<_t> vec2<_t>::operator*(vec2 const& b) const noexcept { return {x * b.x, y * b.y}; }
t_t vec2<_t> vec2<_t>::operator/(vec2 const& b) const noexcept { return {x / b.x, y / b.y}; }

t_t vec2<_t> vec2<_t>::operator+(_t b) const noexcept { return {x + b, y + b}; }
t_t vec2<_t> vec2<_t>::operator-(_t b) const noexcept { return {x - b, y - b}; }
t_t vec2<_t> vec2<_t>::operator*(_t b) const noexcept { return {x * b, y * b}; }
t_t vec2<_t> vec2<_t>::operator/(_t b) const noexcept { return {x / b, y / b}; }

t_t void vec2<_t>::sort(vec2<_t>& a, vec2<_t>& b)
{
	if (a.x > b.x) std::swap(a.x, b.x);
	if (a.y > b.y) std::swap(a.y, b.y);
}

t_t void vec2<_t>::bound_lt(_t x2, _t y2)
{
	if (x < x2) x = x2;
	if (y < y2) y = y2;
}
t_t void vec2<_t>::bound_rd(_t x2, _t y2)
{
	if (x > x2) x = x2;
	if (y > y2) y = y2;
}
t_t void vec2<_t>::bound_lt(vec2<_t>& b) { bound_lt(b.x, b.y); }
t_t void vec2<_t>::bound_rd(vec2<_t>& b) { bound_rd(b.x, b.y); }

t_t vec2<_t> vec2<_t>::round()
{
	static_assert(std::is_same_v<_t, float> || std::is_same_v<_t, double>);

	if constexpr (std::is_same_v<_t, float>)
		return {::roundf(x), ::roundf(y)};
	else
		return {::round(x), ::round(y)};
}

t_t vec2<_t> vec2<_t>::drop_round() { return vec2<int>(*this); }
t_t vec2<_t> vec2<_t>::round_min()
{
	static_assert(std::is_same_v<_t, float> || std::is_same_v<_t, double>);

	if constexpr (std::is_same_v<_t, float>)
		return {::floorf(x), ::floorf(y)};
	else
		return {::floor(x), ::floor(y)};
}
t_t vec2<_t> vec2<_t>::round_max()
{
	static_assert(std::is_same_v<_t, float> || std::is_same_v<_t, double>);

	if constexpr (std::is_same_v<_t, float>)
		return {::ceilf(x), ::ceilf(y)};
	else
		return {::ceil(x), ::ceil(y)};
}
t_t t_b vec2<_b> vec2<_t>::round_max_cast() { return {x == _b(x) ? _b(x) : _b(x) + 1, y == _b(y) ? _b(y) : _b(y) + 1}; }

t_t void vec2<_t>::operator+=(const _t b) noexcept
{
	x += b;
	y += b;
}
t_t void vec2<_t>::operator-=(const _t b) noexcept
{
	x -= b;
	y -= b;
}
t_t void vec2<_t>::operator*=(const _t b) noexcept
{
	x *= b;
	y *= b;
}
t_t void vec2<_t>::operator/=(const _t b) noexcept
{
	x /= b;
	y /= b;
}

t_t void vec2<_t>::operator+=(vec2 const& b) noexcept
{
	x += b.x;
	y += b.y;
}
t_t void vec2<_t>::operator-=(vec2 const& b) noexcept
{
	x -= b.x;
	y -= b.y;
}
t_t void vec2<_t>::operator*=(vec2 const& b) noexcept
{
	x *= b.x;
	y *= b.y;
}
t_t void vec2<_t>::operator/=(vec2 const& b) noexcept
{
	x /= b.x;
	y /= b.y;
}

t_t [[nodiscard]] vec2<_t> operator+(_t a, vec2<_t> const& b) { return {a + b.x, a + b.y}; }
t_t [[nodiscard]] vec2<_t> operator-(_t a, vec2<_t> const& b) { return {a - b.x, a - b.y}; }
t_t [[nodiscard]] vec2<_t> operator*(_t a, vec2<_t> const& b) { return {a * b.x, a * b.y}; }
t_t [[nodiscard]] vec2<_t> operator/(_t a, vec2<_t> const& b) { return {a / b.x, a / b.y}; }

t_t _t vec2<_t>::scalar(const vec2<_t> b) const noexcept { return x * b.x + y * b.y; }

t_t _t vec2<_t>::len() const noexcept { return sqrt(x * x + y * y); }
t_t _t vec2<_t>::len2() const noexcept { return x * x + y * y; }

t_t bool operator<(vec2<_t> const& a, vec2<_t> const& b) { return a.x < b.x && a.y < b.y; }
t_t bool operator<=(vec2<_t> const& a, vec2<_t> const& b) { return a.x <= b.x && a.y <= b.y; }
t_t bool operator>(vec2<_t> const& a, vec2<_t> const& b) { return a.x > b.x && a.y > b.y; }
t_t bool operator>=(vec2<_t> const& a, vec2<_t> const& b) { return a.x > b.x && a.y > b.y; }

t_t bool operator==(vec2<_t> const& a, vec2<_t> const& b) { return a.x == b.x && a.y == b.y; }
t_t bool operator!=(vec2<_t> const& a, vec2<_t> const& b) { return a.x != b.x || a.y != b.y; }

template <class _t, class TC>
std::basic_ostream<TC>& operator<<(std::basic_ostream<TC>& o, vec2<_t> vec)
{
	return o << vec.x << ", " << vec.y;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////

t_t _t _interval<_t>::length() const noexcept { return max - min; }
t_t _t _interval<_t>::operator()(double k) const noexcept { return _t(min + ((double)max - min) * k); }

t_t double _interval<_t>::get_k(_t a) const noexcept { return (a - min) / (max - min); }

t_t t_b _interval<_t>& _interval<_t>::operator=(const _interval<_b>& a) noexcept
{
	min = _t(a.min);
	max = _t(a.max);
	return *this;
}

uint test_line(_xy p1, _xy p2, _xy b);

/////////////////////////////////////////////////////////////////////////////////////////////////////

t_t __area<_t>::__area(const _area2& b) : x{ b.x.min, b.x.max }, y{ b.y.min, b.y.max }
{
	type = (b.empty()) ? _tarea::empty : _tarea::normal;
}

t_t __area<_t>::__area(_size2i s) : x{ 0, s.x }, y{ 0, s.y }
{
	type = (s.empty()) ? _tarea::empty : _tarea::normal;
}

t_t __area<_t>::__area(_t minx, _t maxx, _t miny, _t maxy) : x{minx, maxx}, y{miny, maxy}, type{_tarea::normal}
{
	if ((this->x.min > this->x.max) || (this->y.min > this->y.max)) type = _tarea::empty;
}

t_t bool __area<_t>::operator<=(const __area& b) const noexcept
{
	if ((type == _tarea::normal) || (b.type == _tarea::normal))
		return ((this->x.min >= b.x.min) && (this->x.max <= b.x.max) && (this->y.min >= b.y.min) &&
		        (this->y.max <= b.y.max));
	return (type == _tarea::empty);
}

t_t bool __area<_t>::operator<(const __area& b) const noexcept
{
	if ((type == _tarea::normal) || (b.type == _tarea::normal))
		return ((this->x.min > b.x.min) && (this->x.max < b.x.max) && (this->y.min > b.y.min) &&
		        (this->y.max < b.y.max));
	return ((type == _tarea::empty) && (type != b.type));
}

t_t void __area<_t>::operator=(__xy<_t> b) noexcept
{
	this->x.min = this->x.max = b.x;
	this->y.min = this->y.max = b.y;

	type = _tarea::normal;
}

t_t __area<_t> __area<_t>::expansion(_t b) const noexcept
{
	if (type != _tarea::normal)
	{
		if ((type == _tarea::empty) && (b > 0)) return _tarea::empty;
		return type;
	}
	return __area(this->x.min - b, this->x.max + b, this->y.min - b, this->y.max + b);
}

t_t void __area<_t>::operator+=(__xy<_t> b) noexcept
{
	if (type == _tarea::empty)
	{
		*this = b;
		return;
	}
	if (type != _tarea::normal) return;
	if (b.x < this->x.min) this->x.min = b.x;
	if (b.x > this->x.max) this->x.max = b.x;
	if (b.y < this->y.min) this->y.min = b.y;
	if (b.y > this->y.max) this->y.max = b.y;
}

t_t __area<_t> __area<_t>::operator+(__xy<_t> b) const noexcept
{
	__area res(*this);
	res += b;
	return res;
}

t_t void __area<_t>::operator+=(const __area& b) noexcept
{
	if ((type != _tarea::normal) | (b.type != _tarea::normal))
	{
		if (b.type == _tarea::empty) return;
		if (type == _tarea::empty)
		{
			*this = b;
			return;
		}
	}
	if (b.x.min < this->x.min) this->x.min = b.x.min;
	if (b.x.max > this->x.max) this->x.max = b.x.max;
	if (b.y.min < this->y.min) this->y.min = b.y.min;
	if (b.y.max > this->y.max) this->y.max = b.y.max;
}

t_t __area<_t> __area<_t>::operator/(const __area& b) const noexcept
{
	if ((type != _tarea::normal) | (b.type != _tarea::normal))
	{
		if (type == _tarea::empty)
		{
			if (type == b.type) return _tarea::empty;
			return type;
		}
		if (b.type == _tarea::empty) return _tarea::empty;
	}
	_t dx = b.x.max - b.x.min;
	_t dy = b.y.max - b.y.min;
	return __area((this->x.min - b.x.min) / dx, (this->x.max - b.x.min) / dx, (this->y.min - b.y.min) / dy,
	              (this->y.max - b.y.min) / dy);
}

t_t void __area<_t>::operator&=(const __area& b) noexcept
{
	if ((type != _tarea::normal) | (b.type != _tarea::normal))
	{
		if ((type == _tarea::empty) || (b.type == _tarea::empty))
		{
			type = _tarea::empty;
			return;
		}
	}
	if (b.x.min > this->x.min) this->x.min = b.x.min;
	if (b.x.max < this->x.max) this->x.max = b.x.max;
	if (b.y.min > this->y.min) this->y.min = b.y.min;
	if (b.y.max < this->y.max) this->y.max = b.y.max;
	if ((this->x.max < this->x.min) || (this->y.max < this->y.min)) type = _tarea::empty;
}

t_t __area<_t> __area<_t>::operator&(const __area& b) const noexcept
{
	__area c = *this;
	c &= b;
	return c;
}

t_t bool __area<_t>::operator!=(const __area& b) const noexcept
{
	if ((type == _tarea::empty) || (b.type == _tarea::empty)) return (type != b.type);
	return ((this->x.min != b.x.min) || (this->y.min != b.y.min) || (this->x.max != b.x.max) ||
	        (this->y.max != b.y.max));
}

t_t void __area<_t>::operator=(_tarea t) { type = t; }

t_t bool __area<_t>::operator==(const __area& b) const noexcept
{
	if ((type == _tarea::empty) || (b.type == _tarea::empty)) return (type == b.type);
	return ((this->x.min == b.x.min) && (this->y.min == b.y.min) && (this->x.max == b.x.max) &&
	        (this->y.max == b.y.max));
}

t_t bool __area<_t>::test(__xy<_t> b)
{
	if (type != _tarea::normal) return false;
	return ((b.x >= this->x.min) && (b.x <= this->x.max) && (b.y >= this->y.min) && (b.y <= this->y.max));
}

t_t __xy<_t> __area<_t>::center()
{
	if (type != _tarea::normal) return {0, 0};
	return {(this->x.min + this->x.max) / 2, (this->y.min + this->y.max) / 2};
}

t_t __xy<_t> __area<_t>::top_left()
{
	if (type != _tarea::normal) return {0, 0};
	return {this->x.min, this->y.min};
}
t_t __xy<_t> __area<_t>::top_right()
{
	if (type != _tarea::normal) return {0, 0};
	return {this->x.max, this->y.min};
}
t_t __xy<_t> __area<_t>::bottom_left()
{
	if (type != _tarea::normal) return {0, 0};
	return {this->x.min, this->y.max};
}
t_t __xy<_t> __area<_t>::bottom_right()
{
	if (type != _tarea::normal) return {0, 0};
	return {this->x.max, this->y.max};
}

t_t _t __area<_t>::radius()
{
	if (type != _tarea::normal) return 0.0;
	_t dx = this->x.max - this->x.min;
	_t dy = this->y.max - this->y.min;
	return ((dx < dy) ? dx : dy) / 2;
}

t_t _t __area<_t>::min_length()
{
	if (type != _tarea::normal) return 0.0;
	_t dx = this->x.max - this->x.min;
	_t dy = this->y.max - this->y.min;
	return ((dx < dy) ? dx : dy);
}
t_t _t __area<_t>::max_length()
{
	if (type != _tarea::normal) return 0.0;
	_t dx = this->x.max - this->x.min;
	_t dy = this->y.max - this->y.min;
	return ((dx > dy) ? dx : dy);
}

t_t __area<_t> __area<_t>::move(const __xy<_t>& d) const noexcept
{
	__area<_t> rez;
	rez.type  = type;
	rez.x.min = this->x.min + d.x;
	rez.x.max = this->x.max + d.x;
	rez.y.min = this->y.min + d.y;
	rez.y.max = this->y.max + d.y;
	return rez;
}

t_t __area<_t>::operator _area2() const noexcept
{
	if (type != _tarea::normal) return _area2();
	return { {x.min, x.max}, {y.min, y.max} };
}
