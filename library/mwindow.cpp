#include <map>

#include "mgraphics.h"
#include "xlinux.h"
#include "xwindows.h"
#include "mwindow.h"
//#include "tetron.h"

#if TARGET_SYSTEM_WINDOWS

extern int main();

TCHAR szWindowClass[] = L"win64app";

_keyboard::_key MapW2S_keycode(WPARAM key, LPARAM flags);

struct _window::_impl
{
	_window*   orig;
	_color*&   data;
	vec2i&     size;
	_mouse&    mouse;
	_keyboard& keyboard;

	HWND    handle     = nullptr;
	HBITMAP image      = nullptr;
	HDC     image_hdc  = nullptr;
	bool    is_open    = false;
	bool    is_created = false;
	bool    has_focus  = false;

	_impl(_window* orig) : orig(orig), data(orig->data), size(orig->size), mouse(orig->mouse), keyboard(orig->keyboard)
	{
	}

	static LRESULT wnd_proc(HWND handle, UINT message, WPARAM wParam, LPARAM lParam);

	void create(int w, int h)
	{
		if (handle) return;
		using namespace sysg;

		handle = CreateWindow(szWindowClass, L"", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, w, h, NULL, NULL,
		                      hInstance, NULL);
		if (!handle) std::exit(3);
		is_created = true;
		has_focus  = true;
		SetWindowLongPtrW(handle, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));

		for (auto [id, d] : timers) SetTimer(handle, id, d, 0);
		track_mouse_leave();

		set_size({w, h});
		size = {w, h};

