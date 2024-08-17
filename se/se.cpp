#include "ui.h"
#include "win_basic.h"
#include "e_terminal.h"
#include "e_exchange_graph.h"
#include "exchange_data.h"
#include "exchange_trade.h"
#include "../main.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

_ui ui;

struct _se_mode : public _mode
{
	virtual _bitmap& get_bitmap() override { return ui.canvas; }
	virtual _iarea draw(_isize r) override;
	virtual void timer1000() override;
	virtual void timer250() override;
	virtual void key_press(u64 key) override;
	virtual void key_down(u64 key) override;
	virtual void mouse_move() override;
	virtual void mouse_leave() override;
	virtual void mouse_wheel(i64 delta) override;
	virtual void mouse_button_left(bool pressed, bool dbl) override;
	virtual void mouse_button_right(bool pressed, bool dbl) override;
	virtual void mouse_button_middle(bool pressed, bool dbl) override;

private:
	virtual bool start2() override;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace
{

bool start_unit()
{
	main_modes[L"se"] = std::make_unique<_se_mode>();
	return true;
}
auto ignore = start_unit();

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

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

bool _se_mode::start2()
{
	static bool first_run = true;
	if (first_run) first_run = false; else return true;

	init_ui_elements();
	return true;
}

_iarea _se_mode::draw(_isize r)
{
	return ui.draw(r);
}

void _se_mode::timer1000()
{
	ui.run_timer1000();
}

void _se_mode::timer250()
{
	ui.run_timer250();
}

void _se_mode::key_press(u64 key)
{
	ui.key_press(key);
}

void _se_mode::key_down(u64 key)
{
	ui.key_down(key);
}

void _se_mode::mouse_move()
{
	ui.mouse_move();
}

void _se_mode::mouse_leave()
{
	if (mouse.left_button) ui.mouse_button_left_up();
	if (mouse.right_button) ui.mouse_button_right_up();
	if (mouse.middle_button) ui.mouse_button_middle_up();
}

void _se_mode::mouse_wheel(i64 delta)
{
	ui.mouse_wheel_turn(delta);
}

void _se_mode::mouse_button_left(bool pressed, bool dbl)
{
	if (dbl) { ui.mouse_button_left_down(true); return; }
	if (pressed) ui.mouse_button_left_down(); else ui.mouse_button_left_up();
}

void _se_mode::mouse_button_right(bool pressed, bool dbl)
{
	if (dbl) { ui.mouse_button_right_down(true); return; }
	if (pressed) ui.mouse_button_right_down(); else ui.mouse_button_right_up();
}

void _se_mode::mouse_button_middle(bool pressed, bool dbl)
{
	if (dbl) { ui.mouse_button_middle_down(true); return; }
	if (pressed) ui.mouse_button_middle_down(); else ui.mouse_button_middle_up();
}
