#include "mdialog.h"
#include "mutator.h"
#include "mwindow.h"

#include <fstream>

std::filesystem::path ini_file = L"theme.json";
_wstring tetrons_file;

_window win;

void set_cursorx(_cursor x)
{
	win.set_cursor(x);
	g_cursor = x;
}

void change_window_text()
{
	static std::wstring s_old;
	wchar_t             s[100];
	swprintf(s, 100, L"%d  %4.1e", all_tetron.size, mutator::get_main_scale());
	if (s_old == s) return;
	s_old = s;
	win.set_title(s);
}

void change_window_text2(std::chrono::nanoseconds t)
{
	static double t_old = 0;
	double tt = (t.count() / 1000000.0) * 0.01 + t_old * 0.99;
	t_old = tt;
	std::wstring s = double_to_string(tt, 2);
	win.set_title(s);
}

bool run_timer = true;

void resize_mutator(_window_event::_resize e)
{
	master_bm.resize(e.size.x, e.size.y);
	render.target((_color*)master_bm.data, master_bm.size);
	master_obl_izm = _area_old(0, e.size.x, 0, e.size.y);
}

void on_mouse_move(_window_event::_mouse_move e)
{
	mouse_xy = e.pos;
	n_move->run(0, n_move, flag_run);
	win.display();
}

void on_mouse_button(_window_event::_mouse_button e)
{
	switch (e.button)
	{
	case _mouse::_button::left:
		mutator::mouse_button_left({ e.mouse.position.x, e.mouse.position.y }, e.pressed);
		break;
	case _mouse::_button::right:
		mutator::mouse_button_right({ e.mouse.position.x, e.mouse.position.y }, e.pressed);
		break;
	case _mouse::_button::middle:
		mutator::mouse_button_middle({ e.mouse.position.x, e.mouse.position.y }, e.pressed);
		break;
	}
	win.display();
}

void on_click(_window_event::_mouse_click e)
{
	if (e.double_click)
	{
		*n_s_double->operator int64*() = true;
		if (e.button == _mouse::_button::left)
			n_down_left->run(0, n_down_left, flag_run);
		else if (e.button == _mouse::_button::right)
			n_down_right->run(0, n_down_right, flag_run);
		else if (e.button == _mouse::_button::middle)
			n_down_middle->run(0, n_down_middle, flag_run);
		*n_s_double->operator int64*() = false;
	}
}

void on_scroll(_window_event::_mouse_scroll e)
{
	*n_wheel->operator int64*() = (int64)e.delta;
	n_wheel->run(0, n_wheel, flag_run);
	win.display();
}

void on_timer(_window_event::_timer e)
{
	if (!run_timer) return;
	if (e.id == 1)
	{
		n_timer1000->run(0, n_timer1000, flag_run);
		win.display();
	}
}

void on_char(_window_event::_text e)
{
	*n_press_key->operator int64* () = e.symb; 

	n_press_key->run(0, n_press_key, flag_run);
	win.display();
}

void on_key(_window_event::_key e)
{
	// TODO: повторение клавиш left, right, backspace и т.п.
	*n_s_shift->operator int64*() = e.shift;
	*n_s_alt->operator int64*()   = e.alt;
	*n_s_ctrl->operator int64*()  = e.control;
	if (e.pressed)
	{
		*n_down_key->operator int64* () = (int64)e.key;
		n_down_key->run(0, n_down_key, flag_run);
	}
	else
	{
		switch (e.key)
		{
		case _keyboard::_key::f1:
			win.set_fullscreen(!win.is_fullscreen());
			break;
		case _keyboard::_key::f2:
			run_timer = false;
			if (dialog::y_n(L"сохранить?"))	mutator::save_to_txt_file(tetrons_file);
			run_timer = true;
			break;
		}
	}
	win.display();
}

void load_theme(std::filesystem::path file_name)
{
	_rjson fs(file_name);
	fs.read("tetrons", tetrons_file);
	fs.read("c_background", c_background);
	fs.read("c_maxx", c_maxx);
	fs.read("c_max", c_max);
	fs.read("c_def", c_def);
	fs.read("c_min", c_min);
	fs.read("c_minn", c_minn);
	fs.read("zoom_speed", zoom_speed);
}

int main()
{
	load_theme(ini_file);
	if (!mutator::start(tetrons_file)) return 1;

	set_cursorx((*n_perenos->operator int64*()) ? _cursor::size_all : _cursor::normal);

	win.create(1200, 800);
	win.on_render     = mutator::draw;
	win.on_resize     = resize_mutator;
	win.on_mouse_move = on_mouse_move;
	win.on_scroll     = on_scroll;
	win.on_text_enter = on_char;

	win.on_mouse_down = win.on_mouse_up = on_mouse_button;
	win.on_click = win.on_double_click = on_click;
	win.on_key_down = win.on_key_up = on_key;

	win.add_timer(1, 1s);
	win.on_timer = on_timer;

	return win.event_loop();
}