		BITMAPINFO bmi = {{sizeof(BITMAPINFOHEADER), (LONG)w, -(LONG)h, 1, 32, BI_RGB, 0, 0, 0, 0, 0}};
		image          = CreateDIBSection(0, &bmi, DIB_RGB_COLORS, (void**)(&data), 0, 0);
		image_hdc      = CreateCompatibleDC(0);
		if (!image_hdc) return;
		if (image != 0) SelectObject(image_hdc, image);
	}
	void set_title(std::wstring_view value) { SetWindowTextW(handle, value.data()); }

	void _display()
	{
		PAINTSTRUCT ps;
		BeginPaint(handle, &ps);
		HDC  hdc = GetDC(handle);
		RECT rect;
		GetClientRect(handle, &rect);

		BitBlt(hdc, 0, 0, rect.right, rect.bottom, image_hdc, 0, 0, SRCCOPY);
//		BitBlt(hdc, 0, 0, rect.right, rect.bottom, master_bm.hdc, 0, 0, SRCCOPY);
		ReleaseDC(handle, hdc);
		EndPaint(handle, &ps);
	}
	void display()
	{
		RECT rect;
		GetClientRect(handle, &rect);
		InvalidateRect(handle, &rect, false);
	}

	void update_geometry()
	{
		RECT rect;
		GetClientRect(handle, &rect);
		size = {(int32)rect.right, (int32)rect.bottom};

		DeleteDC(image_hdc);
		DeleteObject(image);
		BITMAPINFO bmi = {{sizeof(BITMAPINFOHEADER), (LONG)size.x, -(LONG)size.y, 1, 32, BI_RGB, 0, 0, 0, 0, 0}};
		image          = CreateDIBSection(0, &bmi, DIB_RGB_COLORS, (void**)(&data), 0, 0);
		image_hdc      = CreateCompatibleDC(0);
		HGDIOBJ old    = image ? SelectObject(image_hdc, image) : 0;
		if (old) DeleteObject(old);
	}

	void track_mouse_hover()
	{
		TRACKMOUSEEVENT ev = {sizeof(TRACKMOUSEEVENT), TME_HOVER, handle, 0};
		TrackMouseEvent(&ev);
	}
	void track_mouse_leave()
	{
		TRACKMOUSEEVENT ev = {sizeof(TRACKMOUSEEVENT), TME_LEAVE, handle, 0};
		TrackMouseEvent(&ev);
	}

	void close()
	{
		if (is_open) { SendMessage(handle, WM_CLOSE, 0, 0); }
	}

	~_impl()
	{
		DeleteDC(image_hdc);
		DeleteObject(image);
	}

	void set_position(vec2i p) { SetWindowPos(handle, 0, p.x, p.y, 0, 0, SWP_NOSIZE); }
	void set_size(vec2i p)
	{
		RECT  rcClient, rcWind;
		vec2i border;
		GetClientRect(handle, &rcClient);
		GetWindowRect(handle, &rcWind);
		border.x = (rcWind.right - rcWind.left) - rcClient.right;
		border.y = (rcWind.bottom - rcWind.top) - rcClient.bottom;
		MoveWindow(handle, rcWind.left, rcWind.top, p.x + border.x, p.y + border.y, TRUE);
	}

	HICON icon = 0;

	void set_icon(_color const* data, vec2i size)
	{
/*		if (icon) DestroyIcon(icon);
		icon = CreateIcon(sysg::hInstance, size.x, size.y, 1, 32, NULL, (const BYTE*)data);
		if (icon)
		{
			SendMessageW(handle, WM_SETICON, ICON_BIG, (LPARAM)icon);
			SendMessageW(handle, WM_SETICON, ICON_SMALL, (LPARAM)icon);
		}*/
	}

	void clear_icon()
	{
		if (icon) DestroyIcon(icon);
		SendMessageW(handle, WM_SETICON, ICON_BIG, (LPARAM)0);
		SendMessageW(handle, WM_SETICON, ICON_SMALL, (LPARAM)0);
	}

	std::map<uint64, UINT> timers;

	void add_timer(uint64 id, _milliseconds delay)
	{
		if (timers.count(id) != 0) return;
		timers[id] = (UINT)delay.count();
		if (is_created) SetTimer(handle, id, (UINT)delay.count(), 0);
	}
	void remove_timer(uint64 id)
	{
		if (timers.count(id) == 0) return;
		timers.erase(id);
		if (is_created) KillTimer(handle, id);
	}

	int event_loop()
	{
		using namespace sysg;

		orig->last_tick_time = std::chrono::system_clock::now();
		ShowWindow(handle, nCmdShow);
		UpdateWindow(handle);

		is_open = true;

		MSG msg;
		while (is_open)
		{
			bool catched = false;
			while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
			{
				catched = true;
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}

			if (!catched) std::this_thread::sleep_for(2ms); // не так быстро!

			auto now             = std::chrono::system_clock::now();
			auto delta_time      = std::chrono::duration_cast<std::chrono::milliseconds>(now - orig->last_tick_time);
			orig->last_tick_time = now;

			push_event(orig->on_tick, mouse, keyboard, delta_time);
		}
		return int(msg.wParam);
	}

	template <class _f, class... _p>
	void push_event(_f& f, _p... p)
	{
		if (f) f({*orig, p...});
	}

	bool clicking[(int)_mouse::_button::button_count] = {};

	bool first_paint = true;

	LRESULT pool_event(UINT message, WPARAM wParam, LPARAM lParam)
	{
		const auto btnid = [&]() -> _mouse::_button {
			if (message == WM_LBUTTONDOWN || message == WM_LBUTTONUP || message == WM_LBUTTONDBLCLK)
				return _mouse::_button::left;
			if (message == WM_MBUTTONDOWN || message == WM_MBUTTONUP || message == WM_MBUTTONDBLCLK)
				return _mouse::_button::middle;
			if (message == WM_RBUTTONDOWN || message == WM_RBUTTONUP || message == WM_RBUTTONDBLCLK)
				return _mouse::_button::right;
			if (message == WM_XBUTTONDOWN || message == WM_XBUTTONUP || message == WM_XBUTTONDBLCLK)
			{
				UINT button = GET_XBUTTON_WPARAM(wParam);
				if (button == MK_XBUTTON1) return _mouse::_button::backward;
				if (button == MK_XBUTTON2) return _mouse::_button::forward;
			}
			return _mouse::_button::left;
		};

		switch (message)
		{
		case WM_PAINT:
		{
			RECT rect;
			GetClientRect(handle, &rect);
			if (rect.right != size.x || rect.bottom != size.y || first_paint)
			{
				auto osize = size;
				update_geometry();
				push_event(orig->on_resize, osize, size);
				first_paint = false;
			}
			push_event(orig->on_render, data, size);
			_display();
			break;
		}
		case WM_TIMER: push_event(orig->on_timer, wParam); break;
		case WM_DESTROY:
			for (auto [id, d] : timers) KillTimer(handle, id);
			timers.clear();

			push_event(orig->on_close);
			DeleteDC(image_hdc);
			DeleteObject(image);
			is_open    = false;
			is_created = false;
			handle     = nullptr;
			image      = nullptr;
			image_hdc  = nullptr;
			has_focus  = false;

			PostQuitMessage(0);
			break;

		case WM_MOUSEMOVE:
		{
			auto opos      = mouse.position;
			mouse.position = {GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)};
			for (int i = 0; i < (int)_mouse::_button::button_count; ++i) clicking[i] = false;
			push_event(orig->on_mouse_move, mouse, opos, mouse.position);
			break;
		}
		case WM_MOUSELEAVE:
		{
			vec2i opos = {GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)};
			push_event(orig->on_mouse_leave, mouse, opos, mouse.position);
			track_mouse_hover();
			break;
		}
		case WM_MOUSEHOVER:
		{
			vec2i opos = {GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)};
			push_event(orig->on_mouse_enter, mouse, opos, mouse.position);
			track_mouse_leave();
			break;
		}
		case WM_MOUSEWHEEL:
		{
			double delta = GET_WHEEL_DELTA_WPARAM(wParam) > 0 ? -1 : 1;
			push_event(orig->on_scroll, mouse, delta);
			break;
		}
		case WM_SETFOCUS:
		{
			has_focus = true;
			push_event(orig->on_focus, has_focus);
			break;
		}
		case WM_KILLFOCUS:
		{
			has_focus = false;
			push_event(orig->on_unfocus, has_focus);
			break;
		}
		case WM_LBUTTONDOWN:
		case WM_RBUTTONDOWN:
		case WM_MBUTTONDOWN:
		case WM_XBUTTONDOWN:
		{
			SetCapture(handle);
			auto btn                = btnid();
			mouse.pressed[(int)btn] = true;
			clicking[(int)btn]      = true;
			push_event(orig->on_mouse_down, mouse, btn, true);
			break;
		}
		case WM_LBUTTONUP:
		case WM_RBUTTONUP:
		case WM_MBUTTONUP:
		case WM_XBUTTONUP:
		{
			ReleaseCapture();
			auto btn                = btnid();
			mouse.pressed[(int)btn] = false;
			if (clicking[(int)btn]) push_event(orig->on_click, mouse, btn, false);
			clicking[(int)btn] = false;
			push_event(orig->on_mouse_up, mouse, btn, false);
			break;
		}
		case WM_LBUTTONDBLCLK:
		case WM_RBUTTONDBLCLK:
		case WM_MBUTTONDBLCLK:
		case WM_XBUTTONDBLCLK:
		{
			auto btn = btnid();
			push_event(orig->on_double_click, mouse, btn, true);
			break;
		}

		case WM_KEYDOWN:
		case WM_SYSKEYDOWN:
		{
			auto key                   = MapW2S_keycode(wParam, lParam);
			keyboard.pressed[(int)key] = true;
			push_event(orig->on_key_down, keyboard, key, true, HIWORD(GetKeyState(VK_MENU)) != 0,
			           HIWORD(GetKeyState(VK_CONTROL)) != 0, HIWORD(GetKeyState(VK_SHIFT)) != 0,
			           HIWORD(GetKeyState(VK_LWIN)) || HIWORD(GetKeyState(VK_RWIN)));
		}
		break;
		case WM_KEYUP:
		case WM_SYSKEYUP:
		{
			auto key                   = MapW2S_keycode(wParam, lParam);
			keyboard.pressed[(int)key] = false;
			push_event(orig->on_key_up, keyboard, key, false, HIWORD(GetKeyState(VK_MENU)) != 0,
			           HIWORD(GetKeyState(VK_CONTROL)) != 0, HIWORD(GetKeyState(VK_SHIFT)) != 0,
			           HIWORD(GetKeyState(VK_LWIN)) || HIWORD(GetKeyState(VK_RWIN)));
		}
		break;
		case WM_CHAR:
		{
			push_event(orig->on_text_enter, keyboard, (wchar_t)wParam);
			break;
		}
		case WM_SETCURSOR:
			if (LOWORD(lParam) == HTCLIENT)
			{
				SetCursor(cur_hcursor);
				return true;
			}
			return DefWindowProc(handle, message, wParam, lParam);
		default: return DefWindowProc(handle, message, wParam, lParam);
		}
		return 0;
	}

	bool is_fullscreen = false;
	void set_fullscreen(bool value)
	{
		if (is_fullscreen == value) return;

		if (value)
		{
			SetWindowLongPtr(handle, GWL_STYLE, WS_VISIBLE);
			ShowWindow(handle, SW_MAXIMIZE);
		}
		else
		{
			ShowWindow(handle, SW_SHOWNORMAL);
			SetWindowLongPtr(handle, GWL_STYLE, WS_VISIBLE | WS_OVERLAPPEDWINDOW);
		}
		is_fullscreen = value;
	}

	_cursor cur_cursor  = (_cursor)-1;
	HCURSOR cur_hcursor = LoadCursor(0, IDC_ARROW);
	void    set_cursor(_cursor type)
	{
		if (type == cur_cursor) return;
		HCURSOR cu = 0;
		if (type == _cursor::normal) cu = LoadCursor(0, IDC_ARROW);
		if (type == _cursor::size_all) cu = LoadCursor(0, IDC_SIZEALL);
		if (type == _cursor::hand_point) cu = LoadCursor(0, IDC_HAND);
		if (type == _cursor::size_we) cu = LoadCursor(0, IDC_SIZEWE);
		if (type == _cursor::size_ns) cu = LoadCursor(0, IDC_SIZENS);
		if (type == _cursor::drag) cu = LoadCursor(0, IDC_UPARROW);
		if (cu)
		{
			SetCursor(cu);
			cur_hcursor = cu;
		}
		cur_cursor = type;
	}
};

