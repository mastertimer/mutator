#include "go_lite.h"
#include "g_button.h"

void _g_button::RisIco(const char* kod, const char* s)
{
	picture.resize(24, 24);
	picture.clear(0xFF000000);
	for (; *kod; kod++)
	{
		if (*kod == 'a')
		{

		}
	}
	if (*s == 0) return;
	int n = 1;
	for (const char* i = s; *i; i++)
		if (*i == '\n') n++;
	int y0 = -5;
	int dy = 6;
	if (n == 1)	y0 = 4;
	if (n == 2)
	{
		y0 = -1;
		dy = 10;
	}
	if (n == 3)
	{
		y0 = -3;
		dy = 7;
	}
	const char* ss = s;
	int y = y0;
	for (const char* i = s; *i; i++)
		if (*i == '\n')
		{
			picture.text16(0, y, ss, c_def);
			y += dy;
			ss = i + 1;
		}
	picture.text16(0, y, ss, c_def);
}

void _g_button::ris2(_trans tr, bool final)
{
	_area_old oo = tr(local_area);
	uint c = 0;
	if (picture.size.x * picture.size.y > 0)
	{
		int rx2 = (int)(picture.size.x * tr.scale + 0.5);
		int ry2 = (int)(picture.size.y * tr.scale + 0.5);
		_ixy ce = oo.center();
		master_bm.stretch_draw3(&picture, ce.x - rx2 / 2, ce.y - ry2 / 2, tr.scale);
	}
	else
		c = c_def;
	if (checked) c = 0x40000000 + c_def;
	if (n_go_move == this) c = 0x80000000 + c_def;
	if (n_tani == this) c = 0x80000000 + c_max;
	master_bm.fill_rectangle({ {(int)oo.x.min, (int)oo.x.max+1}, {(int)oo.y.min, (int)oo.y.max+1} }, c);
}

void _g_button::push(_stack* mem)
{
	_t_go::push(mem);
	*mem << checked << hint << picture;
}

void _g_button::pop(_stack* mem)
{
	_t_go::pop(mem);
	*mem >> checked >> hint >> picture;
}

void _g_button::push(_wjson& b)
{
	_t_go::push(b);
	b.add("checked", checked);
	b.add("hint", hint);
	b.add("picture", picture);
}

void _g_button::pop(_rjson& b)
{
	_t_go::pop(b);
	b.read("checked", checked);
	b.read("hint", hint);
	b.read("picture", picture);
}

_g_button::_g_button() : checked(0)
{
	local_area = _area_old(0, 26, 0, 26);
}

bool _g_button::mouse_move2(_xy r)
{
	if (n_go_move != this) // первое перемещение
	{
		cha_area();
	}
	if (!n_hint) add_hint(hint, this); // нет подсказки
	return true;
}

void _g_button::mouse_finish_move()
{
	cha_area();
	del_hint();
}

bool _g_button::mouse_down_left2(_xy r)
{
	if (test_flags(n_checkbox, flag_parent)) checked = !checked;
	run(this, this, flag_run);
	cha_area();
	return true;
}

