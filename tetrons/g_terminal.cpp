#include "g_terminal.h"

#include "exchange_research.h"
#include "compression.h"
#include "RtMidi.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct _cmd_load_sable_stat : public _g_terminal::_command
{
	std::wstring help() override { return L"статистика цен"; }

	void run(_g_terminal* t, std::vector<std::wstring>& parameters) override
	{
		exchange_fun1(t);
	}
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct _cmd_sad : public _g_terminal::_command
{
	std::wstring help() override { return L"спрос и предложение"; }

	void run(_g_terminal* t, std::vector<std::wstring>& parameters) override
	{
		exchange_fun2(t, parameters);
	}
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct _cmd_delta : public _g_terminal::_command
{
	std::wstring help() override { return L"разность цен"; }

	void run(_g_terminal* t, std::vector<std::wstring>& parameters) override
	{
		exchange_fun3(t, parameters);
	}
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

std::wstring test_file = L"e:\\mutator\\data\\t110521.txt";

struct _cmd_test_arithmetic_coding : public _g_terminal::_command
{
	std::wstring help() override { return L"тестирование арифметического кодирования"; }
	void run(_g_terminal* t, std::vector<std::wstring>& parameters) override
	{
		t->print(L"файл: " + test_file);
		std::vector<uchar> data, data2;
		_bit_vector res;
		if (!load_file(test_file, data))
		{
			t->print(L"ошибка загрузки!");
			return;
		}
		t->print(L"размер:            " + std::to_wstring(data.size()));
		double com1 = 0.0, com2 = 0.0;
		double com = information(data, &com1, &com2);
		t->print(L"информация:        " + double_to_wstring(com, 1) + L" (" + double_to_wstring(com1, 1) + L" + " +
			double_to_wstring(com2, 1) + L")");
		t->print(L"идеал:             " + double_to_wstring(size_arithmetic_coding(data), 1));

		i64 n = 1;
		if (!parameters.empty()) n = std::stoi(parameters[0]);
		if (n < 1) n = 1;
		if (n > 1) t->print(std::to_wstring(n) + L" испытаний");

		i64 mindt = 1000000000;
		i64 maxdt = 0;
		i64 summdt = 0;

		for (i64 i = 0; i < n; i++)
		{
			auto tt = std::chrono::high_resolution_clock::now();
			arithmetic_coding(data, res);
			std::chrono::nanoseconds dt = std::chrono::high_resolution_clock::now() - tt;
			i64 dtt = dt.count() / 1000;
			if (dtt < mindt) mindt = dtt;
			if (dtt > maxdt) maxdt = dtt;
			summdt += dtt;
		}


		double v = res.size() / 8.0;
		t->print(L"arithmetic_coding: " + double_to_wstring(v, 1));
		t->print(L"разница:           " + double_to_wstring(v - com, 1));
		t->print(L"среднее время, мксек:      " + std::to_wstring(summdt / n));
		t->print(L"минимальное время, мксек:  " + std::to_wstring(mindt));
		t->print(L"максимальное время, мксек: " + std::to_wstring(maxdt));

		mindt = 1000000000;
		maxdt = 0;
		summdt = 0;

		for (i64 i = 0; i < n; i++)
		{
			auto tt = std::chrono::high_resolution_clock::now();
			arithmetic_decoding(res, data2);
			std::chrono::nanoseconds dt = std::chrono::high_resolution_clock::now() - tt;
			i64 dtt = dt.count() / 1000;
			if (dtt < mindt) mindt = dtt;
			if (dtt > maxdt) maxdt = dtt;
			summdt += dtt;
		}
		if (data == data2)
			t->print(L"расжатие норма");
		else
			t->print(L"!!ошибка!! расжатый файл не равен исходному!");
		t->print(L"среднее время, мксек:      " + std::to_wstring(summdt / n));
		t->print(L"минимальное время, мксек:  " + std::to_wstring(mindt));
		t->print(L"максимальное время, мксек: " + std::to_wstring(maxdt));
	}
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct _cmd_test_arithmetic_coding2 : public _g_terminal::_command
{
	std::wstring help() override { return L"тестирование арифметического кодирования"; }
	void run(_g_terminal* t, std::vector<std::wstring>& parameters) override
	{
		t->print(L"файл: " + test_file);
		std::vector<uchar> data, data2;
		_bit_vector res;
		if (!load_file(test_file, data))
		{
			t->print(L"ошибка загрузки!");
			return;
		}
		double f0 = 0.01;
		t->print(L"размер: " + std::to_wstring(data.size()));
		t->print(L"идеал1: " + double_to_wstring(size_arithmetic_coding(data), 1));
		t->print(L"идеал0: " + double_to_wstring(size_arithmetic_coding(data, f0), 1));

		i64 n = 1;
		if (!parameters.empty()) n = std::stoi(parameters[0]);

		for (i64 i = 0; i < n; i++)
		{
			stir_vector(data);
			t->print(L"идеал:  " + double_to_wstring(size_arithmetic_coding(data, f0), 1));
		}
	}
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct _cmd_test_ppm : public _g_terminal::_command
{
	std::wstring help() override { return L"тестирование ppm сжатия"; }
	void run(_g_terminal* t, std::vector<std::wstring>& parameters) override
	{
		t->print(L"файл: " + test_file);
		std::vector<uchar> data, data2, res;
		if (!load_file(test_file, data))
		{
			t->print(L"ошибка загрузки!");
			return;
		}
		t->print(L"размер: " + std::to_wstring(data.size()));

		i64 n = 0;
		if (!parameters.empty()) n = std::stoi(parameters[0]);
		if (n < 0) n = 0;
		t->print(L"порядок = " + std::to_wstring(n));

		i64 mindt = 1000000000;
		i64 maxdt = 0;
		i64 summdt = 0;

		{
			auto tt = std::chrono::high_resolution_clock::now();
			ppm(data, res, n);
			std::chrono::nanoseconds dt = std::chrono::high_resolution_clock::now() - tt;
			i64 dtt = dt.count() / 1000;
			if (dtt < mindt) mindt = dtt;
			if (dtt > maxdt) maxdt = dtt;
			summdt += dtt;
		}

		double v = res.size();
		t->print(L"ppm:    " + double_to_wstring(v, 0));
		t->print(L"время, мксек:  " + std::to_wstring(mindt));

	}
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

const double kln2 = -1.0 / log(2.0);

// p - реальная вероятность
// k - оценочная вероятность

double entropy01(double p) // энтропия 01: p(1) = p, p(0) = 1-p
{
	return kln2 * (p * log(p) + (1.0 - p) * log(1.0 - p));
}

double entropy2(double p, double k) // энтропия второго символа по предсказанию
{
	return kln2 * (p * (p * log(k) + (1.0 - p) * log(1.0 - k)) + (1.0 - p) * (p * log(1.0 - k) + (1.0 - p) * log(k)));
}

double ratio_entropy2(double p, double k) // во сколько раз энтропия хуже идеальной
{
	return entropy2(p, k) / entropy01(p);
}

double f1(double p)
{
	return ratio_entropy2(p, 0.82);
}

struct _cmd_test : public _g_terminal::_command
{
	std::wstring help() override { return L"тестирование mask1"; }

	void run(_g_terminal* t, std::vector<std::wstring>& parameters) override
	{
		_area ee{ {0,1.3}, {0,2.2} };
		_area bb;
		bb = ee;

		_offer a{ 4, 4 };
		_offer b{ 4, 4 };

//		t->print(std::to_wstring(sizeof(_offers)));
		t->print((a<b)?L"<":L"!<");

/*		u64 err = 0;
		for (u64 i = 0; i < 100000; i++)
		{
			u64 k = rnd(256) << rnd(57);
			if (position1_64_2(k) != position1_64(k)) err++;
		}
		t->print(L"err=" + std::to_wstring(err));*/

/*		u64 r = 0;
		for (u64 i = 0; i < 8; i++)
		{
			t->start_timer();
			for (u64 j = 0; j < 500000; j++)
			{
				for (u64 k = 0; k < 256; k++)
				{
					r += position1_64(k << (i * 8));
				}
			}
			t->stop_timer(std::to_wstring(i));
		}
		t->print(L"r="+std::to_wstring(r));*/
	}
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct _cmd_clear : public _g_terminal::_command
{
	std::wstring help() override { return L"очищение экрана"; }
	void run(_g_terminal* t, std::vector<std::wstring>& parameters) override
	{
		t->text_clear();
	}
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct _cmd_help : public _g_terminal::_command
{
	std::wstring help() override { return L"вывод справки"; }
	void run(_g_terminal* t, std::vector<std::wstring>& parameters) override
	{
		for (auto& i : t->command) t->print(i.first + L" - " + i.second->help());
	}
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void fun_piano(double deltatime, std::vector< unsigned char >* message, void* userData)
{
	_g_terminal* t = (_g_terminal*)userData;
	auto nBytes = message->size();
	for (auto i = 0; i < nBytes; i++)
		t->print(L"Byte " + std::to_wstring(i) + L" = " + std::to_wstring((int)message->at(i)) + L", ");
	if (nBytes > 0)
		t->print(L"stamp = " + std::to_wstring(deltatime));
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct _cmd_piano : public _g_terminal::_command
{
	RtMidiIn* midiin = nullptr;

	~_cmd_piano()
	{
		if (midiin) delete midiin;
	}

	std::wstring help() override { return L"пианино"; }
	void run(_g_terminal* t, std::vector<std::wstring>& parameters) override
	{
		if (midiin)
		{
			delete midiin;
			midiin = nullptr;
			t->print(L"пианино удалено");
			return;
		}
		midiin = new RtMidiIn();
		unsigned int n_ports = midiin->getPortCount();
		if (n_ports == 0)
		{
			t->print(L"нет портов для пианино");
			delete midiin;
			midiin = nullptr;
			return;
		}
		t->print(L"количество портов: " + std::to_wstring(n_ports));
		midiin->openPort(0);
		midiin->setCallback(&fun_piano, t);
		midiin->ignoreTypes(false, true, true);
		t->print(L"пианино стартануло!");
	}
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct _cmd_line : public _g_terminal::_command
{
	std::wstring help() override { return L"тест скорости рисования линий"; }
	void run(_g_terminal* t, std::vector<std::wstring>& parameters) override
	{
		int iter = 1000000;
		_picture k1({ 3000, 3000 }, 0xFF000000);
		_picture k2({ 3000, 3000 }, 0xFF000000);
		rnd.init(0);
		t->start_timer();
		for (auto i = 0; i < iter; i++)
		{
			auto x = rnd(3000);
			auto y1 = rnd(3000);
			auto y2 = rnd(3000);
			uint c = rnd(0xFFFFFFFF);
			k1.line({ x, y1 }, { x, y2 }, c);
		}
		t->stop_timer(L"line");
		rnd.init(0);
		t->start_timer();
		for (auto i = 0; i < iter; i++)
		{
			auto x = rnd(3000);
			auto y1 = rnd(3000);
			auto y2 = rnd(3000);
			uint c = rnd(0xFFFFFFFF);
			k2.line2({ x, y1 }, { x, y2 }, c);
		}
		t->stop_timer(L"line2");
		if (k1 == k2)
			t->print(L"картинки равны");
		else
			t->print(L"!!НЕСОВПАДЕНИЕ");
	}
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

_g_terminal::_g_terminal()
{
	local_area = { {0, 100}, {0, 100} };
	key_fokus = true;

	command.insert({ L"clear", std::unique_ptr<_command>(new _cmd_clear) });
	command.insert({ L"help",  std::unique_ptr<_command>(new _cmd_help) });
	command.insert({ L"test",  std::unique_ptr<_command>(new _cmd_test) });
	command.insert({ L"a",     std::unique_ptr<_command>(new _cmd_test_arithmetic_coding) });
	command.insert({ L"aa",    std::unique_ptr<_command>(new _cmd_test_arithmetic_coding2) });
	command.insert({ L"ppm",   std::unique_ptr<_command>(new _cmd_test_ppm) });
	command.insert({ L"1",     std::unique_ptr<_command>(new _cmd_load_sable_stat) });
	command.insert({ L"sad",   std::unique_ptr<_command>(new _cmd_sad) });
	command.insert({ L"delta", std::unique_ptr<_command>(new _cmd_delta) });
	command.insert({ L"line",  std::unique_ptr<_command>(new _cmd_line) });
	command.insert({ L"p",     std::unique_ptr<_command>(new _cmd_piano) });
}

void _g_terminal::start_timer()
{
	timer.push_back(std::chrono::high_resolution_clock::now());
}

void _g_terminal::stop_timer(std::wstring_view s)
{
	if (timer.empty()) return;
	std::chrono::nanoseconds dt = std::chrono::high_resolution_clock::now() - timer.back();
	timer.pop_back();
	double dtt = dt.count() / 1.0e9;
	print(std::wstring(s) + L": " + double_to_wstring(dtt, 6) + L" сек");
}

bool _g_terminal::mouse_down_left2(_xy r)
{
	y0_move_slider = -1;
	r = master_trans_go(r);
	_iarea oo = master_trans_go(local_area);
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

		i64 xx = (r.x - x_text) / font_width;
		i64 yy = (y_cmd - r.y) / font_size + scrollbar;

		selection_begin = { xx, yy };
		selection_end = { xx, yy };
	}
	cha_area();
	return true;
}

void _g_terminal::mouse_move_left2(_xy r)
{
	r = master_trans_go(r);
	_iarea oo = master_trans_go(local_area);
	if (y0_move_slider >= 0)
	{
		i64 ypix = oo.y.size() - otst_y * 2 - y_slider.size();
		i64 yline = full_lines - max_lines;
		scrollbar = scrollbar0_move_slider - (r.y - y0_move_slider) * yline / ypix;
		cha_area();
		return;
	}
	// выделение текста
	i64 x_text = oo.x.min + otst_x;
	i64 y_cmd = oo.y.max - otst_y;

	i64 xx = (r.x - x_text) / font_width;
	i64 yy = (y_cmd - r.y) / font_size + scrollbar;
	selection_end = { xx, yy };
	if (yy - scrollbar < 0) scrollbar--;
	if (yy - scrollbar >= max_lines) scrollbar++;

	cha_area();
}

bool _g_terminal::mouse_wheel2(_xy r)
{
	scrollbar += *n_wheel->operator i64 * ();
	cha_area();
	return true;
}

void _g_terminal::run_cmd()
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
	int start_p = 0;
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
			cc->second->run(this, parameters);
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

void _g_terminal::print(std::wstring_view s)
{ // *
	std::lock_guard<std::mutex> lck(mtx);
	text.emplace_back(s);
	if (text.size() > 20000) text.erase(text.begin(), text.begin() + 10000);
	need_to_update = true;
}

void _g_terminal::set_clipboard()
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

void _g_terminal::key_down(ushort key)
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
		if (*(i64*)*n_s_ctrl) set_clipboard();
		break;
	}
	cha_area();
}

void _g_terminal::key_press(ushort key)
{
	if (key < 32) return;
	if (insert_mode || (cursor >= (i64)cmd.size()))
		cmd.insert(cursor, 1, key);
	else
		cmd[cursor] = key;
	cursor++;
	old_cmd_vis_len = -1;
	selection_begin.x = -1;
	cha_area();
	vis_cur = true;
}

void _g_terminal::run(_tetron* tt0, _tetron* tt, u64 flags)
{
	std::lock_guard<std::mutex> lck(mtx);
	if (need_to_update)
	{
		need_to_update = false;
		visible_cursor = true;
		cha_area();
		return;
	}
	add_obl_izm(area_cursor);
	visible_cursor = !visible_cursor;
}

void _g_terminal::ris2(_trans tr, bool final)
{
	std::lock_guard<std::mutex> lck(mtx);
	std::wstring old_font = master_bm.get_font_name();
	master_bm.set_font(L"Consolas", false);
	if (font_width == 0) font_width = master_bm.size_text("0123456789", font_size).x / 10;

	std::wstring full_cmd = prefix + cmd;

	_iarea oo = tr(local_area);
	_iarea oo2 = oo;
	oo2.x.max -= width_scrollbar - 1;

	i64 x_text = oo.x.min + otst_x;
	i64 y_cmd = oo.y.max - font_size - otst_y;
	cmd_vis_len = (oo.x.size() - otst_x * 2 - width_scrollbar) / font_width;
	if (cmd_vis_len <= 0) cmd_vis_len = 1;
	i64 ks = (full_cmd.size() + cmd_vis_len) / cmd_vis_len;

	max_lines = (oo.y.size() - otst_y * 2) / font_size; // строк в окне

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

	i64 length_slider = max_lines * (oo.y.size() - otst_y * 2) / full_lines;
	if (length_slider > oo.y.size() - otst_y * 2) length_slider = oo.y.size() - otst_y * 2;
	if (length_slider < 10) length_slider = 10;

	uint c2 = get_c2();
	uint c0 = get_c();

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
			area_cursor.clear();
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
	if (_area(area_cursor) == master_obl_izm) // перерисовать только курсор
	{
		if (area_cursor.empty()) goto finish; // перестраховка
		master_bm.text({ area_cursor.x.min, area_cursor.y.min }, cmd.substr(cursor, 1), font_size, cc2, cc0);
		if (visible_cursor) master_bm.fill_rectangle(area_cursor, cc3 - 0xC0000000);
		goto finish;
	}

	master_bm.fill_rectangle(oo2, c2);
	if (((c0 >> 24) != 0x00) && (c0 != c2)) master_bm.rectangle(oo2, c0);
	if ((oo2.y.size() < 30) || (oo2.x.size() < 30)) goto finish;

	if (full_lines > max_lines)
	{ // ползунок
		master_bm.line({ oo.x.max - 1, oo.y.min }, { oo.x.max - 1, oo.y.max }, c0);
		master_bm.line({ oo2.x.max, oo.y.min }, { oo.x.max - 2, oo.y.min }, c0);
		master_bm.line({ oo2.x.max, oo.y.max - 1 }, { oo.x.max - 2, oo.y.max - 1 }, c0);

		i64 tt = (oo.y.size() - otst_y * 2 - length_slider) * scrollbar / (full_lines - max_lines);

		y_slider = { oo.y.max - otst_y - tt - length_slider, oo.y.max - otst_y - tt };
		master_bm.fill_rectangle({ {oo.x.max - width_scrollbar + 2, oo.x.max - 2}, y_slider }, c0);
	}

	if (n_act_key == this)
		n_timer1000->add_flags(this, flag_run, false);
	else
		n_timer1000->del_flags(this, flag_run, false);

	for (i64 i = 0; i < ks; i++)
	{
		i64 n = ks - 1 - i - scrollbar;
		if (n < 0) break;
		if (n >= max_lines) continue;
		master_bm.text({ x_text, y_cmd - n * font_size }, full_cmd.substr(i * cmd_vis_len, cmd_vis_len),
			font_size, cc2, cc0);
	}

	if (visible_cursor) master_bm.fill_rectangle(area_cursor, cc3 - 0xC0000000);

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
			master_bm.text({ x_text, y_cmd - n * font_size }, s.substr(j * cmd_vis_len, cmd_vis_len),
				font_size, cc1, cc0);
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
				master_bm.fill_rectangle(a, cc3 - 0xA0000000);
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
				master_bm.fill_rectangle(a, cc3 - 0xA0000000);
			}
			for (i64 yy = y1 - 1; yy > y0; yy--)
				if ((yy >= 0) && (yy < max_lines))
				{
					_iarea a = { {x_text, x_text + cmd_vis_len * font_width},
					{y_cmd - yy * font_size, y_cmd - (yy - 1) * font_size} };
					master_bm.fill_rectangle(a, cc3 - 0xA0000000);
				}

			if ((y0 >= 0) && (y0 < max_lines))
			{
				_iarea a = { {x_text, x_text + (x0 + 1) * font_width},
				{y_cmd - y0 * font_size, y_cmd - (y0 - 1) * font_size} };
				master_bm.fill_rectangle(a, cc3 - 0xA0000000);
			}
		}
	}

finish:
	master_bm.set_font(old_font.c_str(), false);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
