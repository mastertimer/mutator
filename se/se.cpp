﻿#include "ui.h"
#include "win_basic.h"
#include "e_terminal.h"
#include "e_exchange_graph.h"
#include "exchange_data.h"
#include "exchange_trade.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

_ui ui;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void change_window_text(HWND hwnd)
{
/*	static std::wstring s_old;
	wchar_t s[100];
	swprintf(s, 100, L"%d  %4.1e", all_tetron.size, mutator::get_main_scale());
	if (s_old == s) return;
	s_old = s;
	SetWindowText(hwnd, s);*/
}

void init_shift(WPARAM wparam) // !!! сделать по аналогии c ctrl
{
	ui.n_s_shift = wparam & MK_SHIFT;
	ui.n_s_left = wparam & MK_LBUTTON;
	ui.n_s_right = wparam & MK_RBUTTON;
	ui.n_s_middle = wparam & MK_MBUTTON;
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
		ui.mouse_xy = { ((double)(short)LOWORD(lParam)), ((double)(short)HIWORD(lParam)) };
		ui.mouse_move();
		if (!ui.changed_area.empty()) ui.paint(hWnd);
		return 0;
	case WM_MOUSELEAVE:
		tracking_mouse = false;
		if (ui.n_s_left) ui.mouse_button_left_up();
		if (ui.n_s_right) ui.mouse_button_right_up();
		if (ui.n_s_middle) ui.mouse_button_middle_up();
		return 0;
	case WM_MOUSEWHEEL:
		init_shift(GET_KEYSTATE_WPARAM(wParam));
		ui.mouse_wheel_turn(GET_WHEEL_DELTA_WPARAM(wParam) / WHEEL_DELTA);
		if (!ui.changed_area.empty()) ui.paint(hWnd);
		return 0;
	case WM_LBUTTONDOWN: case WM_RBUTTONDOWN: case WM_MBUTTONDOWN:
		init_shift(wParam);
		if (message == WM_LBUTTONDOWN) ui.mouse_button_left_down();
		if (message == WM_RBUTTONDOWN) ui.mouse_button_right_down();
		if (message == WM_MBUTTONDOWN) ui.mouse_button_middle_down();
		if (!ui.changed_area.empty()) ui.paint(hWnd);
		return 0;
	case WM_LBUTTONUP: case WM_RBUTTONUP: case WM_MBUTTONUP:
		init_shift(wParam);
		if (message == WM_LBUTTONUP) ui.mouse_button_left_up();
		if (message == WM_RBUTTONUP) ui.mouse_button_right_up();
		if (message == WM_MBUTTONUP) ui.mouse_button_middle_up();
		if (!ui.changed_area.empty()) ui.paint(hWnd);
		return 0;
	case WM_LBUTTONDBLCLK: case WM_RBUTTONDBLCLK: case WM_MBUTTONDBLCLK:
		init_shift(wParam);
		if (message == WM_LBUTTONDBLCLK) ui.mouse_button_left_down(true);
		if (message == WM_RBUTTONDBLCLK) ui.mouse_button_right_down(true);
		if (message == WM_MBUTTONDBLCLK) ui.mouse_button_middle_down(true);
		if (!ui.changed_area.empty()) ui.paint(hWnd);
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
			}
			break;
		case VK_F2:
			{
				run_timer = false;
				int r = MessageBox(hWnd, L"сохранить?", L"предупреждение", MB_YESNO);
//				if (r == IDYES) mutator::save_to_txt_file(exe_path / tetfile);
				run_timer = true;
			}
			break;
		case VK_CONTROL:
			ui.n_s_ctrl = true;
			break;
		default:
			ui.key_down(wParam);
			if (!ui.changed_area.empty()) ui.paint(hWnd);
		}
		return 0;
	case WM_KEYUP:
		if (wParam == VK_CONTROL) ui.n_s_ctrl = false;
		return 0;
	case WM_CHAR:
		ui.key_press(wParam);
		if (!ui.changed_area.empty()) ui.paint(hWnd);
		return 0;
	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		BeginPaint(hWnd, &ps);
		ui.paint(hWnd);
		EndPaint(hWnd, &ps);
		return 0;
	}
	case WM_SIZE:
//		mutator::resize({ LOWORD(lParam), HIWORD(lParam) });
		return 0;
	case WM_TIMER:
		if (!run_timer) return 0;
		switch (wParam)
		{
		case 1:
			ui.run_timer1000();
			if (!ui.changed_area.empty()) ui.paint(hWnd);
			break;
		case 2:
			ui.run_timer250();
			break;
		}
		return 0;
	case WM_CREATE:
		SetTimer(hWnd, 1, 1000, 0); // общий с отрисовкой
		SetTimer(hWnd, 2, 250, 0); // более быстрый, без отрисовки
		return 0;
	case WM_DESTROY:
		KillTimer(hWnd, 1);
		KillTimer(hWnd, 2);
		PostQuitMessage(0);
		return 0;
	}
	return DefWindowProc(hWnd, message, wParam, lParam);
}

