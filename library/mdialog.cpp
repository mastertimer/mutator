#include "mdialog.h"

#include "mwindow.h"
#include "renderer.h"
#include "tetron.h"
#include "renderer.h"

#include <iostream>

void dialog::message(_wstring_view text, _wstring_view title)
{
	_window win;
	_bitmap bm(1, 1);

	auto tsize = bm.size_text(text, 16);
	win.create((int)(tsize.x + 20), (int)(tsize.y + 20));
	win.set_title(title);
	win.set_position(_screen::get_size() / 2 - win.size / 2);

	win.on_click  = [](_window_event::_mouse_click e) { e.window.close(); };
	win.on_key_up = [](_window_event::_key e) { e.window.close(); };
	win.on_render = [text, &bm](_window_event::_render e) {
		_renderer((_color*)bm.data, bm.size).clear(c_background);
		bm.text(10, 10, text, 16, c_def, c_background);
		_renderer(e.window.data, e.size).image(0, 0, e.size.x, e.size.y, (_color*)bm.data, e.size);
	};
	win.on_resize = [&bm](_window_event::_resize e) {
		bm.resize(e.size.x, e.size.y);
	};

	win.event_loop();
}

bool dialog::y_n(_wstring_view text_, _wstring_view title)
{
	_wstring text{text_};
	if (text.empty())
		text = L"[y/n]";
	else
		text += L" [y/n]";

	_window win;
	_bitmap bm(1, 1);

	auto tsize = bm.size_text(text, 16);
	win.create((int)(tsize.x + 20), (int)(tsize.y + 20));
	win.set_title(title);
	win.set_position(_screen::get_size() / 2 - win.size / 2);

	bool res      = false;
	win.on_key_up = [&](auto e) {
		if (e.key == _keyboard::_key::y)
		{
			res = true;
			e.window.close();
		}
		else if (e.key == _keyboard::_key::n)
		{
			e.window.close();
		}
	};
	win.on_render = [&](auto e) {
		_renderer(e.window.data, e.window.size).clear(c_background);
		_renderer((_color*)bm.data, bm.size).clear(c_background);
		bm.text(10, 10, text, 16, c_def, c_background);
		_renderer(e.window.data, e.size).image(0, 0, e.size.x, e.size.y, (_color*)bm.data, e.size);
	};
	win.on_resize = [&bm](_window_event::_resize e) {
		bm.resize(e.size.x, e.size.y);
	};
	win.event_loop();

	return res;
}

_path dialog::open_file(_wstring_view filter, _wstring_view def_name)
{
	const auto sep = filter.find_last_of('.');
	if (sep == _wstring::npos) return def_name;
	const _wstring name = _wstring(filter.begin(), filter.begin() + sep);
	const _wstring ext  = _wstring(filter.begin() + sep, filter.end());

	for (auto& file : std::filesystem::directory_iterator(L"."))
	{
		if (file.is_directory()) continue;
		auto path = file.path();
		if (ext == L"*" || path.extension() == ext)
			if (name == L"*" || path.filename() == name) return path;
	}
	return def_name;
}

void dialog::panic(_wstring_view text)
{
	std::wcerr << text << std::endl;
	dialog::message(text, L"ошибка");
	std::exit(1);
}