int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nCmdShow)
{
	sysg::hInstance     = hInstance;
	sysg::hPrevInstance = hPrevInstance;
	sysg::lpCmdLine     = lpCmdLine;
	sysg::nCmdShow      = nCmdShow;

	WNDCLASSEX wcex;
	wcex.cbSize        = sizeof(WNDCLASSEX);
	wcex.style         = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
	wcex.lpfnWndProc   = _window::_impl::wnd_proc;
	wcex.cbClsExtra    = 0;
	wcex.cbWndExtra    = 0;
	wcex.hInstance     = hInstance;
	wcex.hIcon         = LoadIcon(hInstance, MAKEINTRESOURCE(101)); // 101 из resourse.h
	wcex.hCursor       = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = 0;
	wcex.lpszMenuName  = NULL;
	wcex.lpszClassName = szWindowClass;
	wcex.hIconSm       = 0;
	if (!RegisterClassEx(&wcex)) std::exit(2);
	return main();
}

LRESULT _window::_impl::wnd_proc(HWND handle, UINT message, WPARAM wParam, LPARAM lParam)
{
	_impl* win = handle ? reinterpret_cast<_impl*>(GetWindowLongPtr(handle, GWLP_USERDATA)) : nullptr;
	if (win) return win->pool_event(message, wParam, lParam);

	if (message == WM_CLOSE) return 0;
	if ((message == WM_SYSCOMMAND) && (wParam == SC_KEYMENU)) return 0;
	if (handle == 0) return 0;
	return DefWindowProc(handle, message, wParam, lParam);
}

