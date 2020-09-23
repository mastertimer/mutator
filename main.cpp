#include <filesystem>

#include "mutator.h"
#include "mult.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

constexpr wchar_t tetfile[] = L"..\\..\\tetrons.txt";
HCURSOR g_cu = LoadCursor(0, IDC_ARROW); // активный курсор
bool view_mult = false; // режим анимации

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void set_cursorx(_cursor x)
{
	if (x == g_cursor) return;
	HCURSOR cu = 0;
	if (x == _cursor::normal)     cu = LoadCursor(0, IDC_ARROW);
	if (x == _cursor::size_all)   cu = LoadCursor(0, IDC_SIZEALL);
	if (x == _cursor::hand_point) cu = LoadCursor(0, IDC_HAND);
	if (x == _cursor::size_we)    cu = LoadCursor(0, IDC_SIZEWE);
	if (x == _cursor::size_ns)    cu = LoadCursor(0, IDC_SIZENS);
	if (x == _cursor::drag)       cu = LoadCursor(0, IDC_UPARROW);
	if (cu)
	{
		SetCursor(cu);
		g_cu = cu;
	}
	g_cursor = x;
}

void change_window_text(HWND hwnd)
{
	static std::wstring s_old;
	wchar_t             s[100];
	swprintf(s, 100, L"%d  %4.1e", all_tetron.size, mutator::get_main_scale());
	if (s_old == s) return;
	s_old = s;
	SetWindowText(hwnd, s);
}

void paint(HWND hwnd)
{
	if (view_mult) return;
	change_window_text(hwnd);
	HDC hdc = GetDC(hwnd);
	RECT rect;
	GetClientRect(hwnd, &rect);
	mutator::draw({ rect.right, rect.bottom });
	BitBlt(hdc, 0, 0, rect.right, rect.bottom, master_bm.hdc, 0, 0, SRCCOPY);
	ReleaseDC(hwnd, hdc);
}

void paint_mult(HWND hwnd)
{
	static auto t = (std::chrono::high_resolution_clock::now() - std::chrono::high_resolution_clock::now()) * 0;
	if (!view_mult) return;
	auto t0 = std::chrono::high_resolution_clock::now();
	HDC hdc = GetDC(hwnd);
	RECT rect;
	GetClientRect(hwnd, &rect);
	_bitmap* bm = draw_mult();
	bm->text16(0, 0, std::to_string(t.count() / 1000000), c_max);
	BitBlt(hdc, 0, 0, rect.right, rect.bottom, bm->hdc, 0, 0, SRCCOPY);
	ReleaseDC(hwnd, hdc);
	t = std::chrono::high_resolution_clock::now() - t0;
}

