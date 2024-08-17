#include "mediator.h"
#include "exchange_trade.h"
#include "reading_sad.h"
#include "e_exchange_graph.h"

i64 can_trade = -6; // разрешенное количество сделок (купить-продать = 2 сделки), отрицательное - неактивно
int vrema_prodat = 0; // время когда нужно продать

void scan_supply_and_demand(_ui& ui)
{
	if (zamok_pokupki) return;
	auto a = read_sad_from_screen();
	if (!a) return;
	ed.push_back(*a);
	update_index_data();

	graph2->update();

	// всякие проверки на начало покупки !!!!
	if (can_trade <= 0) return;

	if (can_trade & 1) // была покупка, но небыло продажи
	{
		if ((a->time >= vrema_prodat) || ((a->time_hour() == 18) && (a->time_minute() > 30))/* || noracle->get_latest_events(noracle->zn.size() - 1).stop()*/)
		{
			/*			int b;
						recognize.ReadTablicaZayavok(0, b);
						if (b != gotovo_prodaz)// лажа, не все купилось, прекратить
						{
							gotovo_prodaz = 0;
							popitok_prodaz = 0;
							return;
						}*/
			zamok_pokupki = true;
			can_trade--;
			auto efun = std::make_shared<_e_function>(&ui);
			efun->run = [tt = efun.get()](){ buy_stock(tt, false); };
			efun->run();
		}
		return;
	}

	if (a->time_hour() >= 18) return; // слишком поздно
	time_t ti = 0;// super_oracle->prediction();


	if (ti == 0) return;
	// купить акции
/*	int b;
	recognize.ReadTablicaZayavok(0, b);
	if (b != gotovo_prodaz)// лажа, не все купилось, прекратить
	{
		gotovo_prodaz = 0;
		popitok_prodaz = 0;
		return;
	}*/
	zamok_pokupki = true;
	vrema_prodat = a->time + ti * 60;
	can_trade--;
	auto efun = std::make_shared<_e_function>(&ui);
	efun->run = [tt = efun.get()]() { buy_stock(tt, true); };
	efun->run();
}

void change_can_trade(bool can)
{
	can_trade = (can) ? abs(can_trade) : -abs(can_trade);
}

void buy_shares(_ui& ui)
{
	if (zamok_pokupki) return;
	zamok_pokupki = true;
	auto efun = std::make_shared<_e_function>(&ui);
	efun->run = [tt = efun.get()]() { buy_stock(tt, true); };
	efun->run();
}

void sell_shares(_ui& ui)
{
	if (zamok_pokupki) return;
	zamok_pokupki = true;
	auto efun = std::make_shared<_e_function>(&ui);
	efun->run = [tt = efun.get()]() { buy_stock(tt, false); };
	efun->run();
}