_keyboard::_key MapW2S_keycode(WPARAM key, LPARAM flags)
{
	switch (key)
	{
	case VK_SHIFT:
	{
		static UINT lShift   = MapVirtualKeyW(VK_LSHIFT, MAPVK_VK_TO_VSC);
		UINT        scancode = static_cast<UINT>((flags & (0xFF << 16)) >> 16);
		return scancode == lShift ? _keyboard::_key::shift_l : _keyboard::_key::shift_r;
	}

	case VK_MENU: return (HIWORD(flags) & KF_EXTENDED) ? _keyboard::_key::alt_r : _keyboard::_key::alt_l;
	case VK_CONTROL: return (HIWORD(flags) & KF_EXTENDED) ? _keyboard::_key::control_r : _keyboard::_key::control_l;

	case VK_LWIN: return _keyboard::_key::system_l;
	case VK_RWIN: return _keyboard::_key::system_r;
	case VK_APPS: return _keyboard::_key::menu;
	case VK_OEM_1: return _keyboard::_key::semicolon;
	case VK_OEM_2: return _keyboard::_key::slash;
	case VK_OEM_PLUS: return _keyboard::_key::equal;
	case VK_OEM_MINUS: return _keyboard::_key::minus;
	case VK_OEM_4: return _keyboard::_key::bracket_l;
	case VK_OEM_6: return _keyboard::_key::bracket_r;
	case VK_OEM_COMMA: return _keyboard::_key::comma;
	case VK_OEM_PERIOD: return _keyboard::_key::dot;
	case VK_OEM_7: return _keyboard::_key::quote;
	case VK_OEM_5: return _keyboard::_key::backslash;
	case VK_OEM_3: return _keyboard::_key::tilde;
	case VK_ESCAPE: return _keyboard::_key::escape;
	case VK_SPACE: return _keyboard::_key::space;
	case VK_RETURN: return _keyboard::_key::enter;
	case VK_BACK: return _keyboard::_key::backspace;
	case VK_TAB: return _keyboard::_key::tab;
	case VK_PRIOR: return _keyboard::_key::page_up;
	case VK_NEXT: return _keyboard::_key::page_down;
	case VK_END: return _keyboard::_key::end;
	case VK_HOME: return _keyboard::_key::home;
	case VK_INSERT: return _keyboard::_key::insert;
	case VK_DELETE: return _keyboard::_key::del;
	case VK_ADD: return _keyboard::_key::add;
	case VK_SUBTRACT: return _keyboard::_key::subtract;
	case VK_MULTIPLY: return _keyboard::_key::multiply;
	case VK_DIVIDE: return _keyboard::_key::divide;
	case VK_PAUSE: return _keyboard::_key::pause;
	case VK_F1: return _keyboard::_key::f1;
	case VK_F2: return _keyboard::_key::f2;
	case VK_F3: return _keyboard::_key::f3;
	case VK_F4: return _keyboard::_key::f4;
	case VK_F5: return _keyboard::_key::f5;
	case VK_F6: return _keyboard::_key::f6;
	case VK_F7: return _keyboard::_key::f7;
	case VK_F8: return _keyboard::_key::f8;
	case VK_F9: return _keyboard::_key::f9;
	case VK_F10: return _keyboard::_key::f10;
	case VK_F11: return _keyboard::_key::f11;
	case VK_F12: return _keyboard::_key::f12;
	case VK_F13: return _keyboard::_key::f13;
	case VK_F14: return _keyboard::_key::f14;
	case VK_F15: return _keyboard::_key::f15;
	case VK_LEFT: return _keyboard::_key::left;
	case VK_RIGHT: return _keyboard::_key::right;
	case VK_UP: return _keyboard::_key::up;
	case VK_DOWN: return _keyboard::_key::down;
	case VK_NUMPAD0: return _keyboard::_key::numpad0;
	case VK_NUMPAD1: return _keyboard::_key::numpad1;
	case VK_NUMPAD2: return _keyboard::_key::numpad2;
	case VK_NUMPAD3: return _keyboard::_key::numpad3;
	case VK_NUMPAD4: return _keyboard::_key::numpad4;
	case VK_NUMPAD5: return _keyboard::_key::numpad5;
	case VK_NUMPAD6: return _keyboard::_key::numpad6;
	case VK_NUMPAD7: return _keyboard::_key::numpad7;
	case VK_NUMPAD8: return _keyboard::_key::numpad8;
	case VK_NUMPAD9: return _keyboard::_key::numpad9;
	case 'A': return _keyboard::_key::a;
	case 'Z': return _keyboard::_key::z;
	case 'E': return _keyboard::_key::e;
	case 'R': return _keyboard::_key::r;
	case 'T': return _keyboard::_key::t;
	case 'Y': return _keyboard::_key::y;
	case 'U': return _keyboard::_key::u;
	case 'I': return _keyboard::_key::i;
	case 'O': return _keyboard::_key::o;
	case 'P': return _keyboard::_key::p;
	case 'Q': return _keyboard::_key::q;
	case 'S': return _keyboard::_key::s;
	case 'D': return _keyboard::_key::d;
	case 'F': return _keyboard::_key::f;
	case 'G': return _keyboard::_key::g;
	case 'H': return _keyboard::_key::h;
	case 'J': return _keyboard::_key::j;
	case 'K': return _keyboard::_key::k;
	case 'L': return _keyboard::_key::l;
	case 'M': return _keyboard::_key::m;
	case 'W': return _keyboard::_key::w;
	case 'X': return _keyboard::_key::x;
	case 'C': return _keyboard::_key::c;
	case 'V': return _keyboard::_key::v;
	case 'B': return _keyboard::_key::b;
	case 'N': return _keyboard::_key::n;
	case '0': return _keyboard::_key::num0;
	case '1': return _keyboard::_key::num1;
	case '2': return _keyboard::_key::num2;
	case '3': return _keyboard::_key::num3;
	case '4': return _keyboard::_key::num4;
	case '5': return _keyboard::_key::num5;
	case '6': return _keyboard::_key::num6;
	case '7': return _keyboard::_key::num7;
	case '8': return _keyboard::_key::num8;
	case '9': return _keyboard::_key::num9;
	}

	return _keyboard::_key::u;
}

#elif TARGET_SYSTEM_LINUX

