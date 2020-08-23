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

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

_trans _trans::operator*(_trans tr)
{
	tr.offset.x = offset.x + tr.offset.x * scale;
	tr.offset.y = offset.y + tr.offset.y * scale;
	tr.scale *= scale;
	return tr;
}

_trans _trans::operator/(_trans tr)
{
	tr.scale    = scale / tr.scale;
	tr.offset.x = offset.x - tr.offset.x * tr.scale;
	tr.offset.y = offset.y - tr.offset.y * tr.scale;
	return tr;
}

void _trans::operator/=(_trans tr)
{
	scale /= tr.scale;
	offset.x -= tr.offset.x * scale;
	offset.y -= tr.offset.y * scale;
}

void _trans::operator*=(_trans tr)
{
	offset.x += tr.offset.x * scale;
	offset.y += tr.offset.y * scale;
	scale *= tr.scale;
}

bool _trans::operator==(_trans& b) { return ((scale == b.scale) && (offset.x == b.offset.x) && (offset.y == b.offset.y)); }

bool _trans::operator!=(const _trans& b) { return ((scale != b.scale) || (offset.x != b.offset.x) || (offset.y != b.offset.y)); }

void _trans::MasToch(_xy b, double m)
{
	offset.x = b.x + m * (offset.x - b.x);
	offset.y = b.y + m * (offset.y - b.y);
	scale *= m;
}

_trans _trans::inverse() const noexcept
{
	double mm = 1.0 / scale;
	return {mm, {-offset.x * mm, -offset.y * mm}};
}

_xy _trans::inverse(const _xy b) const noexcept { return {(b.x - offset.x) / scale, (b.y - offset.y) / scale}; }

_area_old _trans::inverse(const _area_old& b) const noexcept
{
	if (b.type != _tarea::normal) return b;
	_area_old c(_tarea::normal);
	double mm = 1.0 / scale;
	c.x.min   = (b.x.min - offset.x) * mm;
	c.x.max   = (b.x.max - offset.x) * mm;
	c.y.min   = (b.y.min - offset.y) * mm;
	c.y.max   = (b.y.max - offset.y) * mm;
	return c;
}

_xy _trans::operator()(const _xy& b) const noexcept { return _xy{b.x * scale + offset.x, b.y * scale + offset.y}; }

double _trans::operator()(const double b) const noexcept { return scale * b; }

_area_old _trans::operator()(const _area_old& b) const noexcept
{
	if (b.type != _tarea::normal) return b;
	_area_old c(_tarea::normal);
	c.x.min = b.x.min * scale + offset.x;
	c.x.max = b.x.max * scale + offset.x;
	c.y.min = b.y.min * scale + offset.y;
	c.y.max = b.y.max * scale + offset.y;
	return c;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

_area2 _trans2::operator()(const _area2& b) const noexcept
{
//	if (b.empty()) return b;
	return { {b.x.min * scale + offset.x, b.x.max * scale + offset.x},
		     {b.y.min * scale + offset.y, b.y.max * scale + offset.y} };
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

uint test_line(_xy p1, _xy p2, _xy b)
{
	if (p2.x < p1.x) std::swap(p1, p2);
	if ((p1.x > b.x) || (p2.x <= b.x)) return 0;
	return ((b.x - p1.x) * (p2.y - p1.y) - (b.y - p1.y) * (p2.x - p1.x) > 0);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
