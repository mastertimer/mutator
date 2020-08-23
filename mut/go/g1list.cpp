#include "g1list.h"

void _g1list::init_sel() //вызывается только при отсутствии
{
	for (auto i : link)
	{
		_tetron* t = i->pairr(this);
		if (!t->operator _t_string * ()) continue;
		if (!i->test_flags(this, flag_part)) continue;
		i->set_flags(this, i->get_flags(this) | flag_specialty);
		return;
	}
}

void _g1list::ris2(_trans tr, bool final)
{
	_area_old oo2 = tr(oo);
	uint c = get_c();
	master_bm.rectangle((_area2)oo2, c);
	_t_string* s = find1<_t_string>(flag_specialty);
	if (s == nullptr)
	{
		init_sel();
		s = find1<_t_string>(flag_specialty);
		if (s == nullptr) return;
	}
	int sf = (int)(13 * tr.scale + 0.5);
	if (sf < 1) return;
	master_bm.text((int)(oo2.x.min + 4), (int)(oo2.y.min + 1), s->s.c_str(), sf, c, 0);
	if (rez1) return;
	bool start = false;
	int dx = 0;
	for (int i = (int)link.size() - 1; i >= 0; i--)
	{
		_tetron* a = link[i]->pairr(this);
		if (!a->operator std::wstring * ()) continue;
		if (!link[i]->test_flags(this, flag_part)) continue;
		if (start)
		{
			dx--;
			master_bm.text((int)(oo2.x.min + 4), (int)(oo2.y.min + dx * 16.0 * tr.scale),
			                 a->operator std::wstring *()->c_str(), sf, c, 0);
		}
		if (link[i]->test_flags(this, flag_specialty)) start = true; // если будет несколько - будут проблемы
	}
	start = false;
	dx = 0;
	for (auto i : link)
	{
		_tetron* a = i->pairr(this);
		if (!a->operator std::wstring * ()) continue;
		if (!i->test_flags(this, flag_part)) continue;
		if (start)
		{
			dx++;
			master_bm.text((int)(oo2.x.min + 4), (int)(oo2.y.min + dx * 16.0 * tr.scale),
			                 a->operator std::wstring *()->c_str(), sf, c, 0);
		}
		if (i->test_flags(this, flag_specialty)) start = true; // если будет несколько - будут проблемы
	}
}

void _g1list::calc_local_area()
{
	local_area = oo;
	if (rez1) return;
	int n = 0;
	int na = 0;
	for (auto i : link)
	{
		_tetron* t = i->pairr(this);
		if (!t->operator _t_string * ()) continue;
		if (!i->test_flags(this, flag_part)) continue;
		n++;
		if (i->test_flags(this, flag_specialty)) na = n;
	}
	local_area.y.max += ((int64)n - na) * 16;
	local_area.y.min -= (na - 1LL) * 16;
	area_definite = false;
}

_g1list::_g1list()
{
	oo = _area_old(0, 100, 0, 16);
	rez1 = true;
	local_area = oo;
}

void _g1list::mouse_finish_move()
{
	if (!rez1)
	{
		del_area();
		rez1 = true;
		calc_local_area();
		add_area();
	}
}

bool _g1list::mouse_move2(_xy r)
{
	if (!rez1)
	{
		del_area();
		rez1 = true;
		calc_local_area();
		add_area();
	}
	return true;
}

bool _g1list::mouse_wheel2(_xy r)
{
	_t_string* bb = find1<_t_string>(flag_specialty);
	if (bb == nullptr)
	{
		init_sel();
		return true;
	}
	int dx = -(int)(*n_wheel->operator int64 * ());
	if (dx == 0) return true;
	del_area();
	bool start = false;
	_tetron* b = bb;
	if (dx > 0)
	{
		for (int i = (int)link.size() - 1; i >= 0; i--)
		{
			_tetron* a = link[i]->pairr(this);
			if (!a->operator _t_string * ()) continue;
			if (!link[i]->test_flags(this, flag_part)) continue;
			if (start)
			{
				b = a;
				dx--;
				if (dx == 0) break;
			}
			if (bb == a) start = true;
		}
	}
	else
	{
		for (auto i : link)
		{
			_tetron* a = i->pairr(this);
			if (!a->operator _t_string * ()) continue;
			if (!i->test_flags(this, flag_part)) continue;
			if (start)
			{
				b = a;
				dx++;
				if (dx == 0) break;
			}
			if (bb == a) start = true;
		}
	}
	del_flags(bb, flag_specialty);
	add_flags(b, flag_specialty);
	rez1 = false;
	calc_local_area();
	add_area();
	return true;
}