_keyboard::_key XKeysumToSysKey(KeySym symbol)
{
	switch (symbol)
	{
	   // clang-format off
	  case XK_Shift_L:      return _keyboard::_key::shift_l;
	  case XK_Shift_R:      return _keyboard::_key::shift_r;
	  case XK_Control_L:    return _keyboard::_key::control_l;
	  case XK_Control_R:    return _keyboard::_key::control_r;
	  case XK_Alt_L:        return _keyboard::_key::alt_l;
	  case XK_Alt_R:        return _keyboard::_key::alt_r;
	  case XK_Super_L:      return _keyboard::_key::system_l;
	  case XK_Super_R:      return _keyboard::_key::system_r;
	  case XK_Menu:         return _keyboard::_key::menu;
	  case XK_Escape:       return _keyboard::_key::escape;
	  case XK_semicolon:    return _keyboard::_key::semicolon;
	  case XK_slash:        return _keyboard::_key::slash;
	  case XK_equal:        return _keyboard::_key::equal;
	  case XK_minus:        return _keyboard::_key::minus;
	  case XK_bracketleft:  return _keyboard::_key::bracket_l;
	  case XK_bracketright: return _keyboard::_key::bracket_r;
	  case XK_comma:        return _keyboard::_key::comma;
	  case XK_period:       return _keyboard::_key::dot;
	  case XK_apostrophe:   return _keyboard::_key::quote;
	  case XK_backslash:    return _keyboard::_key::backslash;
	  case XK_grave:        return _keyboard::_key::tilde;
	  case XK_space:        return _keyboard::_key::space;
	  case XK_Return:       return _keyboard::_key::enter;
	  case XK_KP_Enter:     return _keyboard::_key::enter;
	  case XK_BackSpace:    return _keyboard::_key::backspace;
	  case XK_Tab:          return _keyboard::_key::tab;
	  case XK_Prior:        return _keyboard::_key::page_up;
	  case XK_Next:         return _keyboard::_key::page_down;
	  case XK_End:          return _keyboard::_key::end;
	  case XK_Home:         return _keyboard::_key::home;
	  case XK_Insert:       return _keyboard::_key::insert;
	  case XK_Delete:       return _keyboard::_key::del;
	  case XK_KP_Add:       return _keyboard::_key::add;
	  case XK_KP_Subtract:  return _keyboard::_key::subtract;
	  case XK_KP_Multiply:  return _keyboard::_key::multiply;
	  case XK_KP_Divide:    return _keyboard::_key::divide;
	  case XK_Pause:        return _keyboard::_key::pause;
	  case XK_F1:           return _keyboard::_key::f1;
	  case XK_F2:           return _keyboard::_key::f2;
	  case XK_F3:           return _keyboard::_key::f3;
	  case XK_F4:           return _keyboard::_key::f4;
	  case XK_F5:           return _keyboard::_key::f5;
	  case XK_F6:           return _keyboard::_key::f6;
	  case XK_F7:           return _keyboard::_key::f7;
	  case XK_F8:           return _keyboard::_key::f8;
	  case XK_F9:           return _keyboard::_key::f9;
	  case XK_F10:          return _keyboard::_key::f10;
	  case XK_F11:          return _keyboard::_key::f11;
	  case XK_F12:          return _keyboard::_key::f12;
	  case XK_F13:          return _keyboard::_key::f13;
	  case XK_F14:          return _keyboard::_key::f14;
	  case XK_F15:          return _keyboard::_key::f15;
	  case XK_Left:         return _keyboard::_key::left;
	  case XK_Right:        return _keyboard::_key::right;
	  case XK_Up:           return _keyboard::_key::up;
	  case XK_Down:         return _keyboard::_key::down;
	  case XK_KP_Insert:    return _keyboard::_key::numpad0;
	  case XK_KP_End:       return _keyboard::_key::numpad1;
	  case XK_KP_Down:      return _keyboard::_key::numpad2;
	  case XK_KP_Page_Down: return _keyboard::_key::numpad3;
	  case XK_KP_Left:      return _keyboard::_key::numpad4;
	  case XK_KP_Begin:     return _keyboard::_key::numpad5;
	  case XK_KP_Right:     return _keyboard::_key::numpad6;
	  case XK_KP_Home:      return _keyboard::_key::numpad7;
	  case XK_KP_Up:        return _keyboard::_key::numpad8;
	  case XK_KP_Page_Up:   return _keyboard::_key::numpad9;
	  case XK_a:            return _keyboard::_key::a;
	  case XK_b:            return _keyboard::_key::b;
	  case XK_c:            return _keyboard::_key::c;
	  case XK_d:            return _keyboard::_key::d;
	  case XK_e:            return _keyboard::_key::r;
	  case XK_f:            return _keyboard::_key::f;
	  case XK_g:            return _keyboard::_key::g;
	  case XK_h:            return _keyboard::_key::h;
	  case XK_i:            return _keyboard::_key::i;
	  case XK_j:            return _keyboard::_key::j;
	  case XK_k:            return _keyboard::_key::k;
	  case XK_l:            return _keyboard::_key::l;
	  case XK_m:            return _keyboard::_key::m;
	  case XK_n:            return _keyboard::_key::n;
	  case XK_o:            return _keyboard::_key::o;
	  case XK_p:            return _keyboard::_key::p;
	  case XK_q:            return _keyboard::_key::q;
	  case XK_r:            return _keyboard::_key::r;
	  case XK_s:            return _keyboard::_key::s;
	  case XK_t:            return _keyboard::_key::t;
	  case XK_u:            return _keyboard::_key::u;
	  case XK_v:            return _keyboard::_key::v;
	  case XK_w:            return _keyboard::_key::w;
	  case XK_x:            return _keyboard::_key::x;
	  case XK_y:            return _keyboard::_key::y;
	  case XK_z:            return _keyboard::_key::z;
	  case XK_0:            return _keyboard::_key::num0;
	  case XK_1:            return _keyboard::_key::num1;
	  case XK_2:            return _keyboard::_key::num2;
	  case XK_3:            return _keyboard::_key::num3;
	  case XK_4:            return _keyboard::_key::num4;
	  case XK_5:            return _keyboard::_key::num5;
	  case XK_6:            return _keyboard::_key::num6;
	  case XK_7:            return _keyboard::_key::num7;
	  case XK_8:            return _keyboard::_key::num8;
	  case XK_9:            return _keyboard::_key::num9;
		// clang-format on
	}
	return _keyboard::_key::unknown;
}

::Pixmap XCreatePixmapFromSysImage(_color const* data, vec2i size, ::Window win, int screen)
{
	_color* ddata = (_color*)malloc(sizeof(_color) * size.x * size.y);
	memcpy(ddata, data, sizeof(_color) * size.x * size.y);
	auto      d   = x::open_display();
	::Pixmap  pxm = XCreatePixmap(d, win, size.x, size.y, DefaultDepth(d, screen));
	XGCValues gcv;
	GC        gc = XCreateGC(d, pxm, GCForeground | GCBackground, &gcv);
	auto image   = XCreateImage(d, DefaultVisual(d, screen), DefaultDepth(d, screen), ZPixmap, 0, (char*)ddata, size.x,
	                           size.y, 32, 0);

	XPutImage(d, pxm, gc, image, 0, 0, 0, 0, size.x, size.y);
	XDestroyImage(image);
	XFreeGC(d, gc);
	return pxm;
}

struct _window::_impl
{
	_window* orig;

	Display*       d      = 0;
	int            screen = 0;
	Window         window = 0;
	GC             gc     = 0;
	XGCValues      gcv    = {};
	XImage*        image        = 0;
	Pixmap         iconPxm      = 0;
	Pixmap         icon_maskPxm = 0;
	XdbeBackBuffer swapBuffer   = 0;
	XdbeSwapInfo   swapInfo;
	XIC            input_context = 0;
	XIM            input_method  = 0;

	bool is_fullscreen = false;
	bool is_open       = false;
	bool has_focus     = false;

