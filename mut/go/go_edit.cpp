#include "g_tetron.h"
#include "basic_tetrons.h"
#include "g_scrollbar.h"
#include "go_edit.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void _g_edit_double::ris2(_trans tr, bool final)
{
	_g_rect::ris2(tr, final);
	_area_old oo = tr(local_area);
	uint c0 = get_c();
	int sf2 = (int)(13 * tr.scale + 0.5);
	if (sf2 < 5) return;
	std::wstring s = double_to_string(a, 2);

	master_bm.text((int)(oo.x.min + 5), (int)(oo.y.min + 1), s.c_str(), sf2, c0, 0xff000000);
	if (n_act_key == this)
	{
		vec2i size = master_bm.size_text(s.substr(/*first_+*/ 0, cursor /*-first_*/).c_str(), sf2);
		master_bm.line({ (int)(oo.x.min + 4 + size.x), (int)(oo.y.min + 1) }, { (int)(oo.x.min + 4 + size.x), (int)(oo.y.min + sf2) }, c0);
	}
}

void _g_edit_double::key_down(ushort key)
{
	if (key == 37) // left
	{
		if (cursor > 0)
		{
			cursor--;
			cha_area();
		}
		return;
	}
	std::wstring s = double_to_string(a, 2);
	if (key == 8) // backspace
	{
		if ((cursor > 0) && (cursor < (int)s.size() - 2))
		{
			int64 b = (int64)a;
			int64 c = 1;
			for (int j = int(s.size() - 3); j > cursor; j--) c *= 10;
			a = (b / (c * 10)) * c + (b % c) + 0.1 * (s[s.size() - 2] - L'0') + 0.01 * (s[s.size() - 1] - L'0');
			cursor--;
			cha_area();
		}
		return;
	}
	if (key == 39) // right
	{
		if (cursor < (int)s.size())
		{
			cursor++;
			cha_area();
		}
		return;
	}
	if (key == 46) // delete
	{
		if (cursor < (int)s.size() - 3)
		{
			int64 b = (int64)a;
			int64 c = 1;
			for (int j = int(s.size() - 4); j > cursor; j--) c *= 10;
			a = (b / (c * 10)) * c + (b % c) + 0.1 * (s[s.size() - 2] - L'0') + 0.01 * (s[s.size() - 1] - L'0');
			cha_area();
		}
		return;
	}
}

void _g_edit_double::key_press(ushort key)
{
	if ((key < L'0') || (key > L'9')) return;
	cha_area();
	std::wstring s = double_to_string(a, 2);
	if (cursor == s.size()) return;
	int64 b = (int64)a;
	if (cursor == s.size() - 1)
		a = b + 0.1 * (s[s.size() - 2] - L'0') + 0.01 * (key - L'0');
	else
		if (cursor == s.size() - 2)
			a = b + 0.01 * (s[s.size() - 1] - L'0') + 0.1 * (key - L'0');
		else
		{
			if (b == 0)
			{
				a = (key - L'0') + 0.1 * (s[s.size() - 2] - L'0') + 0.01 * (s[s.size() - 1] - L'0');
				cursor = 0;
			}
			else
			{
				int64 c = 1;
				for (int j = int(s.size() - 3); j > cursor; j--) c *= 10;
				a = (b / c) * (c * 10) + (b % c) + (key - L'0') * c + 0.1 * (s[s.size() - 2] - L'0') + 0.01 *
					(s[s.size() - 1] - L'0');
			}
		}
	cursor++;
}