void read_cena(_e_button& eb)
{
	static auto fun = std::make_shared<_e_function>(eb.ui, []() { scan_supply_and_demand(ui); });
	eb.ui->n_timer1000.erase(fun);
	if (eb.checked) eb.ui->n_timer1000.insert(fun);
}

void init_ui_elements()
{
	auto term = std::make_shared<_e_terminal>(&ui);
	term->local_area.x = _interval(10, 480);
	term->local_area.y = _interval(10, 740);
	ui.n_ko->add_child(term);
	ui.n_act_key = term;

	auto button = std::make_shared<_e_button>(&ui);
	button->picture.set_from_text("00000000000000000000ff0f000108000108fc3f0804200808400808400810800810800820000920000940000a40000a80000c80000c00010800ff0f000000000000000000000000", 0, ui.cc1);
	button->trans.offset = { 600, 16 };
	button->hint = L"загрузить статистику";
	button->run = [](_e_button&) { start_se(); };
	ui.n_ko->add_child(button);

	button = std::make_shared<_e_button>(&ui);
	button->picture.set_from_text("000000000000fcff0744000a440012440022440022440022c4002284ff21040020040020040020040020040020040020040020040020040020040020040020fcff3f000000000000", 0, ui.cc1);
	button->trans.offset = { 632, 16 };
	button->hint = L"сохранить статистику";
	button->run = [](_e_button&) { ed.save_to_file(); };
	ui.n_ko->add_child(button);

	button = std::make_shared<_e_button>(&ui);
	button->picture.set_from_text("00000000000000380000fe0000830100010180010380010380010300010100830100ff00803900c000006000003000001800000c0000060000020000000000000000000000000000", 0, ui.cc1);
	button->checkbox = true;
	button->trans.offset = { 664, 16 };
	button->hint = L"чтение цен";
	button->run = read_cena;
	ui.n_ko->add_child(button);

	button = std::make_shared<_e_button>(&ui);
	button->picture.set_from_text("000000000000000000006000007c08003608001804001c02000e02000301008101808000c0c00040220020120030920000de00003c00000400000000000000000000000000000000", 0, ui.cc1);
	button->checkbox = true;
	button->trans.offset = { 696, 16 };
	button->hint = L"торговля";
	button->run = [](_e_button& bu) { change_can_trade(bu.checked); };
	ui.n_ko->add_child(button);

	button = std::make_shared<_e_button>(&ui);
	button->picture.set_from_text("00000000000000000000000000800300c00300e00100fc00006600007000003000002c00002400000300800100c00000400000200000000000000000000000000000000000000000", 0, ui.cc1);
	button->trans.offset = { 728, 16 };
	button->hint = L"купить";
	button->run = [](_e_button&) { buy_shares(ui); };
	ui.n_ko->add_child(button);

	button = std::make_shared<_e_button>(&ui);
	button->picture.set_from_text("00000000000000000000000000001800000c00000400000200800100c000006000002000001100800900803400801a00800700800300800000000000000000000000000000000000", 0, ui.cc1);
	button->trans.offset = { 760, 16 };
	button->hint = L"продать";
	button->run = [](_e_button&) { sell_shares(ui); };
	ui.n_ko->add_child(button);

	button = std::make_shared<_e_button>(&ui);
	button->picture.set_from_text("00000000000000000000000000000000000000ff0380010a90001a880022cc006246007efe001acc000a90000280000280000280810300ff00000000000000000000000000000000", 0, ui.cc1);
	button->trans.offset = { 792, 16 };
	button->hint = L"size_el++";
	button->run = [](_e_button&) { expand_elements_graph2(); };
	ui.n_ko->add_child(button);

	button = std::make_shared<_e_button>(&ui);
	button->picture.set_from_text("00000000000000000000000000fc0000870000810180000180003398001a90000eb700fefc0003a00007b0003d880001800001801f0100f000000000000000000000000000000000", 0, ui.cc1);
	button->trans.offset = { 824, 16 };
	button->hint = L"size_el--";
	button->run = [](_e_button&) { narrow_graph_elements2(); };
	ui.n_ko->add_child(button);

	auto eg = std::make_shared<_e_exchange_graph>(&ui);
	eg->local_area.x = _interval(490, 1570);
	eg->local_area.y = _interval(50, 735);
	ui.n_ko->add_child(eg);

	auto polz = std::make_shared<_e_scrollbar>(&ui);
	polz->vid = 2;
	eg->add_child(polz);
	polz->prilip();

}

int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nCmdShow)
{
	exe_path = get_exe_path(hInstance);
	init_ui_elements();
	return run_windows_app(L"se", hInstance, WndProc, nCmdShow);
}
