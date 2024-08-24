#include "e_terminal.h"
#include "win_basic.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void trm_test(_e_terminal& trm, const std::vector<std::wstring>& parameters)
{
	trm.start_timer();
	trm.stop_timer(std::to_wstring(8));
	trm.print(L"====");
}

void trm_clear(_e_terminal& trm, const std::vector<std::wstring>& parameters)
{
	trm.text_clear();
}

void trm_help(_e_terminal& trm, const std::vector<std::wstring>& parameters)
{
	for (auto& i : trm.command) trm.print(i.first + L" - " + i.second.caption);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

_e_terminal::_e_terminal(_ui* ui_) : _ui_element(ui_)
{
	local_area = { {0, 100}, {0, 100} };
	key_fokus = true;

	command.insert({ L"clear", { L"очищение экрана", trm_clear } });
	command.insert({ L"help", { L"вывод справки", trm_help } });
	command.insert({ L"test", { L"тестирование разное", trm_test } });
}

void _e_terminal::ris2(_trans tr)
{
	std::lock_guard<std::mutex> lck(mtx);
	std::wstring old_font = ui->canvas.get_font_name();
	ui->canvas.set_font(L"Consolas", false);
	if (font_width == 0) font_width = ui->canvas.size_text("0123456789", font_size).x / 10;

	std::wstring full_cmd = prefix + cmd;

	_iarea oo = tr(local_area);
	_iarea oo2 = oo;
	oo2.x.max -= width_scrollbar - 1;

	i64 x_text = oo.x.min + otst_x;
	i64 y_cmd = oo.y.max - font_size - otst_y;
	cmd_vis_len = (oo.x.length() - otst_x * 2 - width_scrollbar) / font_width;
	if (cmd_vis_len <= 0) cmd_vis_len = 1;
	i64 ks = (full_cmd.size() + cmd_vis_len) / cmd_vis_len;

	max_lines = (oo.y.length() - otst_y * 2) / font_size; // строк в окне

	full_lines = 0; // общее количество строк
	if (old_cmd_vis_len == cmd_vis_len)
		full_lines = old_full_lines;
	else
	{
		full_lines = ks;
		for (auto s = text.rbegin(); s != text.rend(); s++)
			if (s->empty())
				full_lines++;
			else
				full_lines += (s->size() + cmd_vis_len - 1) / cmd_vis_len;
		old_cmd_vis_len = cmd_vis_len;
		old_full_lines = full_lines;
	}

	i64 length_slider = max_lines * (oo.y.length() - otst_y * 2) / full_lines;
	if (length_slider > oo.y.length() - otst_y * 2) length_slider = oo.y.length() - otst_y * 2;
	if (length_slider < 10) length_slider = 10;

	if (scrollbar > full_lines - max_lines) scrollbar = full_lines - max_lines;
	if (scrollbar < 0) scrollbar = 0;

	i64 x_cur = (cursor + (i64)prefix.size()) % cmd_vis_len;
	i64 y_cur = (cursor + (i64)prefix.size()) / cmd_vis_len;
	i64 n_cur = ks - 1 - y_cur - scrollbar;
	area_cursor = { {x_text + x_cur * font_width, x_text + (x_cur + 1) * font_width},
		{y_cmd - n_cur * font_size, y_cmd - (n_cur - 1) * font_size} };
	if ((n_cur < 0) || (n_cur >= max_lines))
	{
		if (!vis_cur)
			area_cursor = _iarea();
		else
		{
			scrollbar = ks - 1 - y_cur;
			if (scrollbar > full_lines - max_lines) scrollbar = full_lines - max_lines;
			if (scrollbar < 0) scrollbar = 0;
			n_cur = ks - 1 - y_cur - scrollbar;
			area_cursor = { {x_text + x_cur * font_width, x_text + (x_cur + 1) * font_width},
				{y_cmd - n_cur * font_size, y_cmd - (n_cur - 1) * font_size} };
		}
	}
	vis_cur = false;
	if (_area(area_cursor) == ui->changed_area) // перерисовать только курсор
	{
		if (area_cursor.empty()) goto finish; // перестраховка
		ui->canvas.text({ area_cursor.x.min, area_cursor.y.min }, cmd.substr(cursor, 1), font_size, ui->cc2, ui->cc0);
		if (visible_cursor) ui->canvas.fill_rectangle(area_cursor, { ui->cc3 - 0xC0000000 });
		goto finish;
	}

	ui->canvas.fill_rectangle(oo2, c2);
	if ((c.a != 0x00) && (c != c2)) ui->canvas.rectangle(oo2, c);
	if ((oo2.y.length() < 30) || (oo2.x.length() < 30)) goto finish;

	if (full_lines > max_lines)
	{ // ползунок
		ui->canvas.line({ oo.x.max - 1, oo.y.min }, { oo.x.max - 1, oo.y.max }, c);
		ui->canvas.line({ oo2.x.max, oo.y.min }, { oo.x.max - 2, oo.y.min }, c);
		ui->canvas.line({ oo2.x.max, oo.y.max - 1 }, { oo.x.max - 2, oo.y.max - 1 }, c);

		i64 tt = (oo.y.length() - otst_y * 2 - length_slider) * scrollbar / (full_lines - max_lines);

		y_slider = { oo.y.max - otst_y - tt - length_slider, oo.y.max - otst_y - tt };
		ui->canvas.fill_rectangle(_iarea{ {oo.x.max - width_scrollbar + 2, oo.x.max - 2}, y_slider }, { c });
	}

	if (ui->n_act_key.get() == this)
		ui->n_timer1000.insert(shared_from_this());
	else
		ui->n_timer1000.erase(shared_from_this());

	for (i64 i = 0; i < ks; i++)
	{
		i64 n = ks - 1 - i - scrollbar;
		if (n < 0) break;
		if (n >= max_lines) continue;
		ui->canvas.text({ x_text, y_cmd - n * font_size }, full_cmd.substr(i * cmd_vis_len, cmd_vis_len),
			font_size, ui->cc2, ui->cc0);
	}

	if (visible_cursor) ui->canvas.fill_rectangle(area_cursor, { ui->cc3 - 0xC0000000 });

	for (i64 i = text.size() - 1; i >= 0; i--)
	{
		std::wstring& s = text[i];
		i64 ks2 = (s.size() + cmd_vis_len - 1) / cmd_vis_len;
		if (s.empty()) ks2 = 1;

		ks += ks2;

		for (i64 j = 0; j < ks2; j++)
		{
			i64 n = ks - 1 - j - scrollbar;
			if (n < 0) break;
			if (n >= max_lines) continue;
			ui->canvas.text({ x_text, y_cmd - n * font_size }, s.substr(j * cmd_vis_len, cmd_vis_len),
				font_size, ui->cc1, ui->cc0);
		}
		if (ks - scrollbar > max_lines) break;
	}

	if (selection_begin.x >= 0)
	{
		if (selection_begin.y == selection_end.y)
		{
			i64 yy = selection_begin.y - scrollbar;
			if ((yy >= 0) && (yy < max_lines))
			{
				i64 x0 = std::min(selection_begin.x, selection_end.x);
				i64 x1 = std::max(selection_begin.x, selection_end.x) + 1;
				_iarea a = { {x_text + x0 * font_width, x_text + x1 * font_width},
					{y_cmd - yy * font_size, y_cmd - (yy - 1) * font_size} };
				ui->canvas.fill_rectangle(a, { ui->cc3 - 0xA0000000 });
			}
		}
		else
		{
			i64 x0, y0, x1, y1;
			if (selection_begin.y < selection_end.y)
			{
				x0 = selection_begin.x;
				y0 = selection_begin.y - scrollbar;
				x1 = selection_end.x;
				y1 = selection_end.y - scrollbar;
			}
			else
			{
				x1 = selection_begin.x;
				y1 = selection_begin.y - scrollbar;
				x0 = selection_end.x;
				y0 = selection_end.y - scrollbar;
			}
			if ((y1 >= 0) && (y1 < max_lines))
			{
				_iarea a = { {x_text + x1 * font_width, x_text + cmd_vis_len * font_width},
				{y_cmd - y1 * font_size, y_cmd - (y1 - 1) * font_size} };
				ui->canvas.fill_rectangle(a, { ui->cc3 - 0xA0000000 });
			}
			for (i64 yy = y1 - 1; yy > y0; yy--)
				if ((yy >= 0) && (yy < max_lines))
				{
					_iarea a = { {x_text, x_text + cmd_vis_len * font_width},
					{y_cmd - yy * font_size, y_cmd - (yy - 1) * font_size} };
					ui->canvas.fill_rectangle(a, { ui->cc3 - 0xA0000000 });
				}

			if ((y0 >= 0) && (y0 < max_lines))
			{
				_iarea a = { {x_text, x_text + (x0 + 1) * font_width},
				{y_cmd - y0 * font_size, y_cmd - (y0 - 1) * font_size} };
				ui->canvas.fill_rectangle(a, { ui->cc3 - 0xA0000000 });
			}
		}
	}

finish:
	ui->canvas.set_font(old_font.c_str(), false);
}

void _e_terminal::update()
{
	std::lock_guard<std::mutex> lck(mtx);
	if (need_to_update)
	{
		need_to_update = false;
		visible_cursor = true;
		cha_area();
		return;
	}
	ui->add_changed_area(area_cursor);
	visible_cursor = !visible_cursor;
}

void _e_terminal::key_down(u64 key)
{
	visible_cursor = true;
	switch (key)
	{
	case 8: // backspace
		if (cursor > 0)
		{
			cmd.erase(cursor - 1LL, 1);
			cursor--;
			old_cmd_vis_len = -1;
			selection_begin.x = -1;
			vis_cur = true;
		}
		break;
	case 13: // enter
		cursor = 0;
		run_cmd();
		vis_cur = true;
		break;
	case 35: // end
		cursor = (i64)cmd.size();
		vis_cur = true;
		break;
	case 36: // home
		cursor = 0;
		vis_cur = true;
		break;
	case 37: // left
		if (cursor > 0)	cursor--;
		vis_cur = true;
		break;
	case 38: // стрелка вверх
		if (act_previous_cmd > 0) act_previous_cmd--;
		if (act_previous_cmd < (i64)previous_cmd.size()) cmd = previous_cmd[act_previous_cmd];
		cursor = 0;
		old_cmd_vis_len = -1;
		selection_begin.x = -1;
		vis_cur = true;
		break;
	case 39: // right
		if (cursor < (i64)cmd.size()) cursor++;
		vis_cur = true;
		break;
	case 40: // стрелка вниз
		if (act_previous_cmd < (i64)previous_cmd.size() - 1) act_previous_cmd++;
		if (act_previous_cmd < (i64)previous_cmd.size()) cmd = previous_cmd[act_previous_cmd];
		cursor = 0;
		old_cmd_vis_len = -1;
		selection_begin.x = -1;
		vis_cur = true;
		break;
	case 45: // insert
		insert_mode = !insert_mode;
		break;
	case 46: // delete
		if (cursor < (i64)cmd.size())
		{
			cmd.erase(cursor, 1);
			old_cmd_vis_len = -1;
			selection_begin.x = -1;
			vis_cur = true;
		}
		break;
	case 67: // c
		if (keyboard.ctrl_key) set_clipboard();
		break;
	}
	cha_area();
}

void _e_terminal::set_clipboard()
{
	if (selection_begin.x < 0) return;
	std::wstring result;
	i64 x0, y0, x1, y1;
	if (selection_begin.y < selection_end.y)
	{
		x0 = selection_begin.x;
		y0 = selection_begin.y;
		x1 = selection_end.x;
		y1 = selection_end.y;
	}
	else
	{
		x1 = selection_begin.x;
		y1 = selection_begin.y;
		x0 = selection_end.x;
		y0 = selection_end.y;
	}
	if (y0 == y1)
	{
		if (x1 < x0) std::swap(x0, x1);
	}
	i64 yy = full_lines;

	auto fff = [&](const std::wstring& s)
	{
		i64 ks = (s.size() + cmd_vis_len - 1) / cmd_vis_len;
		if (ks == 0) ks = 1;
		if (yy - 1 < y0) return;
		if (yy - ks > y1)
		{
			yy -= ks;
			return;
		}
		if ((yy - 1 < y1) && (yy - ks > y0))
		{
			result += s + L"\r\n";
			yy -= ks;
			return;
		}
		for (i64 i = 0; i < ks; i++)
		{
			yy--;
			if ((yy > y0) && (yy < y1)) result += substr(s, i * cmd_vis_len, cmd_vis_len) + ((i == ks - 1) ? L"\r\n" : L"");
			if ((yy == y1) && (yy == y0))
			{
				result += substr(s, i * cmd_vis_len + x0, x1 - x0 + 1);
				continue;
			}
			if (yy == y1) result += substr(s, i * cmd_vis_len + x1, cmd_vis_len - x1) + ((i == ks - 1) ? L"\r\n" : L"");
			if (yy == y0) result += substr(s, i * cmd_vis_len, x0 + 1) + ((i == ks - 1) ? L"\r\n" : L"");
		}
	};

	mtx.lock();
	for (auto& s : text) fff(s);
	mtx.unlock();
	fff(prefix + cmd);

	set_clipboard_text(result);
	selection_begin.x = -1;
}

void _e_terminal::key_press(u64 key)
{
	if (key < 32) return;
	if (insert_mode || (cursor >= (i64)cmd.size()))
		cmd.insert(cursor, 1, ushort(key));
	else
		cmd[cursor] = ushort(key);
	cursor++;
	old_cmd_vis_len = -1;
	selection_begin.x = -1;
	cha_area();
	vis_cur = true;
}

bool _e_terminal::mouse_wheel2(_xy r, short value)
{
	scrollbar += value;
	cha_area();
	return true;
}

bool _e_terminal::mouse_down_left2(_xy r)
{
	y0_move_slider = -1;
	r = ui->master_trans_go(r);
	_iarea oo = ui->master_trans_go(local_area);
	if (r.x > oo.x.max - width_scrollbar)
	{ // на полосе прокрутки
		if (r.y < y_slider.min)
			scrollbar += max_lines - 1;
		else
			if (r.y > y_slider.max)
				scrollbar -= max_lines - 1;
			else
			{
				y0_move_slider = r.y;
				scrollbar0_move_slider = scrollbar;
			}
	}
	else
	{ // на тексте
		i64 x_text = oo.x.min + otst_x;
		i64 y_cmd = oo.y.max - otst_y;

		i64 xx = i64((r.x - x_text) / font_width);
		i64 yy = i64((y_cmd - r.y) / font_size + scrollbar);

		selection_begin = { xx, yy };
		selection_end = { xx, yy };
	}
	cha_area();
	return true;
}

void _e_terminal::mouse_move_left2(_xy r)
{
	r = ui->master_trans_go(r);
	_iarea oo = ui->master_trans_go(local_area);
	if (y0_move_slider >= 0)
	{
		i64 ypix = oo.y.length() - otst_y * 2 - y_slider.length();
		i64 yline = full_lines - max_lines;
		scrollbar = scrollbar0_move_slider - i64((r.y - y0_move_slider) * yline / ypix);
		cha_area();
		return;
	}
	// выделение текста
	i64 x_text = oo.x.min + otst_x;
	i64 y_cmd = oo.y.max - otst_y;

	i64 xx = i64((r.x - x_text) / font_width);
	i64 yy = i64((y_cmd - r.y) / font_size + scrollbar);
	selection_end = { xx, yy };
	if (yy - scrollbar < 0) scrollbar--;
	if (yy - scrollbar >= max_lines) scrollbar++;

	cha_area();
}

void _e_terminal::run_cmd()
{
	if (cmd.empty()) return;

	std::wstring command_name;
	std::vector<std::wstring> parameters;

	i64 i0 = cmd.size();
	// пропуск первых пробелов
	for (i64 i = 0; i < (i64)cmd.size(); i++)
		if ((cmd[i] != L' ') && (cmd[i] != L'\t'))
		{
			i0 = i;
			break;
		}
	// вычленение command_name
	for (i64 i = i0; i < (i64)cmd.size(); i++)
		if ((cmd[i] == L' ') || (cmd[i] == L'\t'))
		{
			command_name = cmd.substr(i0, i - i0);
			break;
		}
	if (command_name.empty()) command_name = cmd.substr(i0, cmd.size() - i0);
	// вычленение параметров
	int rez = 0; // 0 - пробелы 1 - набор символов 2 - строка "safasf asf"
	i64 start_p = 0;
	for (i64 i = i0 + command_name.size(); i < (i64)cmd.size(); i++)
	{
		wchar_t c = cmd[i];
		if (rez == 0)
		{
			if ((c == L' ') || (c == L'\t')) continue;
			start_p = i;
			rez = (c == L'"') ? 2 : 1;
			continue;
		}
		if (rez == 1) if ((c != L' ') && (c != L'\t')) continue;
		if (rez == 2) if (c != L'"') continue;
		parameters.push_back(cmd.substr(start_p, i - start_p + (rez == 2)));
		rez = 0;
	}
	if (rez == 1) parameters.push_back(cmd.substr(start_p, cmd.size() - start_p));
	if (!command_name.empty())
	{
		print(prefix + cmd);
		previous_cmd.push_back(cmd);
		act_previous_cmd = previous_cmd.size();
		if (auto cc = command.find(command_name); cc != command.end())
		{
			start_timer();
			cc->second.function(*this, parameters);
			stop_timer(L"время выполнения");
		}
		else
			print(L"команда не найдена");
	}
	print(L"");
	old_cmd_vis_len = -1;
	selection_begin.x = -1;
	cmd.clear();
}

void _e_terminal::print(std::wstring_view s)
{ // *
	std::lock_guard<std::mutex> lck(mtx);
	text.emplace_back(s);
	if (text.size() > 20000) text.erase(text.begin(), text.begin() + 10000);
	need_to_update = true;
}

void _e_terminal::start_timer()
{
	timer.push_back(std::chrono::high_resolution_clock::now());
}

void _e_terminal::stop_timer(std::wstring_view s)
{
	if (timer.empty()) return;
	std::chrono::nanoseconds dt = std::chrono::high_resolution_clock::now() - timer.back();
	timer.pop_back();
	double dtt = dt.count() / 1.0e9;
	print(std::wstring(s) + L": " + double_to_wstring(dtt, 6) + L" сек");
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

