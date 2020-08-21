#include "g_scrollbar.h"
#include "go_edit.h"
#include "g_tetron.h"
#include "t_function.h"
#include "g_list_link.h"

bool _g_list_link::mouse_down_left2(_xy r)
{
	if (!find1<_g_scrollbar>(flag_part))
	{
		_g_scrollbar* sb = new _g_scrollbar;
		sb->vid = 3;
		add_flags(sb, flag_sub_go + flag_part + (flag_run << 32));
	}
	cursor = (int)(first + (r.y - 4) / 16);
	if (*n_s_double->operator int64 * ())
	{
		_g_tetron* g = find1<_g_tetron>(flag_specialty);
		if (g == nullptr) return true;
		_tetron* a = g->find1<_tetron>(flag_specialty);
		if (a == nullptr) return true;
		if ((cursor < 0) || (cursor >= (int)a->link.size())) return true;
		_id t = a->link[cursor]->pairr(a);
		n_act = t; // заменить на новое!!
		smena_avt(); // заменить на новое!!
		g->add_unique_flags(t, flag_specialty);
		g->run(0, g, flag_run);
	}
	else
		run(0, this, flag_run); // ChaArea();
	return true;
}

void _g_list_link::ris2(_trans tr, bool final)
{
	_g_rect::ris2(tr, final);
	int sf = (int)(13 * tr.scale + 0.5);
	if (sf < 1) return;
	_g_tetron* g = find1<_g_tetron>(flag_specialty);
	if (g == nullptr) return;
	_tetron* a = g->find1<_tetron>(flag_specialty);
	if (a == nullptr) return;
	if (a->link.size() == 0) return;
	_area_old oo = tr(local_area);
	int len2 = (int)((local_area.y.length() - 8) / 16);
	if (len2 < 1) return;
	_g_scrollbar* polz = find1<_g_scrollbar>(flag_part);
	if (polz)
	{
		int k = (int)a->link.size() - len2;
		if (k >= 1)
		{
			first = (int)(polz->position * k + 0.5);
			if (cursor < first) cursor = first;
			if (cursor >= first + len2) cursor = first + len2 - 1;
		}
	}
	if (cursor >= (int)a->link.size()) cursor = (int)a->link.size() - 1;
	if (cursor < 0) cursor = 0;
	if (first > cursor) first = cursor;
	if (first + len2 > (int)a->link.size()) first = (int)a->link.size() - len2;
	if (first < 0) first = 0;
	int k = len2;
	if ((int)a->link.size() < first + k) k = (int)a->link.size() - first;
	for (int i = 0; i < k; i++)
	{
		int ii = first + i;
		std::wstring s;
		s = string_to_wstring(a->link[ii]->pairr(a)->name());
		//		s = int_to_hex((uint64)a->link[ii].tetron);
		//		int nn = a->link[ii].nom_link();
		//			s += L" " + IntToString(nn);
		s = std::to_wstring(ii) + L". " + s;
		uint color = c_def;
		master_bm.text((int)(oo.x.min + 4 * tr.scale), (int)(oo.y.min + (i * 16.0 + 4) * tr.scale), s.c_str(), sf,
		                 color, 0xff000000);
	}
	if (first) master_bm.line({ (int)(oo.x.min + 2 * tr.scale), (int)(oo.y.min + 2 * tr.scale) },
		{ (int)(oo.x.max - 2 * tr.scale), (int)(oo.y.min + 2 * tr.scale) }, 0x30C0F0);
	if (first + k < (int)a->link.size()) master_bm.line({ (int)(oo.x.min + 2 * tr.scale), (int)(oo.y.max - 2 * tr.scale) },
		{ (int)(oo.x.max - 2 * tr.scale),	(int)(oo.y.max - 2 * tr.scale) }, 0x30C0F0);
	master_bm.rectangle({ {(int64)(oo.x.min + 2 * tr.scale), (int64)(oo.x.max - 2 * tr.scale) + 1},
		{(int64)(oo.y.min + (4 + ((int64)cursor - first) * 16) * tr.scale),
		(int64)(oo.y.min + (19 + ((int64)cursor - first) * 16) * tr.scale) + 1} }, c_def);
	if ((tf != a) || (cursor != curf))
	{
		tf = a;
		curf = cursor;
		int64 a1 = 0;
		if ((cursor >= 0) || (cursor < (int)a->link.size()))
			if (a->link[cursor]->pairr(a))
				a1 = a->link[cursor]->get_flags(a);
		_g_edit64bit* ee = find1<_g_edit64bit>(flag_information);
		if (ee)
		{
			ee->a = a1;
			//			ee->Recovery(); // должны рисоваться после этого списка или все вычисления вынести из Ris в run
		}
	}
}

void _g_list_link::run(_tetron* tt0, _tetron* tt, uint64 flags)
{
	cha_area();
	_g_edit64bit* ee = find1<_g_edit64bit>(flag_information);
	if (ee) ee->cha_area();
}

