#include "main.h"

#include "win_basic.h"
#include "json.h"
#include "resource.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace
{
	constexpr wchar_t sz_window_class[] = L"MUTATOR";
	constexpr wchar_t settings_filemane[] = L"..\\..\\settings.json";
	std::wstring mode_name = L"mutator";
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool _mode::start()
{
	if (!started) started = start2();
	return started;
}

bool _mode::stop()
{
	if (!started) return true;
	started = !stop2();
	return !started;
}

_mode::~_mode()
{
	stop();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void load_settings()
{
	_rjson fs(exe_path / settings_filemane);
	if (fs.error) return;
	fs.read("mode", mode_name);
}

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
	auto& mode = main_modes[mode_name];
	auto area = mode->draw({ rect.right, rect.bottom });
	if (all) area = _isize(rect.right, rect.bottom);
	if (!area.empty()) BitBlt(hdc, int(area.x.min), int(area.y.min), int(area.x.length()), int(area.y.length()),
		mode->get_bitmap().hdc, int(area.x.min), int(area.y.min), SRCCOPY); // съедаются границы при переносе
//	if (!area.empty()) BitBlt(hdc, 0, 0, rect.right, rect.bottom, mode->get_bitmap().hdc, 0, 0, SRCCOPY); // а так норм
	ReleaseDC(hwnd, hdc);
}

void init_shift(WPARAM wparam)
{
	keyboard.ctrl_key = wparam & MK_CONTROL;
	keyboard.shift_key = wparam & MK_SHIFT;
	mouse.left_button = wparam & MK_LBUTTON;
	mouse.right_button = wparam & MK_RBUTTON;
	mouse.middle_button = wparam & MK_MBUTTON;
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
		mouse.position = { ((i64)(short)LOWORD(lParam)), ((i64)(short)HIWORD(lParam)) };
		main_modes[mode_name]->mouse_move();
		mouse.prev_position = mouse.position;
		paint(hWnd);
		return 0;
	case WM_MOUSELEAVE:
		tracking_mouse = false;
		main_modes[mode_name]->mouse_leave();
		return 0;
	case WM_MOUSEWHEEL:
		init_shift(GET_KEYSTATE_WPARAM(wParam));
		main_modes[mode_name]->mouse_wheel(GET_WHEEL_DELTA_WPARAM(wParam) / WHEEL_DELTA);
		paint(hWnd);
		return 0;
	case WM_LBUTTONDOWN: case WM_RBUTTONDOWN: case WM_MBUTTONDOWN:
		init_shift(wParam);
		if (message == WM_LBUTTONDOWN) main_modes[mode_name]->mouse_button_left(true);
		if (message == WM_RBUTTONDOWN) main_modes[mode_name]->mouse_button_right(true);
		if (message == WM_MBUTTONDOWN) main_modes[mode_name]->mouse_button_middle(true);
		paint(hWnd);
		return 0;
	case WM_LBUTTONUP: case WM_RBUTTONUP: case WM_MBUTTONUP:
		init_shift(wParam);
		if (message == WM_LBUTTONUP) main_modes[mode_name]->mouse_button_left(false);
		if (message == WM_RBUTTONUP) main_modes[mode_name]->mouse_button_right(false);
		if (message == WM_MBUTTONUP) main_modes[mode_name]->mouse_button_middle(false);
		paint(hWnd);
		return 0;
	case WM_LBUTTONDBLCLK: case WM_RBUTTONDBLCLK: case WM_MBUTTONDBLCLK:
		init_shift(wParam);
		if (message == WM_LBUTTONDBLCLK) main_modes[mode_name]->mouse_button_left(true, true);
		if (message == WM_RBUTTONDBLCLK) main_modes[mode_name]->mouse_button_right(true, true);
		if (message == WM_MBUTTONDBLCLK) main_modes[mode_name]->mouse_button_middle(true, true);
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
			[[fallthrough]];
		default:
			main_modes[mode_name]->key_down(wParam);
			paint(hWnd);
		}
		return 0;
	case WM_KEYUP:
		if (wParam == VK_CONTROL) keyboard.ctrl_key = false;
		main_modes[mode_name]->key_up(wParam);
		return 0;
	case WM_CHAR:
		main_modes[mode_name]->key_press(wParam);
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
		main_modes[mode_name]->resize({ LOWORD(lParam), HIWORD(lParam) });
		return 0;
	case WM_TIMER:
		if (!run_timer) return 0;
		switch (wParam)
		{
		case 1:
			main_modes[mode_name]->timer1000();
			paint(hWnd);
			change_window_text(hWnd);
			break;
		case 2:
			main_modes[mode_name]->timer250();
			break;
		}
		return 0;
	case WM_CREATE:
		SetTimer(hWnd, 1, 1000, 0); // общий с отрисовкой
		SetTimer(hWnd, 2, 250, 0); // более быстрый, без отрисовки
		return 0;
	case WM_DESTROY:
		main_modes[mode_name]->destroy();
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
    wcex.lpszClassName  = sz_window_class;
    wcex.hIconSm        = 0;

    return RegisterClassExW(&wcex);
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   HWND hWnd = CreateWindowW(sz_window_class, L"mutator", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, 
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
	load_settings();
	if (!main_modes[mode_name]) return 1;
    if (!main_modes[mode_name]->start()) return 2;

    MyRegisterClass(hInstance);

    if (!InitInstance(hInstance, nCmdShow))
    {
        return FALSE;
    }
//    set_cursor((*n_perenos->operator i64 * ()) ? _cursor::size_all : _cursor::normal);

    MSG msg;
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return (int)msg.wParam;
}
