﻿#include "main.h"

#include "tetron.h"
#include "mutator.h"
#include "win_basic.h"
#include "resource.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace
{
    WCHAR szWindowClass[] = L"MUTATOR";

	std::string mode_name = "mutator";
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool _mode::start()
{
	if (!started) started = start2();
	return started;
}

bool _mode::start2()
{
	return false;
}

bool _mode::stop()
{
	if (!started) return true;
	started = !stop2();
	return !started;
}

bool _mode::stop2()
{
	return false;
}

bool _mode::save()
{
	return false;
}

void _mode::key_down(u64 key)
{
}

_mode::~_mode()
{
	stop();
}

std::wstring _mode::get_window_text()
{
	return {};
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void change_window_text(HWND hwnd)
{
	static std::wstring s_old;
	auto s = main_modes[mode_name]->get_window_text();
	if (s_old == s) return;
	s_old = s;
	SetWindowText(hwnd, s.c_str());
}

void paint(HWND hwnd, bool all = false)
{
	HDC hdc = GetDC(hwnd);
	RECT rect;
	GetClientRect(hwnd, &rect);
	auto area = _mutator::draw({ rect.right, rect.bottom });
	if (all) area = _isize(rect.right, rect.bottom);
	if (!area.empty()) BitBlt(hdc, int(area.x.min), int(area.y.min), int(area.x.length()), int(area.y.length()),
		master_bm.hdc, int(area.x.min), int(area.y.min), SRCCOPY);
	ReleaseDC(hwnd, hdc);
}

void init_shift(WPARAM wparam) // !!! сделать по аналогии c ctrl
{
	*n_s_shift ->operator i64* () = wparam & MK_SHIFT;
	keyboard.ctrl_key = wparam & MK_CONTROL;
	*n_s_ctrl  ->operator i64* () = wparam & MK_CONTROL;
	*n_s_left  ->operator i64* () = wparam & MK_LBUTTON;
	*n_s_right ->operator i64* () = wparam & MK_RBUTTON;
	*n_s_middle->operator i64* () = wparam & MK_MBUTTON;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	static bool run_timer = true;
	static bool tracking_mouse = false;
	switch (message)
	{
	case WM_SETCURSOR:
		if (LOWORD(lParam) == HTCLIENT)
		{
			SetCursor(cursors[u64(g_cursor)]);
			return true;
		}
		break;
	case WM_MOUSEMOVE:
		if (!tracking_mouse)
		{
			tracking_mouse = true;
			TRACKMOUSEEVENT a;
			a.cbSize = sizeof(a);
			a.dwFlags = TME_LEAVE;
			a.dwHoverTime = 0;
			a.hwndTrack = hWnd;
			TrackMouseEvent(&a);
		}
		init_shift(wParam);
		mouse_xy = { ((double)(short)LOWORD(lParam)), ((double)(short)HIWORD(lParam)) };
		n_move->run(0, n_move, flag_run);
		paint(hWnd);
		return 0;
	case WM_MOUSELEAVE:
		tracking_mouse = false;
		if (*n_s_left->operator i64 * ())
		{
			*n_s_left->operator i64* () = 0;
			n_up_left->run(0, n_up_left, flag_run);
		}
		if (*n_s_right->operator i64 * ())
		{
			*n_s_right->operator i64* () = 0;
			n_up_right->run(0, n_up_right, flag_run);
		}
		if (*n_s_middle->operator i64 * ())
		{
			*n_s_middle->operator i64* () = 0;
			n_up_middle->run(0, n_up_middle, flag_run);
		}
		return 0;
	case WM_MOUSEWHEEL:
		init_shift(GET_KEYSTATE_WPARAM(wParam));
		*n_wheel->operator i64* () = GET_WHEEL_DELTA_WPARAM(wParam) / WHEEL_DELTA;
		n_wheel->run(0, n_wheel, flag_run);
		paint(hWnd);
		return 0;
	case WM_LBUTTONDOWN: case WM_RBUTTONDOWN: case WM_MBUTTONDOWN:
		init_shift(wParam);
		*n_s_double->operator i64* () = false;
		if (message == WM_LBUTTONDOWN) _mutator::mouse_button_left(true);
		if (message == WM_RBUTTONDOWN) _mutator::mouse_button_right(true);
		if (message == WM_MBUTTONDOWN) _mutator::mouse_button_middle(true);
		paint(hWnd);
		return 0;
	case WM_LBUTTONUP: case WM_RBUTTONUP: case WM_MBUTTONUP:
		init_shift(wParam);
		*n_s_double->operator i64* () = false;
		if (message == WM_LBUTTONUP) _mutator::mouse_button_left(false);
		if (message == WM_RBUTTONUP) _mutator::mouse_button_right(false);
		if (message == WM_MBUTTONUP) _mutator::mouse_button_middle(false);
		paint(hWnd);
		return 0;
	case WM_LBUTTONDBLCLK: case WM_RBUTTONDBLCLK: case WM_MBUTTONDBLCLK:
		init_shift(wParam);
		*n_s_double->operator i64* () = true;
		if (message == WM_LBUTTONDBLCLK) n_down_left->run(0, n_down_left, flag_run);
		if (message == WM_RBUTTONDBLCLK) n_down_right->run(0, n_down_right, flag_run);
		if (message == WM_MBUTTONDBLCLK) n_down_middle->run(0, n_down_middle, flag_run);
		paint(hWnd);
		return 0;
	case WM_KEYDOWN:
		switch (wParam)
		{
		case VK_F1:
		{
			static bool norm = true;
			if (norm)
			{
				SetWindowLongPtr(hWnd, GWL_STYLE, WS_VISIBLE);
				ShowWindow(hWnd, SW_MAXIMIZE);
			}
			else
			{
				ShowWindow(hWnd, SW_SHOWNORMAL);
				SetWindowLongPtr(hWnd, GWL_STYLE, WS_VISIBLE | WS_OVERLAPPEDWINDOW);
			}
			norm = !norm;
			break;
		}
		case VK_F2:
		{
			run_timer = false;
			int r = MessageBox(hWnd, L"сохранить?", L"предупреждение", MB_YESNO);
			if (r == IDYES) main_modes[mode_name]->save();
			run_timer = true;
			break;
		}
		case VK_CONTROL:
			keyboard.ctrl_key = true;
		default:
			main_modes[mode_name]->key_down(wParam);
			paint(hWnd);
		}
		return 0;
	case WM_KEYUP:
		if (wParam == VK_CONTROL)
		{
			keyboard.ctrl_key = false;
			*n_s_ctrl->operator i64* () = 0;
		}
		return 0;
	case WM_CHAR:
		*n_press_key->operator i64* () = wParam;
		n_press_key->run(0, n_press_key, flag_run);
		paint(hWnd);
		return 0;
	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		BeginPaint(hWnd, &ps);
		paint(hWnd, true);
		EndPaint(hWnd, &ps);
		return 0;
	}
	case WM_SIZE:
		_mutator::resize({ LOWORD(lParam), HIWORD(lParam) });
		return 0;
	case WM_TIMER:
		if (!run_timer) return 0;
		switch (wParam)
		{
		case 1:
			n_timer1000->run(0, n_timer1000, flag_run);
			paint(hWnd);
			change_window_text(hWnd);
			break;
		case 2:
			n_timer250->run(0, n_timer250, flag_run);
			break;
		}
		return 0;
	case WM_CREATE:
		SetTimer(hWnd, 1, 1000, 0); // общий с отрисовкой
		SetTimer(hWnd, 2, 250, 0); // более быстрый, без отрисовки
		return 0;
	case WM_DESTROY:
		run_before_del_link = false; // разобраться почему без этого - ошибка
		KillTimer(hWnd, 1);
		KillTimer(hWnd, 2);
		PostQuitMessage(0);
		return 0;
	}
	return DefWindowProc(hWnd, message, wParam, lParam);
}

ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_MUTATOR));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = CreateSolidBrush(RGB(0, 0, 0));
    wcex.lpszMenuName   = 0;
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = 0;

    return RegisterClassExW(&wcex);
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   HWND hWnd = CreateWindowW(szWindowClass, L"mutator", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, 
       nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow)
{
    exe_path = get_exe_path(hInstance);
    if (!main_modes[mode_name]->start()) return 1;

    MyRegisterClass(hInstance);

    if (!InitInstance(hInstance, nCmdShow))
    {
        return FALSE;
    }
    set_cursor((*n_perenos->operator i64 * ()) ? _cursor::size_all : _cursor::normal);

    MSG msg;
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return (int)msg.wParam;
}
