#include "g_scrollbar.h"

void _g_scrollbar::push(_wjson& b)
{
	_t_go::push(b);
	b.add("position", position);
	b.add("vid", vid);
}

void _g_scrollbar::pop(_rjson& b)
{
	_t_go::pop(b);
	b.read("position", position);
	b.read("vid", vid);
}

void _g_scrollbar::prilip(_t_go* r)
{
	if (!r) return;
	del_area();
	double l = ((vid & 1) == 1) ? local_area.x.length() : local_area.y.length();
	_area_old& o = r->local_area;
	if (vid == 2) local_area = _area_old(o.x.min, o.x.max, o.y.max, o.y.max + l);
	if (vid == 3) local_area = _area_old(o.x.max, o.x.max + l, o.y.min, o.y.max);
	if (vid == 4) local_area = _area_old(o.x.min, o.x.max, o.y.min - l, o.y.min);
	if (vid == 5) local_area = _area_old(o.x.min - l, o.x.min, o.y.min, o.y.max);
	//	if (vid > 1) trans = _trans(); //глобальная замена trans
	area = _tarea::indefinite;
	add_area();
}

void _g_scrollbar::after_create_link(_link* li)
{
	if (vid > 1)
		if (li->test_flags(this, inverted_flags(flag_sub_go)))
		{
			_t_go* r = *li->pairr(this);
			prilip(r);
		}
	_t_go::after_create_link(li);
}

void _g_scrollbar::ris2(_trans tr, bool final)
{
	_area_old a = tr(local_area);
	uint c = c_def;
	if ((vid & 1) == 0)
	{
		master_bm.line({ (int)a.x.min, (int)a.y(0.5) }, { (int)a.x.max, (int)a.y(0.5) }, c);
		master_bm.line({ (int)a.x(position), (int)a.y.min }, { (int)a.x(position), (int)a.y.max }, c);
	}
	else
	{
		master_bm.line({ (int)a.x(0.5), (int)a.y.min }, { (int)a.x(0.5), (int)a.y.max }, c);
		master_bm.line({ (int)a.x.min, (int)a.y(position) }, { (int)a.x.max, (int)a.y(position) }, c);
	}
}

void _g_scrollbar::mouse_move_left2(_xy r)
{
	double ii;
	if ((vid & 1) == 0)
		ii = (r.x - local_area.x.min) / local_area.x.length();
	else
		ii = (r.y - local_area.y.min) / local_area.y.length();
	if (ii < 0) ii = 0;
	if (ii > 1) ii = 1;
	if (ii != position)
	{
		position = ii;
		cha_area();
		run(0, this, flag_run);
	}
}
