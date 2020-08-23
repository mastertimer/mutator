#include "mgeometry.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void _area2::operator+=(const _area2& b) noexcept
{
	if (b.empty()) return;
	if (empty()) { *this = b; return; }
	if (b.x.min < this->x.min) this->x.min = b.x.min;
	if (b.x.max > this->x.max) this->x.max = b.x.max;
	if (b.y.min < this->y.min) this->y.min = b.y.min;
	if (b.y.max > this->y.max) this->y.max = b.y.max;
}

bool _area2::operator<=(const _area2& b) const noexcept
{
	if (empty()) return true;
	if (b.empty()) return false;
	return ((x.min >= b.x.min) && (x.max <= b.x.max) && (y.min >= b.y.min) && (y.max <= b.y.max));
}

bool _area2::inside(const _area2& b) const noexcept
{
	if (b.empty()) return false;
	if (empty()) return true;
	return ((x.min > b.x.min) && (x.max < b.x.max) && (y.min > b.y.min) && (y.max < b.y.max));
}

bool _area2::operator<(const _area2& b) const noexcept
{
	if (b.empty()) return false;
	if (empty()) return true;
	if ((x.min == b.x.min) && (x.max == b.x.max) && (y.min == b.y.min) && (y.max == b.y.max)) return false;
	return ((x.min >= b.x.min) && (x.max <= b.x.max) && (y.min >= b.y.min) && (y.max <= b.y.max));
}

bool _area2::test(_coo2 b)
{
	if (empty()) return false;
	return ((b.x >= this->x.min) && (b.x <= this->x.max) && (b.y >= this->y.min) && (b.y <= this->y.max));
}

_size _area2::radius()
{
	if (empty()) return 0.0;
	double dx = x.max - x.min;
	double dy = y.max - y.min;
	return ((dx < dy) ? dx : dy) / 2;
}

_area2 _area2::expansion(double b) const noexcept
{
	if (empty()) return *this;
	return { {x.min - b, x.max + b}, {y.min - b, y.max + b} };
}

_area2 _area2::move(_coo2 d) const noexcept
{
	return { {x.min + d.x, x.max + d.x}, {y.min + d.y, y.max + d.y} };
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

_trans2 _trans2::operator*(_trans2 tr) const noexcept
{
	tr.offset.x = offset.x + tr.offset.x * scale;
	tr.offset.y = offset.y + tr.offset.y * scale;
	tr.scale *= scale;
	return tr;
}

_coo2 _trans2::inverse(_coo2 b) const noexcept
{
	return { (b.x - offset.x) / scale, (b.y - offset.y) / scale };
}

_trans2 _trans2::inverse() const noexcept
{
	double mm = 1.0 / scale;
	return { mm, {-offset.x * mm, -offset.y * mm} };
}

_trans2 _trans2::operator/(_trans2 tr) const noexcept
{
	tr.scale = scale / tr.scale;
	tr.offset.x = offset.x - tr.offset.x * tr.scale;
	tr.offset.y = offset.y - tr.offset.y * tr.scale;
	return tr;
}

bool _trans2::operator!=(const _trans2& b) const noexcept
{
	return ((scale != b.scale) || (offset.x != b.offset.x) || (offset.y != b.offset.y));
}

_coo2 _trans2::operator()(const _coo2& b) const noexcept
{
	return _coo2{ b.x * scale + offset.x, b.y * scale + offset.y };
}

_area2 _trans2::inverse(const _area2& b) const noexcept
{
	if (b.empty()) return b;
	_area2 c;
	double mm = 1.0 / scale;
	c.x.min = (b.x.min - offset.x) * mm;
	c.x.max = (b.x.max - offset.x) * mm;
	c.y.min = (b.y.min - offset.y) * mm;
	c.y.max = (b.y.max - offset.y) * mm;
	return c;
}

void _trans2::MasToch(_coo2 b, double m)
{
	offset.x = b.x + m * (offset.x - b.x);
	offset.y = b.y + m * (offset.y - b.y);
	scale *= m;
}

void _trans2::operator/=(_trans2 tr)
{
	scale /= tr.scale;
	offset.x -= tr.offset.x * scale;
	offset.y -= tr.offset.y * scale;
}

_area2 _trans2::operator()(const _area2& b) const noexcept
{
//	if (b.empty()) return b;
	return { {b.x.min * scale + offset.x, b.x.max * scale + offset.x},
		     {b.y.min * scale + offset.y, b.y.max * scale + offset.y} };
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

uint test_line(_coo2 p1, _coo2 p2, _coo2 b)
{
	if (p2.x < p1.x) std::swap(p1, p2);
	if ((p1.x > b.x) || (p2.x <= b.x)) return 0;
	return ((b.x - p1.x) * (p2.y - p1.y) - (b.y - p1.y) * (p2.x - p1.x) > 0);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
