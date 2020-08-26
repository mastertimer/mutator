#include "t_function.h"
#include "go.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void _g_edit_double::ris2(_trans tr, bool final)
{
	_g_rect::ris2(tr, final);
	_area2 oo = tr(local_area);
	uint c0 = get_c();
	int sf2 = (int)(13 * tr.scale + 0.5);
	if (sf2 < 5) return;
	std::wstring s = double_to_string(a, 2);

	master_bm.text((int)(oo.x.min + 5), (int)(oo.y.min + 1), s.c_str(), sf2, c0, 0xff000000);
	if (n_act_key == this)
	{
		_size2i size = master_bm.size_text(s.substr(/*first_+*/ 0, cursor /*-first_*/).c_str(), sf2);
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
	local_area = { {0, 100}, {0, 16} };
	key_fokus = true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void _g_edit_int::ris2(_trans tr, bool final)
{
	_area2 oo = tr(local_area);
	uint c0 = get_c();
	master_bm.rectangle(oo, c0);
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
		_size2i size = master_bm.size_text(s.substr(/*first_+*/ 0, cursor /*-first_*/).c_str(), sf2);
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

bool _g_edit_int::mouse_wheel2(_coo2 r)
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
	_area2 oo = tr(local_area);
	uint c0 = get_c();
	master_bm.rectangle(oo, c0);
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
		_size2i size = master_bm.size_text(s->substr(first, len2).c_str(), sf);
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
		_size2i size = master_bm.size_text(s->substr(first, (int64)cursor - first).c_str(), sf);
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
	local_area = { {0, 100}, {0, 16} };
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void _g_edit_multi_string::ris2(_trans tr, bool final)
{
	_area2 oo = tr(local_area);
	if ((oo.x.length() <= 3) || (oo.y.length() <= 3)) return;
	_multi_string* str = find1_plus_gtetron<_multi_string>(this, flag_specialty);
	if (!str) str = &strings;
	master_bm.rectangle((_area2)oo, c_def);
	if (str != &strings)
	{
		_area2 ooo = oo;
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
	int64 k = len2;
	if ((int)str->line.size() < first + k) k = (int64)str->line.size() - first;
	for (int64 i = 0; i < k; i++)
	{
		int64 ii = first + i;
		master_bm.text((int)(oo.x.min + 4 * tr.scale), (int)(oo.y.min + (i * 16LL + 4) * tr.scale),
			str->line[ii].c_str(), sf, c_def, 0xff000000);
	}
	if (first) master_bm.line({ (int)(oo.x.min + 2 * tr.scale), (int)(oo.y.min + 2 * tr.scale) },
		{ (int)(oo.x.max - 2 * tr.scale), (int)(oo.y.min + 2 * tr.scale) }, 0x30C0F0);
	if (first + k < (int)str->line.size()) master_bm.line({ (int)(oo.x.min + 2 * tr.scale), (int)(oo.y.max - 2 * tr.scale) },
		{ (int)(oo.x.max - 2 * tr.scale), (int)(oo.y.max - 2 * tr.scale) }, 0x30C0F0);

	if (n_act_key == this)
	{
		_size2i size = master_bm.size_text(str->line[cursor.y].substr(0, cursor.x).c_str(), sf);
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
	local_area = { {0, 100}, {0, 100} };
	key_fokus = true;
}

void _g_edit_multi_string::run(_tetron* tt0, _tetron* tt, uint64 flags)
{
	cha_area();
}

bool _g_edit_multi_string::mouse_down_left2(_coo2 r)
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

bool _g_edit64bit::mouse_down_left2(_coo2 r)
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

bool _g_edit64bit::mouse_move2(_coo2 r)
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
	_area2  oo = tr(local_area);
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
	local_area = { {0, 58}, {0, 58} };
	a = 0;
	act = -1;
}

void _g_edit64bit::run(_tetron* tt0, _tetron* tt, uint64 flags) { cha_area(); }

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool _g_edit_one::mouse_down_left2(_coo2 r)
{
	_area2 aa = local_area;
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
	_area2 bb = tr(local_area);
	_area2 oo = bb;
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
			_size2i size = master_bm.size_text(s.substr(/*first_+*/ 0, cursor /*-first_*/).c_str(), sf2);
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
			_size2i size = master_bm.size_text(s.substr(/*first_+*/ 0, cursor /*-first_*/).c_str(), sf2);
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
	local_area = { {0, 100}, {0, 30} };
	key_fokus = true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void _g_button::RisIco(astr kod, const char* s)
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
	for (astr i = s; *i; i++)
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
	astr ss = s;
	int y = y0;
	for (astr i = s; *i; i++)
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
	_area2 oo = tr(local_area);
	uint c = 0;
	if (picture.size.x * picture.size.y > 0)
	{
		int64 rx2 = (int64)(picture.size.x * tr.scale + 0.5);
		int64 ry2 = (int64)(picture.size.y * tr.scale + 0.5);
		_num2 ce = oo.center();
		master_bm.stretch_draw3(&picture, ce.x - rx2 / 2, ce.y - ry2 / 2, tr.scale);
	}
	else
		c = c_def;
	if (checked) c = c_def - 0x40000000;
	if (n_go_move == this) c = c_def - 0x80000000;
	if (n_tani == this) c = c_max - 0x80000000;
	master_bm.fill_rectangle({ {(int64)oo.x.min, (int64)oo.x.max + 1}, {(int64)oo.y.min, (int64)oo.y.max + 1} }, c);
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
	local_area = { {0, 26}, {0, 26} };
}

bool _g_button::mouse_move2(_coo2 r)
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

bool _g_button::mouse_down_left2(_coo2 r)
{
	if (test_flags(n_checkbox, flag_parent)) checked = !checked;
	run(this, this, flag_run);
	cha_area();
	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void _g_color_ring::ris2(_trans tr, bool final)
{
	const double toll = 2.0;
	_area2 a = tr(local_area);
	double r = a.radius();
	_coo2 c = a.center();
	master_bm.ring(a.center(), r, r * 0.04, c_def);
	master_bm.fill_circle(c.x, c.y, r * 0.3, color);
	double r1 = 0.4 * r;
	double r2 = 0.5 * r;
	double r1_ = li_r_1 * r; // для ползунка
	double r2_ = li_r_2 * r; // для ползунка
	double dk = 1.0 / ((pi * 0.5 - zazor) * r2);
	for (double k = 0; k <= 1; k += dk)
	{
		double alpha = zazor * 0.5 + (pi * 0.5) * 0 + (pi * 0.5 - zazor) * k;
		_hsva cv = color;
		cv.h = k * 360;
		master_bm.lines(c + _coo2{ r1 * cos(alpha), -r1 * sin(alpha) }, c + _coo2{ r2 * cos(alpha), -r2 * sin(alpha) }, toll, cv);
	}
	for (double k = 0; k <= 1; k += dk)
	{
		double alpha = zazor * 0.5 + (pi * 0.5) * 1 + (pi * 0.5 - zazor) * k;
		_hsva cv = color;
		cv.s = k;
		master_bm.lines(c + _coo2{ r1 * cos(alpha), -r1 * sin(alpha) }, c + _coo2{ r2 * cos(alpha), -r2 * sin(alpha) }, toll, cv);
	}
	for (double k = 0; k <= 1; k += dk)
	{
		double alpha = zazor * 0.5 + (pi * 0.5) * 2 + (pi * 0.5 - zazor) * k;
		_hsva cv = color;
		cv.v = k;
		master_bm.lines(c + _coo2{ r1 * cos(alpha), -r1 * sin(alpha) }, c + _coo2{ r2 * cos(alpha), -r2 * sin(alpha) }, toll, cv);
	}
	for (double k = 0; k <= 1; k += dk)
	{
		double alpha = zazor * 0.5 + (pi * 0.5) * 3 + (pi * 0.5 - zazor) * k;
		_hsva cv = color;
		cv.a = k;
		master_bm.lines(c + _coo2{ r1 * cos(alpha), -r1 * sin(alpha) }, c + _coo2{ r2 * cos(alpha), -r2 * sin(alpha) }, toll, cv);
	}
	double k = color.h / 360.0;
	double alpha = zazor * 0.5 + (pi * 0.5) * 0 + (pi * 0.5 - zazor) * k;
	master_bm.lines(c + _coo2{ r1_ * cos(alpha), -r1_ * sin(alpha) }, c + _coo2{ r2_ * cos(alpha), -r2_ * sin(alpha) }, 1.2, 0xFFFFFFFF);
	k = color.s;
	alpha = zazor * 0.5 + (pi * 0.5) * 1 + (pi * 0.5 - zazor) * k;
	master_bm.lines(c + _coo2{ r1_ * cos(alpha), -r1_ * sin(alpha) }, c + _coo2{ r2_ * cos(alpha), -r2_ * sin(alpha) }, 1.2, 0xFFFFFFFF);
	k = color.v;
	alpha = zazor * 0.5 + (pi * 0.5) * 2 + (pi * 0.5 - zazor) * k;
	master_bm.lines(c + _coo2{ r1_ * cos(alpha), -r1_ * sin(alpha) }, c + _coo2{ r2_ * cos(alpha), -r2_ * sin(alpha) }, 1.2, 0xFFFFFFFF);
	k = color.a;
	alpha = zazor * 0.5 + (pi * 0.5) * 3 + (pi * 0.5 - zazor) * k;
	master_bm.lines(c + _coo2{ r1_ * cos(alpha), -r1_ * sin(alpha) }, c + _coo2{ r2_ * cos(alpha), -r2_ * sin(alpha) }, 1.2, 0xFFFFFFFF);
}

void _g_color_ring::change(_coo2 p, bool start)
{
	double r = local_area.radius();
	_coo2 c = local_area.center();
	_coo2 v = p - c;
	double rr = v.len() / r;
	if ((rr > li_r_1) && (rr < li_r_2))
	{
		double alpha;
		int i;
		if (start)
		{
			alpha = atan2(-v.y, v.x);
			if (alpha < 0) alpha += 2 * pi;
			i = (int)(alpha / (pi / 2));
			iii = i;
		}
		else
		{
			alpha = atan2(-v.y, v.x);
			i = iii;
			if (((alpha < 0) && (i != 0)) || (i == 3)) alpha += 2 * pi;
		}
		alpha -= (pi / 2) * i;
		double k = (alpha - (zazor * 0.5)) / (pi / 2 - zazor);
		if (k < 0) k = 0;
		if (k > 1) k = 1;
		switch (i)
		{
		case 0: color.h = 360 * k; break;
		case 1: color.s = k; break;
		case 2: color.v = k; break;
		case 3: color.a = k; break;
		}
		cha_area();
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool _g_list_link::mouse_down_left2(_coo2 r)
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
	_area2 oo = tr(local_area);
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

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

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
	_area2 oo2 = tr(oo);
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
				a->operator std::wstring * ()->c_str(), sf, c, 0);
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
				a->operator std::wstring * ()->c_str(), sf, c, 0);
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
	oo = { {0, 100}, {0, 16} };
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

bool _g1list::mouse_move2(_coo2 r)
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

bool _g1list::mouse_wheel2(_coo2 r)
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

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool _g_tetron::mouse_move2(_coo2 r)
{
	if (n_hint) return true; // есть подсказка
	_tetron* a = find1<_tetron>(flag_specialty);
	std::wstring re;
	if (a) re = string_to_wstring(a->name()) + L" " + uint64_to_wstr_hex(a->id);
	if (hint.size())
	{
		if (re.size()) re += L": ";
		re += hint;
	}
	if (re.size()) add_hint(re, this);
	return true;
}

void _g_tetron::mouse_finish_move()
{
	del_hint();
}

bool _g_tetron::cmp_drawn()
{
	if (master_chain_go.chain.size() > drawn_create_star.size()) return false;
	for (uint i = 0; i < master_chain_go.chain.size(); i++)
		if (master_chain_go.chain[i] != drawn_create_star[i])
			return false;
	return true;
}

bool _g_tetron::test_local_area(_coo2 b)
{
	if (!local_area.test(b)) return false;
	_coo2 pp = local_area.center();
	double r = std::min(pp.x - local_area.x.min, pp.y - local_area.y.min);
	return ((b - pp).len2() <= r * r);
}

void _g_tetron::ris2(_trans tr, bool final)
{
	_area2 a = tr(local_area);
	_coo2 p = a.center();
	double r = a.radius();
	double d = log(1.001 + r) * 0.5;
	double y = 0.7 * (r - d) * 2;
	uint c = c_min;

	_tetron* t2 = find1<_tetron>(flag_specialty); // связанный тетрон
	bool im_temp = test_flags(n_temp_go, flag_parent); // текущий _g_tetron - временный
	// выделение красным цветом
	static _tetron* root_tetron = nullptr;
	if (!im_temp)
		root_tetron = t2;
	else
		if (t2 == root_tetron) c = 0xFFFF0000;

	master_bm.fill_ring(p, r, d, c, (im_temp) ? 0 : c_background);

	_t_trans* ttr = nullptr; // звезда
	if (star) ttr = *star;

	if (t2 == nullptr) // нет связанного тетрона
	{
		if (star) delete_hvost(star, true, false);
		return;
	}

	if (final) // нужно заканчивать
	{
		if (star)
			if (cmp_drawn())
				delete_hvost(star, true, false);
		int fr = t2->get_froglif();
		master_bm.froglif(p - _coo2{ y / 2, y / 2 }, y, (uchar*)& fr, 2, c_max);
		return;
	}

	if (star) return;

	_g_tetron* gg2;
	// ищет существующий временный _g_tetron
	_g_tetron* aa = t2->find_intermediate<_g_tetron>(n_temp_go, inverted_flags(flag_specialty), flag_parent);
	if (aa)
		if (aa != this)
		{ // может уже есть звезда?
			gg2 = aa; // не this
			ttr = nullptr; // подходящая звезда для использования
			if (gg2->star) ttr = *gg2->star;
			if (ttr)
			{
				drawn_create_star = master_chain_go.chain;
				_t_trans* ttr3 = new _t_trans;
				ttr3->add_flags(n_temp_go, flag_parent);
				ttr3->trans.scale = std::min(local_area.x.length(), local_area.y.length()) * 0.5;
				ttr3->trans.offset = { local_area.x(0.5), local_area.y(0.5) };
				star = ttr3;
				ttr3->add_flags(ttr, flag_part + flag_sub_go, false);
				add_flags(ttr3, flag_part + flag_sub_go, false);
				return;
			}
		}
	drawn_create_star = master_chain_go.chain;
	constexpr double kr = 0.333; // 0.4 растояние между тетронами = kr * radius
	int vl = 0;
	for (auto& i : t2->link)
		if (!i->pairr(t2)->test_flags(n_temp_go, flag_parent)) vl++;
	int vl2 = vl;
	int orbit = 0;
	while (vl2 > 0)
	{
		orbit++;
		vl2 -= (int)(pi * orbit * (2 + kr));
	}
	if (orbit == 0) orbit = 1;
	double radius = 0.98 / (orbit * (2.0 + kr) + 1.0); // 0.95
	ttr = new _t_trans;
	ttr->add_flags(n_temp_go, flag_parent);
	ttr->trans.scale = radius;
	_t_trans* ttr2 = new _t_trans;
	ttr2->add_flags(n_temp_go, flag_parent);
	ttr->add_flags(ttr2, flag_part + flag_sub_go, false);

	if (im_temp)
		star = ttr;
	else
	{
		_t_trans* ttr3 = new _t_trans;
		ttr3->add_flags(n_temp_go, flag_parent);
		ttr3->trans.scale = std::min(local_area.x.length(), local_area.y.length()) * 0.5;
		ttr3->trans.offset = { local_area.x(0.5), local_area.y(0.5) };
		star = ttr3;
		ttr3->add_flags(ttr, flag_part + flag_sub_go, false);
	}
	add_flags(star, flag_part + flag_sub_go, false);

	if (aa)
		gg2 = aa;
	else
	{
		gg2 = new _g_tetron;
		gg2->add_flags(n_temp_go, flag_parent);
		gg2->add_flags(t2, flag_specialty, false);
		gg2->local_area = { {-1.0, 1.0}, {-1.0, 1.0} };
		gg2->star = ttr;
		gg2->add_flags(ttr, flag_part + flag_sub_go, false);
	}

	ttr2->add_flags(gg2, flag_part + flag_sub_go, false);

	int uze_nar = 0;
	uint ilink = 0;
	for (int n_orb = 1; n_orb <= orbit; n_orb++)
	{
		int nn = (int)(pi * n_orb * (2 + kr));
		if (nn > vl - uze_nar) nn = vl - uze_nar;
		int inn = 0;
		for (; (ilink < t2->link.size()) && (inn < nn); ilink++)
		{
			_tetron* t = t2->link[ilink]->pairr(t2);
			if (t->test_flags(n_temp_go, flag_parent)) continue;
			aa = t->find_intermediate<_g_tetron>(n_temp_go, inverted_flags(flag_specialty), flag_parent);
			_g_tetron* gg;
			if (aa)
				gg = aa;
			else
			{
				gg = new _g_tetron;
				gg->add_flags(n_temp_go, flag_parent);
				gg->add_flags(t, flag_specialty, false);
				gg->local_area = { {-1.0, 1.0}, {-1.0, 1.0} };
			}
			ttr2 = new _t_trans;
			ttr2->add_flags(n_temp_go, flag_parent);
			double rad = n_orb * (2.0 + kr);
			ttr2->trans.offset.x = rad * cos(2.0 * pi * inn / nn);
			ttr2->trans.offset.y = -rad * sin(2.0 * pi * inn / nn);
			ttr2->add_flags(gg, flag_part + flag_sub_go, false);
			ttr->add_flags(ttr2, flag_part + flag_sub_go, false);

			_g_link* gl = new _g_link;
			gl->add_flags(n_temp_go, flag_parent);
			gl->add_flags(gg, flag_specialty, false);
			gl->add_flags(gg2, flag_specialty2, false);
			_t_trans* ttr3 = new _t_trans;
			ttr3->set_layer(-1.0);
			ttr3->add_flags(n_temp_go, flag_parent);
			ttr3->add_flags(gl, flag_part + flag_sub_go, false);
			ttr->add_flags(ttr3, flag_part + flag_sub_go, false);
			gl->k = kr / (n_orb * (2 + kr) - 2); // 0==0 k=1, 0= 0 k=0.5
			gl->calc_local_area();
			gl->area_definite = false;
			inn++;
		}
		uze_nar += nn;
	}
	return;
}

void _g_tetron::add_unique_flags(_tetron* t, uint64 flags, bool after)
{
	if (star) delete_hvost(star, true, false);
	_tetron::add_unique_flags(t, flags, after);
	//	cha_area();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void _g_tetron2::ris2(_trans tr, bool final)
{
	_area2 a = tr(local_area);
	_coo2 p = a.center();
	double r = a.radius();
	double d = log(1.001 + r) * 0.5;
	double y = 0.7 * (r - d) * 2;
	uint c = c_min;
	master_bm.fill_ring(p, r, d, c, c_background);
	_tetron* t2 = find1<_tetron>(flag_specialty); // связанный тетрон
	if (final || (t2 == nullptr))
	{
		mo.clear();
		return;
	}

	if (mo.empty())
	{
		SuperDelTetron2::PodgSpUnikSv(t2);
		SuperDelTetron2::ud->push_back(t2); // т.к. в списке главного тетрона нет

		for (auto i : *SuperDelTetron2::ud)
		{
			_molecule m;
			m.tetron = i;
			mo.push_back(m);
		}
		/*		for (auto i : *SuperDelTetron2::ud)
				{
					molecule m;
					m.c = C_MIN;
					for (int j = 0; j < i->vlink_; j++)
					{
						Tetron* a = i->link_[j].tetron_;
						if (!a) continue;
						m.tetron = a;
						m.p.x = (prng.Random(2001) * 0.001 - 1.0);
						m.p.y = (prng.Random(2001) * 0.001 - 1.0);
						mo.insert(m);
					}
				}*/
		SuperDelTetron2::FreeBank();
		int k = (int)mo.size();
		int ki = 0;
		for (auto& i : mo)
		{
			_t_trans* ttr = new _t_trans;
			i.tr_t = ttr;
			ttr->trans.offset = local_area.center() + _coo2{ 10 * cos(ki * pi * 2 / k), 10 * sin(ki * pi * 2 / k) };
			ttr->trans.scale = 0.2;
			_g_tetron* g = new _g_tetron;
			g->add_flags(i.tetron, flag_specialty);
			ttr->add_flags(g, flag_part | flag_sub_go);
			add_flags(ttr, flag_part | flag_sub_go);
			ki++;
		}
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void _g_link::ris2(_trans tr, bool final)
{
	_coo2 pp11 = tr(p11);
	_coo2 pp12 = tr(p12);
	_coo2 pp21 = tr(p21);
	_coo2 pp22 = tr(p22);

	_g_tetron* g1 = find1<_g_tetron>(flag_specialty);
	_g_tetron* g2 = find1<_g_tetron>(flag_specialty2);
	_tetron* t1 = g1->find1<_tetron>(flag_specialty);
	_tetron* t2 = g2->find1<_tetron>(flag_specialty);
	uint64 f = t1->get_flags(t2);

	constexpr uint cc[32] = { 0xFF0080FD, 0xFFEF0000, 0xFF9E3BFF, 0xFF938700, 0xFF12AA00, 0xFFD34E0D, 0xFF7470DC,
		0xFF4D9682, 0xFFC80FCE, 0xFFB06381,	0xFFE22653, 0xFF009D93, 0xFFC4439D, 0xFF258ECB, 0xFF659B00, 0xFF3F77FF,
		0xFF887E87, 0xFF9659DD, 0xFFB57000, 0xFF39A337, 0xFF5D84BE, 0xFFAF1FFB, 0xFFDE028A, 0xFF768F51, 0xFFB400FC,
		0xFFE23700, 0xFFCB29B2, 0xFFA3755A, 0xFFC45D37, 0xFFEB171B, 0xFF966DA8, 0xFFAC4ACF };

	_coo2 v = p21 - p11;
	v /= v.len();
	double b = (pp12 - pp11).len() * 0.5;
	double r = b / sin(dalpha);
	int al = act_li;
	if (tr != master_trans_go_move) al = -1;
	for (int i = 0; i < v_link; i++)
	{
		double bb = b * (v_link - 0.5 - i) / v_link;
		double dl = sqrt(r * r - bb * bb) - r * cos(dalpha);
		double dl2 = (k > 0.9) ? dl : 0;
		uint c = (f & (1ULL << i)) ? cc[i] : cclow(cc[i]);
		double t = 1;
		if (al == i)
		{
			t = 2;
			c = brighten(c);
		}
		if ((al != -1) || (f & (1ULL << i)))
			master_bm.lines(pp11 + (pp12 - pp11) * ((i + 0.5) / (v_link * 2)) + v * dl,
				pp21 + (pp22 - pp21) * ((i + 0.5) / (v_link * 2)) - v * dl2, t, c);
		c = (f & (1ULL << (i + 32))) ? cc[i] : cclow(cc[i]);
		t = 1;
		if (al == (v_link * 2 - 1 - i))
		{
			t = 2;
			c = brighten(c);
		}
		if ((al != -1) || (f & (1ULL << (i + 32))))
			master_bm.lines(pp11 + (pp12 - pp11) * ((v_link * 2 - 0.5 - i) / (v_link * 2)) + v * dl,
				pp21 + (pp22 - pp21) * ((v_link * 2 - 0.5 - i) / (v_link * 2)) - v * dl2, t, c);
	}
}

bool _g_link::test_local_area(_coo2 b)
{
	if (!local_area.test(b)) return false;
	if (((test_line(p11, p12, b) + test_line(p21, p22, b) + test_line(p11, p21, b) + test_line(p12, p22, b)) & 1) == 0) return false;
	_coo2 v = p21 - p11;
	v /= v.len();
	_coo2 c1 = (p11 + p12) * 0.5;
	double d = (p12 - p11).len() * 0.5;
	double ot = d / tan(dalpha);
	double r = d / sin(dalpha);
	_coo2 p1 = c1 - v * ot;
	if ((b - p1).len2() < r * r) return false;
	if (k > 0.9)
	{
		c1 = (p21 + p22) * 0.5;
		p1 = c1 + v * ot;
		if ((b - p1).len2() < r * r) return false;
	}
	return true;
}

void _g_link::calc_local_area()
{
	local_area.clear();
	_g_tetron* g1 = find1<_g_tetron>(flag_specialty);
	_g_tetron* g2 = find1<_g_tetron>(flag_specialty2);
	if ((g1 == nullptr) || (g2 == nullptr)) return;
	_t_trans* par = find1<_t_trans>(flag_sub_go << 32);
	if (par == nullptr) return;
	_t_basic_go* parpar = par->find1<_t_basic_go>(flag_sub_go << 32);
	if (parpar == nullptr) return;
	_t_trans* tr1 = parpar->find_intermediate<_t_trans>(g1, flag_sub_go, flag_sub_go);
	_t_trans* tr2 = parpar->find_intermediate<_t_trans>(g2, flag_sub_go, flag_sub_go);
	if ((tr1 == nullptr) || (tr2 == nullptr)) return;
	_area2 a1 = tr1->trans(g1->local_area);
	_area2 a2 = tr2->trans(g2->local_area);
	par->trans = _trans();
	_coo2 p1 = a1.center();
	double r1 = a1.radius();
	_coo2 p2 = a2.center();
	double r2 = a2.radius();
	_coo2 d = p2 - p1;
	if ((d.y == 0) && (d.x == 0)) return;
	double alpha = atan2(d.y, d.x);
	double dv0 = d.len();
	double dv = (dv0 - (r1 + r2) * cos(dalpha)) * k;
	if (k < 0.9)
		dv = (dv0 - (r1 + r2)) * k + r1 * (1 - cos(dalpha));
	double kk = dv / dv0;
	local_area += p11 = _coo2{ p1.x + r1 * cos(alpha - dalpha), p1.y + r1 * sin(alpha - dalpha) };
	local_area += p12 = _coo2{ p1.x + r1 * cos(alpha + dalpha), p1.y + r1 * sin(alpha + dalpha) };
	p21 = { p11.x + d.x * kk, p11.y + d.y * kk };
	p22 = { p12.x + d.x * kk, p12.y + d.y * kk };
	local_area += p21;
	local_area += p22;
}

bool _g_link::mouse_down_left2(_coo2 r)
{
	if (act_li < 0) return true;

	_g_tetron* g1 = find1<_g_tetron>(flag_specialty);
	_g_tetron* g2 = find1<_g_tetron>(flag_specialty2);
	_tetron* t1 = g1->find1<_tetron>(flag_specialty);
	_tetron* t2 = g2->find1<_tetron>(flag_specialty);
	uint64 f;
	if (act_li < v_link)
		f = (1ULL << act_li);
	else
		f = (1ULL << (32 + 2 * v_link - 1 - act_li));
	t1->xor_flags(t2, f);
	cha_area(master_trans_go);
	return true;
}

bool _g_link::mouse_move2(_coo2 r)
{
	static constexpr std::wstring_view nh[v_link] = { L"parent", L"part", L"run", L"???", L"sub_go", L"information",
		L"information2", L"specialty", L"specialty2" };
	_coo2 v = p12 - p11;
	double b = v.len();
	double a = (r - p11).scalar(v) / b;
	double k = a / b; // 0..1
	int ii = act_li;
	act_li = (int)(k * v_link * 2);
	if (act_li < 0) act_li = 0;
	if (act_li >= v_link * 2) act_li = act_li = v_link * 2 - 1;

	if (ii != act_li)
	{
		change_hint((act_li < v_link) ? nh[act_li] : nh[2 * v_link - 1 - act_li]);
		cha_area(master_trans_go);
	}
	if (!n_hint) add_hint((act_li < v_link) ? nh[act_li] : nh[2 * v_link - 1 - act_li], this);
	return true;
}

void _g_link::mouse_finish_move()
{
	act_li = -1;
	cha_area(master_trans_go_move);
	del_hint();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