_g_edit_double::_g_edit_double() : cursor(0), a(0.0)
{
	local_area = _area_old(0, 100, 0, 16);
	key_fokus = true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void _g_edit_int::ris2(_trans tr, bool final)
{
	_area_old oo = tr(local_area);
	uint c0 = get_c();
	master_bm.rectangle((_area2)oo, c0);
	int sf2 = (int)(13 * tr.scale + 0.5);
	if (sf2 < 5) return;
	int64* a = find1_plus_gtetron<int64>(this, flag_specialty);
	if (!a)
	{
		master_bm.line(oo.top_left(), oo.bottom_right(), c0);
		master_bm.line(oo.bottom_left(), oo.top_right(), c0);
		return;
	}
	bool hex = test_flags(n_hex, flag_information);
	std::wstring s = (hex) ? std::wstring(uint64_to_wstr_hex(*a)) : std::to_wstring(*a);
	master_bm.text((int)(oo.x.min + 5), (int)(oo.y.min + 1), s.c_str(), sf2, c0, 0xff000000);
	if (n_act_key == this)
	{
		vec2i size = master_bm.size_text(s.substr(/*first_+*/ 0, cursor /*-first_*/).c_str(), sf2);
		master_bm.line({ (int)(oo.x.min + 4 + size.x), (int)(oo.y.min + 1) }, { (int)(oo.x.min + 4 + size.x),
			(int)(oo.y.min + sf2) }, c0);
	}
}

void _g_edit_int::key_down(ushort key)
{
	int64* a = find1_plus_gtetron<int64>(this, flag_specialty);
	if (!a) return;
	bool hex = test_flags(n_hex, flag_information);

	if (key == 8) // backspace
	{
		if (cursor > 0)
		{
			if (hex)
			{
				*a /= 16;
			}
			else
			{
				int l = (int)std::to_wstring(*a).size();
				if (cursor > l) cursor = l;
				int64 mn = 1;
				for (int i = cursor; i < l; i++) mn *= 10;
				*a = (*a / (mn * 10)) * mn + (*a % mn);
			}
			cursor--;
			cha_area();
		}
		return;
	}
	if (key == 37) // left
	{
		if (cursor > 0)
		{
			cursor--;
			cha_area();
		}
		return;
	}
	int l = (int)((hex) ? std::wstring(uint64_to_wstr_hex(*a)) : std::to_wstring(*a)).size();
	if (key == 39) // right
	{
		if (cursor < l)
		{
			cursor++;
			cha_area();
		}
		return;
	}
	if (key == 46) // delete
	{
		if (cursor < l)
		{
			if (hex)
			{
				*a /= 16;
			}
			else
			{
				int l = (int)std::to_wstring(*a).size();
				if (cursor > l) cursor = l;
				int64 mn = 1;
				for (int i = cursor; i < l; i++) mn *= 10;
				*a = (*a / mn) * (mn / 10) + (*a % (mn / 10));
			}
			cha_area();
		}
		return;
	}
}

bool _g_edit_int::mouse_wheel2(_xy r)
{
	int64* a = find1_plus_gtetron<int64>(this, flag_specialty);
	if (!a) return true;
	*a += *n_wheel->operator int64 * ();
	cha_area();
	return true;
}

void _g_edit_int::key_press(ushort key)
{
	int64* a = find1_plus_gtetron<int64>(this, flag_specialty);
	if (!a) return;
	if (test_flags(n_hex, flag_information))
	{
		if ((key >= L'0') && (key <= L'9'))* a = *a * 16 + (key - L'0');
		if ((key >= L'a') && (key <= L'f'))* a = *a * 16 + (key - L'a') + 10;
		if ((key >= L'A') && (key <= L'F'))* a = *a * 16 + (key - L'A') + 10;
	}
	else
		if ((key >= L'0') && (key <= L'9'))
		{
			if (*a == 0) cursor = 1; // особый случай
			int l = (int)std::to_wstring(*a).size();
			if (cursor > l) cursor = l;
			int64 mn = 1;
			for (int i = cursor; i < l; i++) mn *= 10;
			*a = (*a / mn) * (mn * 10) + (key - L'0') * mn + (*a % mn);
			l = (int)std::to_wstring(*a).size();
			if (cursor < l) cursor++;
		}
	cha_area();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void _g_edit_string::ris2(_trans tr, bool final)
{
	_area_old oo = tr(local_area);
	uint c0 = get_c();
	master_bm.rectangle((_area2)oo, c0);
	std::wstring* s = find1_plus_gtetron<std::wstring>(this, flag_specialty);
	if (!s)
	{
		master_bm.line(oo.top_left(), oo.bottom_right(), c0);
		master_bm.line(oo.bottom_left(), oo.top_right(), c0);
		return;
	}
	int sf = (int)(13 * tr.scale + 0.5);
	if (sf < 1) return;
	int l = (int)s->size();
	if (cursor < 0) cursor = 0;
	if (cursor > l) cursor = l;
	if (first > cursor) first = cursor;
	if (first + len2 > l) len2 = l - first;
	if (first + len2 < cursor) first = cursor - len2;
	int rx_text = (int)(oo.x.length() - 8);
	if ((l > 0) && (rx_text > 3))
	{
		int l2_min = 1;
		int l2_max = l - first;
		vec2i size = master_bm.size_text(s->substr(first, len2).c_str(), sf);
		if (size.x > rx_text)
		{
			while (first + len2 > cursor)
			{
				len2--;
				size = master_bm.size_text(s->substr(first, len2).c_str(), sf);
				if (size.x <= rx_text) break;
			}
			if (size.x > rx_text)
				while (first < cursor)
				{
					len2--;
					first++;
					size = master_bm.size_text(s->substr(first, len2).c_str(), sf);
					if (size.x <= rx_text) break;
				}
		}
		else if (size.x < rx_text)
		{
			while (first + len2 < l)
			{
				size = master_bm.size_text(s->substr(first, len2 + 1LL).c_str(), sf);
				if (size.x > rx_text) break;
				len2++;
			}
			if (size.x < rx_text)
				while (first > 0)
				{
					size = master_bm.size_text(s->substr(first - 1LL, len2 + 1LL).c_str(), sf);
					if (size.x > rx_text) break;
					len2++;
					first--;
				}
		}
		master_bm.text((int)(oo.x.min + 5), (int)oo.y.min, s->substr(first, len2).c_str(), sf, c0, 0xff000000);
		if (first > 0)
			master_bm.line({ (int)(oo.x.min + 2), (int)oo.y.min }, { (int)(oo.x.min + 2), (int)oo.y.max }, 0x30C0F0);
		if (len2 < l - first)
			master_bm.line({ (int)(oo.x.max - 2), (int)oo.y.min }, { (int)(oo.x.max - 2), (int)oo.y.max },
				0x30C0F0);
	}
	if (n_act_key == this)
	{
		vec2i size = master_bm.size_text(s->substr(first, (int64)cursor - first).c_str(), sf);
		master_bm.line({ (int)(oo.x.min + 4 + size.x), (int)(oo.y.min + 1) }, { (int)(oo.x.min + 4 + size.x), (int)(oo.y.min + sf) }, c0);
	}
}

void _g_edit_string::key_down(ushort key)
{
	std::wstring* s = find1_plus_gtetron<std::wstring>(this, flag_specialty);
	if (!s) return;
	if (key == 8) // backspace
	{
		if (cursor > 0)
		{
			s->erase(cursor - 1LL, 1);
			cursor--;
			cha_area();
		}
		return;
	}
	if (key == 37) // left
	{
		if (cursor > 0)
		{
			cursor--;
			cha_area();
		}
		return;
	}
	int l = (int)s->size();
	if (key == 39) // right
	{
		if (cursor < l)
		{
			cursor++;
			cha_area();
		}
		return;
	}
	if (key == 46) // delete
	{
		if (cursor < l)
		{
			s->erase(cursor, 1);
			cha_area();
		}
		return;
	}
}

void _g_edit_string::key_press(ushort key)
{
	if (key < 32) return;
	std::wstring* s = find1_plus_gtetron<std::wstring>(this, flag_specialty);
	if (!s) return;
	s->insert(cursor, 1, key);
	cursor++;
	cha_area();
}

_g_edit_string::_g_edit_string()
{
	key_fokus = true;
	local_area = _area_old(0, 100, 0, 16);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void _g_edit_multi_string::ris2(_trans tr, bool final)
{
	_area_old oo = tr(local_area);
	if ((oo.x.length() <= 3) || (oo.y.length() <= 3)) return;
	_multi_string* str = find1_plus_gtetron<_multi_string>(this, flag_specialty);
	if (!str) str = &strings;
	master_bm.rectangle((_area2)oo, c_def);
	if (str != &strings)
	{
		_area_old ooo = oo;
		ooo = ooo.expansion(-1.0);
		master_bm.rectangle((_area2)ooo, c_def);
	}
	len2 = (int)((local_area.y.length() - 8) / 16);
	if (len2 == 0) return;
	int sf = (int)(13 * tr.scale + 0.5);
	if (sf < 5) return;
	if (_g_scrollbar * polz = find1<_g_scrollbar>(flag_part))
	{
		int k = (int)str->line.size() - len2;
		if (k < 0) k = 0;
		max_i = k;
		if (k >= 1)
		{
			first = (int)(polz->position * k + 0.5);
			if (cursor.y < first) cursor.y = first;
			if (cursor.y >= first + len2) cursor.y = first + len2 - 1;
		}
	}
	if (cursor.y >= (int)str->line.size()) cursor.y = (int)str->line.size() - 1;
	if (cursor.y < 0) cursor.y = 0;
	if (first > cursor.y) first = cursor.y;
	if (first + len2 > (int)str->line.size()) first = (int)str->line.size() - len2;
	if (first < 0) first = 0;
	int k = len2;
	if ((int)str->line.size() < first + k) k = (int)str->line.size() - first;
	for (int i = 0; i < k; i++)
	{
		int ii = first + i;
		master_bm.text((int)(oo.x.min + 4 * tr.scale), (int)(oo.y.min + (i * 16LL + 4) * tr.scale),
			str->line[ii].c_str(), sf, c_def, 0xff000000);
	}
	if (first) master_bm.line({ (int)(oo.x.min + 2 * tr.scale), (int)(oo.y.min + 2 * tr.scale) },
		{ (int)(oo.x.max - 2 * tr.scale), (int)(oo.y.min + 2 * tr.scale) }, 0x30C0F0);
	if (first + k < (int)str->line.size()) master_bm.line({ (int)(oo.x.min + 2 * tr.scale), (int)(oo.y.max - 2 * tr.scale) },
		{ (int)(oo.x.max - 2 * tr.scale), (int)(oo.y.max - 2 * tr.scale) }, 0x30C0F0);

	if (n_act_key == this)
	{
		vec2i size = master_bm.size_text(str->line[cursor.y].substr(0, cursor.x).c_str(), sf);
		master_bm.line({ (int)(oo.x.min + 4 * tr.scale + size.x - 1),
			(int)(oo.y.min + (((int64)cursor.y - first) * 16 + 4) * tr.scale) },
			{ (int)(oo.x.min + 4 * tr.scale + size.x - 1),
			(int)(oo.y.min + ((cursor.y + 1LL - first) * 16 + 4) * tr.scale) }, c_def);
	}
}

void _g_edit_multi_string::key_down(ushort key)
{
	_multi_string* str = find1_plus_gtetron<_multi_string>(this, flag_specialty);
	if (!str) str = &strings;
	if (str->line.size() == 0) return;
	if (cursor.y >= (int)str->line.size()) cursor.y = (int)str->line.size() - 1;

	if (key == 8) // backspace
	{
		int ll = 0;
		if (cursor.y) ll = (int)str->line[cursor.y - 1LL].size();
		if (str->delete_char(cursor.y, cursor.x - 1))
		{
			cursor.x--;
			if (cursor.x < 0)
			{
				cursor.x = ll;
				cursor.y--;
			}
			cha_area();
		}
		return;
	}
	if (key == 13) // enter
	{
		str->div2line(cursor.y, cursor.x);
		cursor.y++;
		cursor.x = 0;
		cha_area();
	}
	if (key == 37) // left
	{
		if (cursor.x > 0)
		{
			cursor.x--;
			cha_area();
		}
		else
			if (cursor.y > 0)
			{
				cursor.y--;
				cursor.x = (int)str->line[cursor.y].size();
				cha_area();
			}

		return;
	}
	if (key == 38) // up
	{
		if (cursor.y > 0)
		{
			cursor.y--;
			int l = (int)str->line[cursor.y].size();
			if (cursor.x > l) cursor.x = l;
			if (_g_scrollbar * polz = find1<_g_scrollbar>(flag_part))
			{
				int first = (int)(polz->position * max_i + 0.5);
				if (first > cursor.y)
				{
					polz->position = (double)cursor.y / max_i;
					polz->cha_area();
				}
			}
		}
		else
			cursor.x = 0;
		cha_area();
		return;
	}
	int l = (int)str->line[cursor.y].size();
	if (key == 39) // right
	{
		if (cursor.x < l)
		{
			cursor.x++;
			cha_area();
		}
		else
			if (cursor.y < (int)str->line.size() - 1)
			{
				cursor.y++;
				cursor.x = 0;
				cha_area();
			}
		return;
	}
	if (key == 40) // down
	{
		if (cursor.y < (int)str->line.size() - 1)
		{
			cursor.y++;
			int l = (int)str->line[cursor.y].size();
			if (cursor.x > l) cursor.x = l;
			if (_g_scrollbar * polz = find1<_g_scrollbar>(flag_part))
			{
				int first = (int)(polz->position * max_i + 0.5);
				if (cursor.y >= first + len2)
				{
					polz->position = (double)(1LL + cursor.y - len2) / max_i;
					polz->cha_area();
				}
			}
		}
		else
			cursor.x = (int)str->line[cursor.y].size();
		cha_area();
		return;
	}
	if (key == 46) // delete
	{
		if (str->delete_char(cursor.y, cursor.x)) cha_area();
		return;
	}
}

void _g_edit_multi_string::key_press(ushort key)
{
	if (key < 32) return;
	_multi_string* str = find1_plus_gtetron<_multi_string>(this, flag_specialty);
	if (!str) str = &strings;
	str->insert_char(cursor.y, cursor.x++, key);
	cha_area();
}

_g_edit_multi_string::_g_edit_multi_string() : cursor{ 0,0 }
{
	local_area = _area_old(0, 100, 0, 100);
	key_fokus = true;
}

void _g_edit_multi_string::run(_tetron* tt0, _tetron* tt, uint64 flags)
{
	cha_area();
}

bool _g_edit_multi_string::mouse_down_left2(_xy r)
{
	if (!find1<_g_scrollbar>(flag_information))
	{
		_g_scrollbar* sb = new _g_scrollbar;
		sb->vid = 3;
		add_flags(sb, flag_sub_go + flag_part + flag_information + (flag_run << 32));
	}
	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool _g_edit64bit::mouse_down_left2(_xy r)
{
	if (act < 0) return true;
	a ^= (1LL << act);
	cha_area();
	return true;
}

void _g_edit64bit::mouse_finish_move()
{
	if (act < 0) return;
	act = -1;
	cha_area();
	del_hint();
}

bool _g_edit64bit::mouse_move2(_xy r)
{
	double d1 = local_area.x.length();
	double d2 = local_area.y.length();
	if ((d1 == 0) || (d2 == 0)) return true;
	int act2 = act;
	int x = (int)((r.x - local_area.x.min) * 8 / d1);
	int y = (int)((r.y - local_area.y.min) * 8 / d2);
	if (x < 0) x = 0;
	if (y < 0) y = 0;
	if (x >= 8) x = 7;
	if (y >= 8) y = 7;
	act = y * 8 + x;
	if (act2 != act)
	{
		cha_area();
		_multi_string* s = find_intermediate<_multi_string>(n_zagolovok, flag_information, flag_parent);
		std::wstring   ss;
		if (s)
			if ((int)s->line.size() > act) ss = s->line[act];
		if (ss.empty()) ss = std::to_wstring(act);
		add_hint(ss, this);
	}
	return true;
}

void _g_edit64bit::ris2(_trans tr, bool final)
{
	_area_old  oo = tr(local_area);
	double d = oo.x.length();
	double d2 = oo.y.length();
	if ((d < 5) || (d2 < 5)) return;
	uint64* c = (uint64*)& a;
	for (int j = 0; j < 8; j++)
		for (int i = 0; i < 8; i++)
			if (*c & (1ULL << (j * 8 + i)))
				master_bm.fill_rectangle({ {(int)(oo.x.min + 1 + (d - 1) * 0.125 * i),
					(int)(oo.x.min + (d - 1) * 0.125 * (i + 1.0))+1},
					{(int)(oo.y.min + 1 + (d2 - 1) * 0.125 * j),
					(int)(oo.y.min + (d2 - 1) * 0.125 * (j + 1.0))+1} }, c_max);
	if ((d < 25) || (d2 < 25)) { master_bm.rectangle((_area2)oo, c_def); }
	else
	{
		for (int i = 0; i <= 8; i++)
		{
			master_bm.line({ (int)oo.x.min, (int)(oo.y.min + d2 * 0.125 * i) },
				{ (int)oo.x.max, (int)(oo.y.min + d2 * 0.125 * i) }, c_def);
			master_bm.line({ (int)(oo.x.min + d * 0.125 * i), (int)oo.y.min },
				{ (int)(oo.x.min + d * 0.125 * i), (int)oo.y.max }, c_def);
		}
		if (act >= 0)
		{
			int j = act / 8;
			int i = act % 8;
//			master_bm.rectangle((int)(oo.x.min + d * 0.125 * i), (int)(oo.y.min + d2 * 0.125 * j),
//				(int)(oo.x.min + d * 0.125 * (i + 1.0)), (int)(oo.y.min + d2 * 0.125 * (j + 1.0)),
//				0xFF0000);
			master_bm.rectangle({ {(int64)(oo.x.min + d * 0.125 * i), (int64)(oo.x.min + d * 0.125 * (i + 1.0))+1},
				{(int64)(oo.y.min + d2 * 0.125 * j), (int64)(oo.y.min + d2 * 0.125 * (j + 1.0))+1} }, 0xFFFF0000);
		}
	}
}

_g_edit64bit::_g_edit64bit()
{
	local_area = _area_old(0, 58, 0, 58);
	a = 0;
	act = -1;
}

void _g_edit64bit::run(_tetron* tt0, _tetron* tt, uint64 flags) { cha_area(); }

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool _g_edit_one::mouse_down_left2(_xy r)
{
	_area_old aa = local_area;
	aa.x.max = aa.x(0.12);
	if (aa.test(r))
	{
		mode = (int)(aa.y.get_k(r.y) * 2);
		if (mode < 0) mode = 0;
		if (mode > 1) mode = 1;
		_one_tetron* ot = find1_plus_gtetron<_one_tetron>(this, flag_specialty);
		if (ot)
		{
			if (mode == 0) a_i = ot->i[n] + 1;   // чтобы обновилось
			if (mode == 1) a = ot->d[n] * 2 + 1; // чтобы обновилось
		}
		cursor = 0;
		cha_area();
		return true;
	}
	aa = local_area;
	aa.x.max = aa.x(0.3);
	if (aa.test(r))
	{
		n = (n + 1) % 16;
		_one_tetron* ot = find1_plus_gtetron<_one_tetron>(this, flag_specialty);
		if (ot)
		{
			if (mode == 0) a_i = ot->i[n] + 1;   // чтобы обновилось
			if (mode == 1) a = ot->d[n] * 2 + 1; // чтобы обновилось
		}
		cursor = 0;
		cha_area();
	}
	return true;
}

void _g_edit_one::ris2(_trans tr, bool final)
{
	_g_rect::ris2(tr, final);
	_area_old bb = tr(local_area);
	_area_old oo = bb;
	oo.x.min = bb.x(0.3);
	oo.x.max = bb.x(0.99);
	oo.y.min = bb.y(0.01);
	oo.y.max = bb.y(0.99);
	master_bm.rectangle((_area2)oo, c_def);
	uint c0 = get_c();
	int  sf2 = (int)(13 * tr.scale + 0.5);
	if (sf2 < 5) return;
	if (mode == 0)
		master_bm.fill_rectangle({ {(int)(bb.x(0.03)), (int)(bb.x(0.12))+1}, {(int)(bb.y(0.1)), (int)(bb.y(0.45))+1} }, c_def);
	else
		master_bm.rectangle({ {(int64)(bb.x(0.03)), (int64)(bb.x(0.12)) + 1}, {(int64)(bb.y(0.1)), (int64)(bb.y(0.45)) + 1} }, c_def);
	if (mode == 1)
		master_bm.fill_rectangle({ {(int)(bb.x(0.03)), (int)(bb.x(0.12))+1}, {(int)(bb.y(0.55)), (int)(bb.y(0.9))+1} }, c_def);
	else
		master_bm.rectangle({ {(int64)(bb.x(0.03)), (int64)(bb.x(0.12)) + 1}, {(int64)(bb.y(0.55)), (int64)(bb.y(0.9)) + 1} }, c_def);
	master_bm.text((int)(bb.x(0.15)), (int)(bb.y.min + 5), std::to_wstring(n).c_str(), sf2, c0, 0xff000000);
	_one_tetron* ot = find1_plus_gtetron<_one_tetron>(this, flag_specialty);
	if (!ot) return;
	if (mode == 0)
	{
		if (a_i != ot->i[n])
		{
			a_i = ot->i[n];
			s = std::to_wstring(a_i);
		}
		master_bm.text((int)(oo.x.min + 5), (int)(oo.y.min + 1), s.c_str(), sf2, c0, 0xff000000);
		if (n_act_key == this)
		{
			vec2i size = master_bm.size_text(s.substr(/*first_+*/ 0, cursor /*-first_*/).c_str(), sf2);
			master_bm.line({ (int)(oo.x.min + 4 + size.x), (int)(oo.y.min + 1) },
				{ (int)(oo.x.min + 4 + size.x), (int)(oo.y.min + sf2) }, c0);
		}
		return;
	}
	if (mode == 1)
	{
		if (a != ot->d[n])
		{
			a = ot->d[n];
			s = std::to_wstring(a);
		}
		master_bm.text((int)(oo.x.min + 5), (int)(oo.y.min + 1), s.c_str(), sf2, c0, 0xff000000);
		if (n_act_key == this)
		{
			vec2i size = master_bm.size_text(s.substr(/*first_+*/ 0, cursor /*-first_*/).c_str(), sf2);
			master_bm.line({ (int)(oo.x.min + 4 + size.x), (int)(oo.y.min + 1) },
				{ (int)(oo.x.min + 4 + size.x), (int)(oo.y.min + sf2) }, c0);
		}
		return;
	}
}

void _g_edit_one::key_down(ushort key)
{
	_one_tetron* ot = find1_plus_gtetron<_one_tetron>(this, flag_specialty);
	if (!ot) return;
	if (mode == 0)
		if (a_i != ot->i[n])
		{
			a_i = ot->i[n];
			s = std::to_wstring(a_i);
		}
	if (mode == 1)
		if (a != ot->d[n])
		{
			a = ot->d[n];
			s = std::to_wstring(a);
		}
	if (cursor > s.length()) cursor = s.length();

	switch (key)
	{
	case 8: // backspace
		if (cursor > 0)
		{
			s.erase(cursor - 1, 1);
			cursor--;
		}
		break;
	case 13: // enter
		if (mode == 0)
		{
			std::wistringstream(s) >> a_i;
			ot->i[n] = a_i;
			s = std::to_wstring(a_i);
		}
		if (mode == 1)
		{
			std::wistringstream(s) >> a;
			ot->d[n] = a;
			s = std::to_wstring(a);
		}
		cursor = 0;
		break;
	case 37: // left
		if (cursor > 0) cursor--;
		break;
	case 39: // right
		if (cursor < s.length()) cursor++;
		break;
	case 46: // delete
		if (cursor < s.length()) s.erase(cursor, 1);
		break;
	default: break;
	}
	cha_area();
}

void _g_edit_one::key_press(ushort key)
{
	if (!(((key >= L'0') && (key <= L'9')) || (key == L'.') || (key == L'-') || (key == L'e') || (key == L'E'))) return;
	_one_tetron* ot = find1_plus_gtetron<_one_tetron>(this, flag_specialty);
	if (!ot) return;
	if (mode == 0)
		if (a_i != ot->i[n])
		{
			a_i = ot->i[n];
			s = std::to_wstring(a_i);
		}
	if (mode == 1)
		if (a != ot->d[n])
		{
			a = ot->d[n];
			s = std::to_wstring(a);
		}
	if (cursor > s.length()) cursor = s.length();
	if (s == L"0")
	{
		s[0] = key;
		cursor = 1;
	}
	else
	{
		s.insert(cursor, 1, key);
		cursor++;
	}
	cha_area();
}

_g_edit_one::_g_edit_one()
{
	local_area = _area_old(0, 100, 0, 30);
	key_fokus = true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