	vec2i      position;
	_color*&   data;
	vec2i&     size;
	_mouse&    mouse;
	_keyboard& keyboard;

	_impl(_window* orig) : orig(orig), data(orig->data), size(orig->size), mouse(orig->mouse), keyboard(orig->keyboard)
	{
		d      = x::open_display();
		screen = DefaultScreen(d);
	}

	void free()
	{
		if (image) XDestroyImage(image);
		if (gc) XFreeGC(d, gc);
		if (swapBuffer) XdbeDeallocateBackBufferName(d, swapBuffer);
		if (window) XDestroyWindow(d, window);
		if (iconPxm) XFreePixmap(d, iconPxm);
		if (icon_maskPxm) XFreePixmap(d, icon_maskPxm);
		if (input_context) XDestroyIC(input_context);
		if (input_method) XCloseIM(input_method);
		if (cursor) XFreeCursor(d, cursor);
		image         = 0;
		gc            = 0;
		swapBuffer    = 0;
		window        = 0;
		cursor        = 0;
		iconPxm       = 0;
		icon_maskPxm  = 0;
		input_context = 0;
		input_method  = 0;
	}

	void create(int w, int h)
	{
		free();
		size   = {w, h};
		window = XCreateSimpleWindow(d, DefaultRootWindow(d), 0, 0, w, h, 0, 0, BlackPixel(d, screen));
		XSelectInput(d, window,
		             ExposureMask | KeyPressMask | KeyReleaseMask | PointerMotionMask | ButtonPressMask |
		                 ButtonReleaseMask | StructureNotifyMask | EnterWindowMask | LeaveWindowMask);
		XMapWindow(d, window);
		gc = XCreateGC(d, window, GCForeground | GCBackground, &gcv);

		swapBuffer = XdbeAllocateBackBufferName(d, window, XdbeBackground);
		swapInfo   = {.swap_window = window, .swap_action = XdbeBackground};

		XSetWMProtocols(d, window, &x::atom("WM_DELETE_WINDOW", false), 1);

		input_method = XOpenIM(d, 0, 0, 0);
		if (input_method)
		{
			input_context = XCreateIC(input_method, XNClientWindow, window, XNFocusWindow, window, XNInputStyle,
			                          XIMPreeditNothing | XIMStatusNothing, NULL);
		}

		data  = (_color*)malloc(sizeof(_color) * size.x * size.y);
		image = XCreateImage(d, DefaultVisual(d, screen), DefaultDepth(d, screen), ZPixmap, 0, (char*)data, size.x,
		                     size.y, 32, 0);

		is_open       = true;
		has_focus     = true;
		is_fullscreen = false;
	}
	void set_title(std::wstring_view str) { XStoreName(d, window, x::to_utf8(str).data()); }

	void update_geometry()
	{
		{
			uint64  _pass;
			uint64* pass = &_pass;
			uint32  sx, sy;
			XGetGeometry(d, window, (unsigned long*)pass, &position.x, &position.y, &sx, &sy, (uint32*)pass, (uint32*)pass);
			size.x = sx;
			size.y = sy;
		}
		XDestroyImage(image);

		data  = (_color*)malloc(sizeof(_color) * size.x * size.y);
		image = XCreateImage(d, DefaultVisual(d, screen), DefaultDepth(d, screen), ZPixmap, 0, (char*)data, size.x,
		                     size.y, 32, 0);
	}
	void set_fullscreen(bool value)
	{
		if (is_fullscreen == value) return;

		XWindowAttributes xwa;
		XGetWindowAttributes(d, window, &xwa);

		XEvent e;
		e.xclient.type         = ClientMessage;
		e.xclient.message_type = x::atom("_NET_WM_STATE", true);
		e.xclient.display      = d;
		e.xclient.window       = window;
		e.xclient.format       = 32;
		e.xclient.data.l[0]    = 2; // * 2 - переключить, 1 - добавить, 0 - убрать
		e.xclient.data.l[1]    = x::atom("_NET_WM_STATE_FULLSCREEN", false);
		e.xclient.data.l[2]    = 0;
		e.xclient.data.l[3]    = 0;
		e.xclient.data.l[4]    = 0;
		XSendEvent(d, xwa.root, false, SubstructureNotifyMask | SubstructureRedirectMask, &e);

		is_fullscreen = value;
	}

	~_impl()
	{
		free();
		x::close_display();
	}

	void close()
	{
		XEvent event;
		event.xclient.type         = ClientMessage;
		event.xclient.window       = window;
		event.xclient.message_type = x::atom("WM_PROTOCOLS");
		event.xclient.format       = 32;
		event.xclient.data.l[0]    = x::atom("WM_DELETE_WINDOW");
		event.xclient.data.l[1]    = CurrentTime;
		XSendEvent(d, window, False, NoEventMask, &event);
	}

	struct Timer
	{
		_system_time_point next_end;
		_milliseconds duration;
	};

	std::map<uint64, Timer> timers;
	std::vector<int64> timers_lock;
	std::vector<std::pair<int64, Timer>> timers_unlock;
	bool timers_locked = false;

	void add_timer(uint64 id, _milliseconds delay)
	{
		auto end = std::chrono::system_clock::now() + delay;
		if (timers_locked) timers_unlock.emplace_back(id, Timer{end, delay});
		else timers[id] = {end, delay};
	}
	void remove_timer(uint64 id)
	{
		if (timers_locked && std::find(timers_lock.begin(), timers_lock.end(), id) == timers_lock.end()) timers_lock.push_back(id);
		else if (timers.count(id)) timers.erase(id);
	}

	Cursor cursor = 0;
	bool requires_cursor = false;
	_cursor required_cursor = _cursor::normal;

	void load_cursor(_cursor type)
	{
		if (cursor) XFreeCursor(d, cursor);
		switch (type)
		{
		case _cursor::normal:     cursor = XCreateFontCursor(d, XC_left_ptr); break;
		case _cursor::drag:       cursor = XCreateFontCursor(d, XC_center_ptr); break;
		case _cursor::hand_point: cursor = XCreateFontCursor(d, XC_hand1); break;
		case _cursor::size_all:   cursor = XCreateFontCursor(d, XC_fleur); break;
		case _cursor::size_ns:    cursor = XCreateFontCursor(d, XC_sb_v_double_arrow); break;
		case _cursor::size_we:    cursor = XCreateFontCursor(d, XC_sb_h_double_arrow); break;
		}
	}

