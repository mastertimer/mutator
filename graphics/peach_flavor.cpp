#include "peach_flavor.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

_canvas::_canvas(_size2i size) : bitmap((int)size.x, (int)size.y), changed_area(size)
{
	
}

void _canvas::resize(_size2i size)
{
	if (bitmap.resize((int)size.x, (int)size.y)) changed_area = size;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void _peach::delete_me()
{
	del_area();
	for (auto i = parent->flavor.begin(); i != parent->flavor.end(); ++i)
		if (i->second.get() == this)
		{
			parent->flavor.erase(i);
			return;
		}
}

bool _peach::mouse_button_left(_number2 xy, bool pressed, _transformation2 tr)
{
	tr *= trans;
	for (auto i = flavor.rbegin(); i != flavor.rend(); ++i)
	{
		if (!i->second->area.test((tr * i->second->trans).inverse(xy))) continue;
		if (i->second->mouse_button_left(xy, pressed, tr)) return true;
	}
	_coordinate2 r = tr.inverse(xy);
	if (local_area.test(r))
		if (ia)
		{
			if (pressed) if (ia->mouse_down_left(this))	return true;
		}
	return false;
}

void _peach::set_mouse_down_left(std::function<bool(_peach* a)> b)
{
	if (!ia) ia = std::unique_ptr<_interactivity>(new _interactivity);
	ia->mouse_down_left = b;
}

void _peach::set_picture(_size2i size)
{
	if (!picture)
		picture = std::unique_ptr<_canvas>(new _canvas(size));
	else
		picture->resize(size);
}

_area2 _peach::changed()
{
	if (!picture) return _area2();
	return picture->changed_area;
}

void _peach::insert(_coordinate layer, _peach* peach)
{
	peach->parent = this;
	flavor.insert(std::pair<_coordinate, std::unique_ptr<_peach>>{ layer, peach });
	peach->add_area();
}

_picture* _peach::draw()
{
	if (!picture) return nullptr;
	if (((c2 >> 24) == 0)&&(flavor.empty())) return nullptr;
	if (picture->changed_area.empty()) return &(picture->bitmap);
	picture->bitmap.set_area(picture->changed_area);
	picture->bitmap.clear(c2);
	draw(picture->bitmap, picture->changed_area & picture->bitmap.size, _transformation2());
	picture->changed_area.clear();
	return &(picture->bitmap);
}

void _peach::draw(_bitmap& pic, const _area2 & obl_izm, _transformation2 tr)
{
	tr *= trans;
	if (((tr(calc_area())) & obl_izm).empty()) return;
	draw_one(pic, tr);
	for (auto& i : flavor) i.second->draw(pic, obl_izm, tr);
}

_area2 _peach::calc_area()
{
	if (!area_is_bad) return area;
	area_is_bad = false;
	area = local_area;
	for (auto& i : flavor) area += i.second->trans(i.second->calc_area());
	return area;
}

void _peach::add_area(_area2 a, bool start)
{
	if (start) a = calc_area();
	if (!(a <= area)) area_is_bad = true;
	_area2 ta = trans(a);
	if (picture) picture->add_obl_izm(ta);
	if (parent) parent->add_area(ta, false);
}

void _peach::del_area(_area2 a, bool start)
{
	if (start) a = calc_area();
	if (!a.inside(area)) area_is_bad = true;
	_area2 ta = trans(a);
	if (picture) picture->add_obl_izm(ta);
	if (parent) parent->del_area(ta, false);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void _p_rect::draw_one(_bitmap& pic, _transformation2 tr)
{
	_area2 oo = tr(local_area);
	pic.fill_rectangle({ {(int)oo.x.min, (int)oo.x.max}, {(int)oo.y.min, (int)oo.y.max} }, c2);
	if (((c >> 24) != 0x00) && (c != c2)) pic.rectangle((_area2i)oo, c);
}

_p_rect::_p_rect(const _area2& b, _transformation2 tr, uint c_, uint c2_)
{
	local_area = b;
	trans = tr;
	c = c_;
	c2 = c2_;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

_p_text::_p_text(std::string_view s): text(s)
{
	local_area = temp_bmp.size_text(text, 13);
	local_area.x.min = -1;
}

void _p_text::draw_one(_bitmap& pic, _transformation2 tr)
{
	int sf = (int)(13 * tr.scale + 0.5);
	if (sf < 5) return;
	pic.text((int)tr.offset.x, (int)tr.offset.y, text, sf, c, c2);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