void init_shift(WPARAM wparam)
{
	*n_s_shift ->operator i64* () = wparam & MK_SHIFT;
	*n_s_alt   ->operator i64* () = false;
	*n_s_ctrl  ->operator i64* () = wparam & MK_CONTROL;
	*n_s_left  ->operator i64* () = wparam & MK_LBUTTON;
	*n_s_right ->operator i64* () = wparam & MK_RBUTTON;
	*n_s_middle->operator i64* () = wparam & MK_MBUTTON;
	*n_s_double->operator i64* () = false;
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
			SetCursor(g_cu);
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
		if (!master_obl_izm.empty()) paint(hWnd);
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
		if (!master_obl_izm.empty()) paint(hWnd);
		return 0;
	case WM_LBUTTONDOWN: case WM_RBUTTONDOWN: case WM_MBUTTONDOWN:
		init_shift(wParam);
		if (message == WM_LBUTTONDOWN) mutator::mouse_button_left(true);
		if (message == WM_RBUTTONDOWN) mutator::mouse_button_right(true);
		if (message == WM_MBUTTONDOWN) mutator::mouse_button_middle(true);
		if (!master_obl_izm.empty()) paint(hWnd);
		return 0;
	case WM_LBUTTONUP: case WM_RBUTTONUP: case WM_MBUTTONUP:
		init_shift(wParam);
		if (message == WM_LBUTTONUP) mutator::mouse_button_left(false);
		if (message == WM_RBUTTONUP) mutator::mouse_button_right(false);
		if (message == WM_MBUTTONUP) mutator::mouse_button_middle(false);
		if (!master_obl_izm.empty()) paint(hWnd);
		return 0;
	case WM_LBUTTONDBLCLK: case WM_RBUTTONDBLCLK: case WM_MBUTTONDBLCLK:
		init_shift(wParam);
		*n_s_double->operator i64* () = true;
		if (message == WM_LBUTTONDBLCLK) n_down_left->run(0, n_down_left, flag_run);
		if (message == WM_RBUTTONDBLCLK) n_down_right->run(0, n_down_right, flag_run);
		if (message == WM_MBUTTONDBLCLK) n_down_middle->run(0, n_down_middle, flag_run);
		if (!master_obl_izm.empty()) paint(hWnd);
		return 0;
	case WM_KEYDOWN:
		if (wParam == VK_F1)
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
			return 0;
		}
		if (wParam == VK_F2)
		{
			run_timer = false;
			int r = MessageBox(hWnd, L"сохранить?", L"предупреждение", MB_YESNO);
			if (r == IDYES) mutator::save_to_txt_file((exe_path + tetfile).c_str());
			run_timer = true;
			return 0;
		}
		if (wParam == VK_F3)
		{
			view_mult = !view_mult;
			return 0;
		}
		//		InitShift(Shift);
		*n_down_key->operator i64* () = wParam;
		n_down_key->run(0, n_down_key, flag_run);
		if (!master_obl_izm.empty()) paint(hWnd);
		return 0;
	case WM_CHAR:
		*n_press_key->operator i64* () = wParam;
		n_press_key->run(0, n_press_key, flag_run);
		if (!master_obl_izm.empty()) paint(hWnd);
		return 0;
	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		BeginPaint(hWnd, &ps);
		paint(hWnd);
		EndPaint(hWnd, &ps);
	}
	return 0;
	case WM_TIMER:
		if (!run_timer) return 0;
		switch (wParam)
		{
		case 1:
			n_timer1000->run(0, n_timer1000, flag_run);
			if (!master_obl_izm.empty()) paint(hWnd);
			break;
		case 2:
			n_timer250->run(0, n_timer250, flag_run);
			break;
		case 3:
			paint_mult(hWnd);
			break;
		}
		return 0;
	case WM_CREATE:
		SetTimer(hWnd, 1, 1000, 0); // общий с отрисовкой
		SetTimer(hWnd, 2, 250, 0); // более быстрый, без отрисовки
		SetTimer(hWnd, 3, 50, 0); // для анимации
		return 0;
	case WM_DESTROY:
		KillTimer(hWnd, 1);
		KillTimer(hWnd, 2);
		KillTimer(hWnd, 3);
		PostQuitMessage(0);
		return 0;
	}
	return DefWindowProc(hWnd, message, wParam, lParam);
}

int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nCmdShow)
{
	wchar_t buffer[MAX_PATH];
	GetModuleFileName(hInstance, buffer, MAX_PATH);
	std::filesystem::path fn = buffer;
	fn.remove_filename();
	exe_path = fn;
	if (!mutator::start((exe_path + tetfile).c_str())) return 1;

	static TCHAR szWindowClass[] = L"win64app";
	WNDCLASSEX wcex;
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(101)); // 101 из resourse.h
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = 0;
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = szWindowClass;
	wcex.hIconSm = 0;
	if (!RegisterClassEx(&wcex)) return 2;
	HWND hWnd = CreateWindow(szWindowClass, L"mutator", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, 1200, 800,
		NULL, NULL, hInstance, NULL);
	if (!hWnd) return 3;
	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);
	set_cursorx((*n_perenos->operator i64 * ()) ? _cursor::size_all : _cursor::normal);
	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return (int)msg.wParam;
}