	void set_cursor(_cursor type)
	{
		if (window)
		{
			load_cursor(type);
			XDefineCursor(d, window, cursor);
			XSync(d, False);
		}
		else
		{
			requires_cursor = true;
			required_cursor = type;
		}
	}

	void set_position(vec2i p) { XMoveWindow(d, window, p.x, p.y); }
	void set_size(vec2i size) { XResizeWindow(d, window, size.x, size.y); }

	template <class _f, class... _p>
	void push_event(_f& f, _p... p)
	{
		if (f) f({*orig, p...});
	}

	bool clicking[(int)_mouse::_button::button_count] = {};
	_system_time_point last_click_time = _system_time_point::max();

	int event_loop()
	{
		if (!window) return -1;
		static XEvent ev;
		static auto   xbtnget = []() -> _mouse::_button {
		      if (ev.xbutton.button == 1) return _mouse::_button::left;
		      if (ev.xbutton.button == 2) return _mouse::_button::middle;
		      if (ev.xbutton.button == 3) return _mouse::_button::right;
		      if (ev.xbutton.button == 8) return _mouse::_button::backward;
		      if (ev.xbutton.button == 9) return _mouse::_button::forward;
		      return _mouse::_button::left;
	   };
		auto xbtnscroll = []() -> std::pair<bool, int> {
			if (ev.xbutton.button == 4 || ev.xbutton.button == 5)
				return {true, ev.xbutton.button == 5 ? 1 : -1};
			else if (ev.xbutton.button == 6 || ev.xbutton.button == 7)
				return {true, 0};
			return {false, 0};
		};

		if (requires_cursor)
		{
			load_cursor(required_cursor);
			XDefineCursor(d, window, cursor);
			XSync(d, False);
		}
		requires_cursor = false;

		orig->last_tick_time = std::chrono::system_clock::now();
		bool catched         = false;
		while (is_open)
		{
			auto checkEvent = [](Display*, XEvent* event, XPointer userData) -> Bool {
				return event->xany.window == (::Window)(userData);
			};
			while (XCheckIfEvent(d, &ev, checkEvent, (XPointer)window))
			{
				catched = true;
				switch (ev.type)
				{
				case Expose:
					if (ev.xexpose.width != size.x || ev.xexpose.height != size.y)
					{
						auto osize = size;
						update_geometry();
						push_event(orig->on_resize, osize, size);
					}
					display();
					break;

				case MotionNotify:
				{
					auto opos      = mouse.position;
					mouse.position = {ev.xmotion.x, ev.xmotion.y};
					for (int i = 0; i < (int)_mouse::_button::button_count; ++i) clicking[i] = false;
					push_event(orig->on_mouse_move, mouse, opos, mouse.position);
				}
				break;

				case ButtonPress:
				{
					auto [is_scroll, delta] = xbtnscroll();
					if (!is_scroll)
					{
						auto btn                = xbtnget();
						mouse.pressed[(int)btn] = true;
						clicking[(int)btn]      = true;
						push_event(orig->on_mouse_down, mouse, btn, true);
					}
					else if (delta != 0)
					{
						push_event(orig->on_scroll, mouse, (double)delta);
					}
				}
				break;

				case ButtonRelease:
				{
					auto [is_scroll, delta] = xbtnscroll();
					if (!is_scroll)
					{
						auto now = std::chrono::system_clock::now();
						auto btn                = xbtnget();
						mouse.pressed[(int)btn] = false;
						if (clicking[(int)btn])
						{
							if (last_click_time < now && now - last_click_time < 200ms) push_event(orig->on_double_click, mouse, btn, true);
							else push_event(orig->on_click, mouse, btn, false);
						}
						clicking[(int)btn] = false;
						last_click_time = now;
						push_event(orig->on_mouse_up, mouse, btn, false);
					}
				}
				break;

				case LeaveNotify:
				{
					push_event(orig->on_mouse_leave, mouse, mouse.position, vec2i{ev.xcrossing.x, ev.xcrossing.y});
				}
				case EnterNotify:
				{
					push_event(orig->on_mouse_enter, mouse, mouse.position, vec2i{ev.xcrossing.x, ev.xcrossing.y});
				}

				case FocusIn:
				{
					has_focus = true;
					if (input_context) XSetICFocus(input_context);
					push_event(orig->on_focus, has_focus);
				}
				break;

				case FocusOut:
				{
					has_focus = false;
					if (input_context) XUnsetICFocus(input_context);
					push_event(orig->on_focus, has_focus);
				}
				break;

				case KeyPress:
				{
					_keyboard::_key key = _keyboard::_key::unknown;
					for (int i = 0; i < 4 && key == _keyboard::_key::unknown; ++i)
						key = XKeysumToSysKey(XLookupKeysym(&ev.xkey, i));
					keyboard.pressed[(int)key] = true;

					auto msk = [](int64_t v) -> bool { return ev.xkey.state & v; };
					push_event(orig->on_key_down, keyboard, key, true, msk(Mod1Mask), msk(ControlMask), msk(ShiftMask),
					           msk(Mod4Mask));

					if (input_context)
					{
						Status status;
						char   buffer[16];
						int    length = Xutf8LookupString(input_context, &ev.xkey, buffer, sizeof(buffer), nullptr, &status);

						if (length > 0)
						{
							std::wstring s = x::from_utf8(std::string(buffer, length));
							for (auto v : s)
								push_event(orig->on_text_enter, keyboard, v);
						}
					}
				}
				break;

				case KeyRelease:
				{
					_keyboard::_key key = _keyboard::_key::unknown;
					for (int i = 0; i < 4 && key == _keyboard::_key::unknown; ++i)
						key = XKeysumToSysKey(XLookupKeysym(&ev.xkey, i));
					keyboard.pressed[(int)key] = false;

					auto msk = [](int64_t v) -> bool { return ev.xkey.state & v; };
					push_event(orig->on_key_up, keyboard, key, false, msk(Mod1Mask), msk(ControlMask), msk(ShiftMask),
					           msk(Mod4Mask));
				}
				break;

				case ConfigureNotify:
				{
					if (ev.xconfigure.x != position.x || ev.xconfigure.y != position.y)
					{
						auto old_pos = position;
						position = {ev.xconfigure.x, ev.xconfigure.y};
						push_event(orig->on_move, old_pos, position);
					}
				}
				break;

				case ClientMessage:
					if (ev.xclient.data.l[0] == (int64)x::atom("WM_DELETE_WINDOW", false))
					{
						is_open         = false;
						is_fullscreen   = false;
						has_focus       = false;
						wait_for_render = false;
						push_event(orig->on_close);
					}
					break;
				}
			}
			auto now             = std::chrono::system_clock::now();
			auto delta_time      = std::chrono::duration_cast<std::chrono::milliseconds>(now - orig->last_tick_time);
			orig->last_tick_time = now;

			// ! не тестировал
			timers_locked = true;
			for (auto& [id, t] : timers) {
				while (now >= t.next_end) {
					auto p = std::find(timers_lock.begin(), timers_lock.end(), id);
					if (p != timers_lock.end()) break;
					auto id2 = id;
					auto p2 = std::find_if(timers_unlock.begin(), timers_unlock.end(), [id2](auto& v){ return v.first == id2; });
					if (p2 != timers_unlock.end())
					{
						t = p2->second;
						timers_unlock.erase(p2);
						if (now < t.next_end) break;
					}
					push_event(orig->on_timer, id);
					t.next_end += t.duration;
				}
			}
			for (auto id : timers_lock)
				timers.erase(id);
			for (auto& [id, v] : timers_unlock)
				timers[id] = v;
			timers_lock.clear();
			timers_unlock.clear();
			timers_locked = false;

			push_event(orig->on_tick, mouse, keyboard, delta_time);

			if (!catched) std::this_thread::sleep_for(2ms); // не так быстро!
			if (wait_for_render)
			{
				wait_for_render = false;
				push_event(orig->on_render, orig->data, orig->size);
				_display();
			};
		}
		wait_for_render = true;
		free();
		return 0;
	}

