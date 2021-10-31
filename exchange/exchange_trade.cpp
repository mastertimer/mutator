#include "g_exchange_graph.h"
#include "mediator.h"
#include "t_function.h"
#include "exchange_trade.h"

i64 can_trade = -6; // разрешенное количество сделок (купить-продать = 2 сделки), отрицательное - неактивно
int vrema_prodat = 0; // время когда нужно продать

void scan_supply_and_demand()
{
	if (zamok_pokupki) return;
	_supply_and_demand a;
	int ok = recognize.read_prices_from_screen(&a);
	if (ok != 0)
	{
		/*#ifdef DEBUG_MMM
				wstring fn = exe_path + L"errorscr.bmp";
				if (!FileExists(fn.c_str()))
				{
					recognize.image_.SaveToFile(fn.c_str());
					ofstream file(exe_path + L"errorscr.txt");
					file << ok;
					file.close();
				}
		#endif // DEBUG_MMM*/
		return;
	}
	ed.push_back(a);
	update_index_data();

	graph->run(nullptr, graph, flag_run);

	// всякие проверки на начало покупки !!!!
	if (can_trade <= 0) return;

	if (can_trade & 1) // была покупка, но небыло продажи
	{
		if ((a.time >= vrema_prodat) || ((a.time_hour() == 18) && (a.time_minute() > 30))/* || noracle->get_latest_events(noracle->zn.size() - 1).stop()*/)
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
			_t_function* fu = new _t_function(36);
			fu->run(0, fu, flag_run);
		}
		return;
	}

	if (a.time_hour() >= 18) return; // слишком поздно
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
	vrema_prodat = a.time + ti * 60;
	can_trade--;
	_t_function* fu = new _t_function(35);
	fu->run(0, fu, flag_run);
}

void change_can_trade(bool can)
{
	can_trade = (can) ? abs(can_trade) : -abs(can_trade);
}

void buy_shares()
{
	if (zamok_pokupki) return;
	zamok_pokupki = true;
	_t_function* fu = new _t_function(35);
	fu->run(0, fu, flag_run);
}

void sell_shares()
{
	if (zamok_pokupki) return;
	zamok_pokupki = true;
	_t_function* fu = new _t_function(36);
	fu->run(0, fu, flag_run);
}

