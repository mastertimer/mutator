#include "geometry.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

_xy _xy::rotation(double b) const
{
	double c = cos(b);
	double s = sin(b);
	return { c * x - s * y, s * x + c * y };
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void _interval::operator&=(const _interval& b)
{ 
	if (empty) return;
	if (b.empty)
	{
		empty = true;
		return;
	}
	if (b.min > min) min = b.min; 
	if (b.max < max)
	{
		max = b.max;
		right_closed = b.right_closed;
	}
	else
		if (b.max == max) right_closed &= b.right_closed;
	empty = !((max > min) || ((max == min) && right_closed));
}

_interval& _interval::operator << (double x)
{
	if (empty)
	{
		min = max = x;
		empty = false;
		right_closed = true;
		return *this;
	}
	if (x < min) min = x;
	if (x >= max)
	{
		max = x;
		right_closed = true;
	}
	return *this;
}

bool _interval::operator==(const _interval& b) const
{
	if (empty && b.empty) return true;
	return (min == b.min) && (max == b.max) && (right_closed == b.right_closed);
}

bool _interval::operator<=(const _interval& b) const
{
	if (empty) return true;
	if (b.empty) return false;
	if (min < b.min) return false;
	if (max == b.max) return (!right_closed) || b.right_closed;
	return (max <= b.max);
}

void _interval::operator|=(const _interval& b)
{
	if (b.empty) return;
	if (empty)
	{ 
		*this = b;
		return;
	}
	if (b.min < min) min = b.min;
	if (b.max > max)
	{
		max = b.max;
		right_closed = b.right_closed;
		return;
	}
	if (b.max == max) right_closed |= b.right_closed;
}

bool _interval::test(double b)
{
	if (empty) return false;
	if (b == max) return right_closed;
	return (b >= min) && (b <= max);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

_area::_area(_interval x_, _interval y_) : x(x_), y(y_)
{
}

_area::_area(_isize b) : x{ 0.0, b.x - de_i }, y{ 0.0, b.y - de_i }
{
}

_area::_area(_iarea b) : x(b.x), y(b.y)
{
}

_area::_area(_xy b) : x(b.x), y(b.y)
{
}

_area::operator _iarea() const
{ 
	return { x, y };
}

bool _area::operator==(const _area& b) const
{
	if (empty() && b.empty()) return true;
	return (x == b.x) && (y == b.y);
}

bool _area::operator<=(const _area& b) const
{
	if (empty()) return true;
	if (b.empty()) return false;
	return (x <= b.x) && (y <= b.y);
}

bool _area::operator<(const _area& b) const
{
	if (b.empty()) return false;
	if (empty()) return true;
	if (*this == b) return false;
	return *this <= b;
}

void _area::operator&=(const _area& b)
{
	x &= b.x;
	y &= b.y;
}

void _area::operator|=(const _area& b)
{
	if (b.empty()) return;
	if (empty())
	{
		*this = b;
		return;
	}
	x |= b.x;
	y |= b.y;
}

bool _area::test(_xy b)
{
	return x.test(b.x) && y.test(b.y);
}

_area _area::expansion(double b) const
{
	if (empty()) return *this;
	return { {x.min - b, x.max + b}, {y.min - b, y.max + b} };
}

double _area::min_length()
{ 
	return std::min(x.length(), y.length());
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

_area _trans::operator()(const _area& b) const
{
	if (b.empty()) return b;
	return { {b.x.min * scale + offset.x, b.x.max * scale + offset.x},
			 {b.y.min * scale + offset.y, b.y.max * scale + offset.y} };
}

_xy _trans::operator()(const _xy  b) const
{
	return b * scale + offset;
}

double _trans::operator()(const double b) const
{
	return scale * b;
}

_trans _trans::operator*(const _trans& b) const
{
	return { scale * b.scale, offset + b.offset * scale };
}

_trans _trans::operator/(const _trans& b) const
{
	double new_scale = scale / b.scale;
	return { new_scale, offset - b.offset * new_scale };
}

void _trans::operator*=(const _trans& b)
{
	offset += b.offset * scale;
	scale *= b.scale;
}

void _trans::operator/=(const _trans& b)
{
	scale /= b.scale;
	offset -= b.offset * scale;
}

bool _trans::operator!=(const _trans& b) const
{
	return ((scale != b.scale) || (offset != b.offset));
}

_trans _trans::inverse() const
{
	double mm = 1.0 / scale;
	return { mm, -offset * mm };
}

_xy _trans::inverse(_xy b) const
{
	return (b - offset) / scale;
}

_area _trans::inverse(const _area& b) const
{
	if (b.empty()) return b;
	_area c;
	double mm = 1.0 / scale;
	c.x.min = (b.x.min - offset.x) * mm;
	c.x.max = (b.x.max - offset.x) * mm;
	c.y.min = (b.y.min - offset.y) * mm;
	c.y.max = (b.y.max - offset.y) * mm;
	return c;
}

void _trans::scale_up(_xy b, double m)
{
	scale *= m;
	offset = b + (offset - b) * m;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

_iinterval::_iinterval(double min_, double max_) : min(min_), max(max_)
{
	if ((min_ < 0) && (min != min_)) min--;
	if ((max_ > 0) || (max == max_)) max++;
}

bool _iinterval::operator==(_iinterval b) const
{
	if (empty() && b.empty()) return true;
	return ((min == b.min) && (max == b.max));
}

_iinterval& _iinterval::operator << (i64 x)
{
	if (empty())
	{
		min = x;
		max = x + 1;
		return *this;
	}
	if (x < min) min = x;
	if (x + 1 > max) max = x + 1;
	return *this;
}

_interval _iinterval::operator/(const _iinterval b)
{
	auto len = b.length();
	if (len == 0)
	{
		if (empty()) return { 0, 1.0 };
		return { -1e300, 1e300 };
	}
	return { (double(min - b.min)) / len, (double(max - b.min)) / len };
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool _iarea::operator!=(_isize b) const
{
	if (b.empty() && empty()) return false;
	return (x.min != 0) || (y.min != 0) || (x.max != b.x) || (y.max != b.y);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

std::optional<_segment> _segment::operator&(const _area& b)
{
	if (b.empty()) return {};
	return {};
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

uint test_line(_xy p1, _xy p2, _xy b)
{
	if (p2.x < p1.x) std::swap(p1, p2);
	if ((p1.x > b.x) || (p2.x <= b.x)) return 0;
	return ((b.x - p1.x) * (p2.y - p1.y) - (b.y - p1.y) * (p2.x - p1.x) > 0);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