	void _display()
	{
		XPutImage(d, swapBuffer, gc, image, 0, 0, 0, 0, size.x, size.y);
		XdbeSwapBuffers(d, &swapInfo, 1);
	}

	bool wait_for_render = true;

	void display() { wait_for_render = true; }

	void set_icon(_color const* data, vec2i size)
	{
		if (iconPxm != 0) XFreePixmap(d, iconPxm);
		if (icon_maskPxm != 0) XFreePixmap(d, icon_maskPxm);

		_color* ddata = new _color[size.x * size.y];
		std::transform(data, data + size.x * size.y, ddata,
		               [](auto&& c) { return c.a > 127 ? _color(0, 0, 0) : _color(255, 255, 255); });

		iconPxm      = XCreatePixmapFromSysImage(data, size, window, screen);
		icon_maskPxm = XCreatePixmapFromSysImage(ddata, size, window, screen);

		auto wmh         = XAllocWMHints();
		wmh->flags       = IconPixmapHint | IconMaskHint;
		wmh->icon_pixmap = iconPxm;
		wmh->icon_mask   = icon_maskPxm;
		XSetWMHints(d, window, wmh);
		XFree(wmh);
	}
	void clear_icon()
	{
		if (iconPxm != 0) XFreePixmap(d, iconPxm);
		if (icon_maskPxm != 0) XFreePixmap(d, icon_maskPxm);
		iconPxm          = 0;
		icon_maskPxm     = 0;
		auto wmh         = XAllocWMHints();
		wmh->flags       = IconPixmapHint | IconMaskHint;
		wmh->icon_pixmap = iconPxm;
		wmh->icon_mask   = icon_maskPxm;
		XSetWMHints(d, window, wmh);
		XFree(wmh);
	}
};

vec2i _screen::get_size()
{
	auto d      = x::open_display();
	auto screen = XScreenOfDisplay(d, XDefaultScreen(d));
	return {screen->width, screen->height};
	x::close_display();
}

#endif

_window::_window() : data(nullptr), size(0, 0), impl(new _impl(this)) {}
_window::_window(int w, int h) : _window() { create(w, h); }
_window::_window(int w, int h, std::wstring_view title) : _window(w, h) { set_title(title); }

_window::~_window() { delete impl; }
void _window::create(int w, int h) { impl->create(w, h); }
void _window::set_title(std::wstring_view value) { impl->set_title(value); }
void _window::close() { impl->close(); }
void _window::add_timer(uint64 id, _milliseconds delay) { impl->add_timer(id, delay); }
void _window::remove_timer(uint64 id) { impl->remove_timer(id); }
void _window::display() { impl->display(); }
int  _window::event_loop() { return impl->event_loop(); }

bool _window::has_focus() { return impl->has_focus; }
void _window::set_fullscreen(bool value) { impl->set_fullscreen(value); }
bool _window::is_fullscreen() { return impl->is_fullscreen; }

void _window::set_cursor(_cursor type) { impl->set_cursor(type); }
void _window::set_icon(_color const* data, vec2i size) { impl->set_icon(data, size); }
void _window::set_icon(_picture const& pic) { impl->set_icon((_color const*)pic.data, size); }
void _window::clear_icon() { impl->clear_icon(); }

void _window::set_position(vec2i p) { impl->set_position(p); }
void _window::set_size(vec2i size) { impl->set_size(size); }

#if TARGET_SYSTEM_WINDOWS

vec2i _screen::get_size()
{
	RECT r;
	HWND screen = GetDesktopWindow();
	GetWindowRect(screen, &r);
	return {r.right, r.bottom};
}

#elif TARGET_SYSTEM_LINUX

#endif
