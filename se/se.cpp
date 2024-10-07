#include "ui.h"
#include "e_terminal.h"
#include "e_exchange_graph.h"
#include "exchange_data.h"
#include "exchange_trade.h"
#include "exchange_research.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct _se_mode : public _ui
{
private:

	virtual bool start2() override;
	void init_ui_elements();
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

void read_cena(_e_button& eb, _ui& ui)
{
	static auto fun = std::make_shared<_e_function>(eb.ui, [&ui]() { scan_supply_and_demand(ui); });
	eb.ui->n_timer1000.erase(fun);
	if (eb.checked) eb.ui->n_timer1000.insert(fun);
}

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void _se_mode::init_ui_elements()
{
	auto term = std::make_shared<_e_terminal>(this);
	term->local_area.x = _interval(10, 480);
	term->local_area.y = _interval(10, 740);
	n_ko->add_child(term);
	n_act_key = term;
	term->command.insert({ L"1", { L"статистика цен", exchange__fun1 } });
	term->command.insert({ L"2", { L"тестирование фильтра", test__filter } });
	term->command.insert({ L"sad", { L"спрос и предложение", exchange__fun2 } });
	term->command.insert({ L"delta", { L"разность цен", exchange__fun3 } });

	auto button = std::make_shared<_e_button>(this);
	button->picture.set_from_text("00000000000000000000ff0f000108000108fc3f0804200808400808400810800810800820000920000940000a40000a80000c80000c00010800ff0f000000000000000000000000", 0, cc1);
	button->trans.offset = { 600, 16 };
	button->hint = L"загрузить статистику";
	button->run = [](_e_button&) { load_se(); };
	n_ko->add_child(button);

	button = std::make_shared<_e_button>(this);
	button->picture.set_from_text("000000000000fcff0744000a440012440022440022440022c4002284ff21040020040020040020040020040020040020040020040020040020040020040020fcff3f000000000000", 0, cc1);
	button->trans.offset = { 632, 16 };
	button->hint = L"сохранить статистику";
	button->run = [](_e_button&) { ed.save_to_file(); };
	n_ko->add_child(button);

	button = std::make_shared<_e_button>(this);
	button->picture.set_from_text("00000000000000380000fe0000830100010180010380010380010300010100830100ff00803900c000006000003000001800000c0000060000020000000000000000000000000000", 0, cc1);
	button->checkbox = true;
	button->trans.offset = { 664, 16 };
	button->hint = L"чтение цен";
	button->run = [this](_e_button& eb) { read_cena(eb, *this); };
	n_ko->add_child(button);

	button = std::make_shared<_e_button>(this);
	button->picture.set_from_text("000000000000000000006000007c08003608001804001c02000e02000301008101808000c0c00040220020120030920000de00003c00000400000000000000000000000000000000", 0, cc1);
	button->checkbox = true;
	button->trans.offset = { 696, 16 };
	button->hint = L"торговля";
	button->run = [](_e_button& bu) { change_can_trade(bu.checked); };
	n_ko->add_child(button);

	button = std::make_shared<_e_button>(this);
	button->picture.set_from_text("00000000000000000000000000800300c00300e00100fc00006600007000003000002c00002400000300800100c00000400000200000000000000000000000000000000000000000", 0, cc1);
	button->trans.offset = { 728, 16 };
	button->hint = L"купить";
	button->run = [this](_e_button&) { buy_shares(*this); };
	n_ko->add_child(button);

	button = std::make_shared<_e_button>(this);
	button->picture.set_from_text("00000000000000000000000000001800000c00000400000200800100c000006000002000001100800900803400801a00800700800300800000000000000000000000000000000000", 0, cc1);
	button->trans.offset = { 760, 16 };
	button->hint = L"продать";
	button->run = [this](_e_button&) { sell_shares(*this); };
	n_ko->add_child(button);

	button = std::make_shared<_e_button>(this);
	button->picture.set_from_text("00000000000000000000000000000000000000ff0380010a90001a880022cc006246007efe001acc000a90000280000280000280810300ff00000000000000000000000000000000", 0, cc1);
	button->trans.offset = { 792, 16 };
	button->hint = L"size_el++";
	button->run = [](_e_button&) { expand_elements_graph2(); };
	n_ko->add_child(button);

	button = std::make_shared<_e_button>(this);
	button->picture.set_from_text("00000000000000000000000000fc0000870000810180000180003398001a90000eb700fefc0003a00007b0003d880001800001801f0100f000000000000000000000000000000000", 0, cc1);
	button->trans.offset = { 824, 16 };
	button->hint = L"size_el--";
	button->run = [](_e_button&) { narrow_graph_elements2(); };
	n_ko->add_child(button);

	auto eg = std::make_shared<_e_exchange_graph>(this);
	eg->local_area.x = _interval(490, 1570);
	eg->local_area.y = _interval(50, 735);
	n_ko->add_child(eg);
	eg->curve.push_back(new _candle_curve2);
	eg->curve.push_back(new _prices_curve4);

	auto polz = std::make_shared<_e_scrollbar>(this);
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
