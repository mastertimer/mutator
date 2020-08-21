#pragma once

#include "mgeometry.h"
#include "color.h"

#include <functional>
#include <memory>

#include "mtime.hpp"

struct _picture;
struct _window;

struct _mouse
{
	vec2i position = {0, 0};
	enum class _button
	{
		left = 0,
		right,
		middle,
		forward,
		backward,
		button_count
	};
	bool pressed[(int)_button::button_count] = {};
};
struct _keyboard
{
	enum class _key
	{
		unknown = -1,
		// clang-format off
		a = 0, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t, u, v, w, x, y, z,
		num0, num1, num2, num3, num4, num5, num6, num7, num8, num9,
		escape, control_l, shift_l, alt_l, system_l, control_r, shift_r, alt_r, system_r, menu, bracket_l, bracket_r,
		semicolon, comma, dot, quote, slash, backslash, tilde, equal, minus, space, enter, backspace, tab,
		page_up, page_down, end, home, insert, del, add, subtract, multiply, divide, left, right, up, down,
		numpad0, numpad1, numpad2, numpad3, numpad4, numpad5, numpad6, numpad7, numpad8, numpad9,
		f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, f13, f14, f15, pause,
		// clang-format on

		key_count,
	};
	bool pressed[(int)_key::key_count] = {};

//	static int to_num(_key code); // цифра или -1
};

namespace _window_event
{
	struct _render
	{
		_window& window;
		_color* data;
		vec2i size;
	};
	struct _resize
	{
		_window& window;
		vec2i old_size;
		vec2i size;
	};
	struct _move
	{
		_window& window;
		vec2i old_position;
		vec2i position;
	};
	struct _close
	{
		_window& window;
	};
	struct _timer
	{
		_window& window;
		uint64 id;
	};
	struct _mouse_move
	{
		_window& window;
		_mouse& mouse;
		vec2i old_pos;
		vec2i pos;
	};
	struct _mouse_button
	{
		_window& window;
		_mouse& mouse;
		_mouse::_button button;
		bool pressed;
	};
	struct _mouse_click
	{
		_window& window;
		_mouse& mouse;
		_mouse::_button button;
		bool double_click;
	};
	struct _mouse_scroll
	{
		_window& window;
		_mouse& mouse;
		double delta;
	};
	struct _focus
	{
		_window& window;
		bool focus;
	};
	struct _key
	{
		_window& window;
		_keyboard& keyboard;
		_keyboard::_key key;
		bool pressed;
		bool alt, control, shift, system;
	};
	struct _text
	{
		_window& window;
		_keyboard& keyboard;
		wchar_t symb;
	};
	struct _tick
	{
		_window& window;
		_mouse& mouse;
		_keyboard& keyboard;
		_milliseconds delta_time;
	};
}

struct _window
{
	// TODO: on_create, get_position
	_color* data;
	vec2i size;

	_window();
	_window(int w, int h);
	_window(int w, int h, std::wstring_view title);
	~_window();

	void create(int w, int h);
	void set_title(std::wstring_view value);
	void close();

	void add_timer(uint64 id, _milliseconds delay);
	void remove_timer(uint64 id);

	void display();
	int  event_loop();

	void set_fullscreen(bool value);
	bool is_fullscreen();

	void set_cursor(_cursor type);
	void set_icon(_color const* data, vec2i size);
	void set_icon(_picture const& pic);
	void clear_icon();

	void set_position(vec2i p);
	void set_size(vec2i size);

	t_t using _handler = std::function<void(_t)>;

	_handler<_window_event::_render> on_render;
	_handler<_window_event::_resize> on_resize;
	_handler<_window_event::_close>  on_close;
	_handler<_window_event::_focus>  on_focus;
	_handler<_window_event::_focus>  on_unfocus;
	_handler<_window_event::_move>   on_move;

	_mouse mouse;

	_handler<_window_event::_mouse_move>   on_mouse_move;
	_handler<_window_event::_mouse_move>   on_mouse_leave;
	_handler<_window_event::_mouse_move>   on_mouse_enter;
	_handler<_window_event::_mouse_button> on_mouse_down;
	_handler<_window_event::_mouse_button> on_mouse_up;
	_handler<_window_event::_mouse_click>  on_click;
	_handler<_window_event::_mouse_click>  on_double_click;
	_handler<_window_event::_mouse_scroll> on_scroll;

	_keyboard keyboard;

	_handler<_window_event::_key>  on_key_up;
	_handler<_window_event::_key>  on_key_down;
	_handler<_window_event::_text> on_text_enter;

	_handler<_window_event::_tick>  on_tick;
	_handler<_window_event::_timer> on_timer;

	_system_time_point last_tick_time;
	bool has_focus();

	struct _impl;

  private:
	_impl* impl;
};

struct _screen
{
	static vec2i get_size();
};
