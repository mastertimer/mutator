﻿/*//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

рисование ломанной линии
выделить нулевые оси у графика
не подписывать сетку < 0, если все данные > 0 (и наоборот)
сделать отображение секунд при увеличении графика цен
преобразование координат при выводе графика

~ коэфиициент - насколько числа красивые? у кого красивее - у покупки или у продажи? (у всех и у 10-15)

распределение "вероятностей" вместо одного числа 0..+5 -> 0.2, +5..+10 -> 0.8, ... +50..+inf -> 0.01%

/*//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <chrono>
#include <deque>

#include "t_function.h"
#include "mediator.h"
#include "stock.h"

constexpr wchar_t file_stock_statistics[] = L"..\\..\\data\\base.c3";

_sable_graph *graph = nullptr; // график

_interval y_graph; // координата у на картинке
_interval y_graph_re; // реальная координата y

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct _index           // разнообразные минутные коэффициенты
{
	_iinterval ncc;     // диапазон цен
	i64    time    = 0; // время (с обнуленной секундой (time%60 = 0))
	double min     = 0; // минимальная цена
	double max     = 0; // макимальная цена
	double first   = 0; // первая цена
	double last    = 0; // последняя цена
	double c3_buy  = 0; // цена покупки на 3-й секунде
	double c3_sale = 0; // цена продажи на 3-й секунде
	double minmin  = 0; // минимальная цена минимального спроса ([19])
	double maxmax  = 0; // максимальная цена максимального предложения ([19])
};

struct _index_data // все коэффициенты
{
	std::vector<_index> data; // поминутный вектор

	bool update(); // обновить ранные, вызывать после обновления sss2
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

_index_data index; // все расчетные данные

//_basic_curve* super_oracle = nullptr; // оракул для предсказания

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct _candle_curve : public _basic_curve // классические свечи
{
	void draw(i64 n, _area area) override; // нарисовать 1 элемент
	_interval get_y(i64 n) override; // дипазон рисования по y
};

struct _prices_curve : public _basic_curve // посекундный спрос/предложение
{
	static const int max_part = 22000; // максимально количество элементов ss
	std::deque<_supply_and_demand> part_ss; // часть супер-статистики
	i64 begin_ss = 0; // начало куска супер-статистики

	void draw(i64 n, _area area) override; // нарисовать 1 элемент
	_interval get_y(i64 n) override; // дипазон рисования по y
};

struct _prices_curve2 : public _basic_curve // посекундный спрос/предложение (цвет - дельта)
{
	static const int max_part = 22000; // максимально количество элементов ss
	std::deque<_supply_and_demand> part_ss; // часть супер-статистики
	i64 begin_ss = 0; // начало куска супер-статистики

	void draw(i64 n, _area area) override; // нарисовать 1 элемент
	_interval get_y(i64 n) override; // дипазон рисования по y
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct _label_statistics
{
	struct _lppn // номера для статистики предсказаний
	{
		i64 start_basis = 0; // номер первого элемента базиса
		i64 n = 0; // номер элемента, который предсказывается
	};

	i64 prediction_depth = 1; // глубина предсказания 35 минут
	i64 prediction_basis = 65; // база предсказания 60 минут
	std::vector<_lppn> label;

	void calc();
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void start_stock()
{
	static bool first = true; if (!first) return; first = false;

	stock_statistics.load_from_file(exe_path + file_stock_statistics);

	if (!graph) return;
	if (!graph->find1<_g_scrollbar>(flag_part))
	{
		_g_scrollbar* sb = new _g_scrollbar;
		sb->vid = 2;
		graph->add_flags(sb, flag_sub_go + flag_part + (flag_run << 32));
	}
	graph->cha_area();

	index.update();
	graph->curve2.push_back(new _candle_curve);
	graph->curve2.push_back(new _prices_curve2);
//	graph->curve2.push_back(new _prices_curve);
}

void sable_fun1(_g_terminal* t)
{
/*	fun13(nullptr, nullptr, 0);
	t->add_text(L"количество цен: " + std::to_wstring(sss.size));
	i64 size_pak = (sss.data.size() + 7) / 8;
	t->add_text(L"сжатый размер: " + std::to_wstring(size_pak));
	t->add_text(L"размер   сжатой записи:  " + double_to_wstring(double(size_pak) / sss.size, 1));
	t->add_text(L"размер несжатой записи: " + std::to_wstring(sizeof(_supply_and_demand)));
	_prices c;
	for (i64 i = 0; i < sss.size; i++)
	{
		sss.read(i, c);
		stock_statistics.push_back(c);
		if (rnd(100) == 13) stock_statistics.push_back(c);
	}
	t->start_timer();
//	_compression_stock_statistics cs(stock_statistics);
//	cs.save_to_file(exe_path + sss2_file);
	stock_statistics.save_to_file(exe_path + sss2_file);
	t->stop_timer(L"время сжатия и сохранения");*/
}

void sable_fun2(_g_terminal* t)
{
/*	fun13(nullptr, nullptr, 0);
	t->add_text(L"количество цен: " + std::to_wstring(sss.size));
	i64 size_pak = (sss.data.size() + 7) / 8;
	t->add_text(L"сжатый размер: " + std::to_wstring(size_pak));
	t->add_text(L"размер   сжатой записи:  " + double_to_wstring(double(size_pak) / sss.size, 1));
	t->start_timer();
	stock_statistics.load_from_file(exe_path + sss2_file);
	t->stop_timer(L"время загрузки и распаковки");
	if (sss.size != (i64)stock_statistics->size())
	{
		t->add_text(L"несовпадение количества!! ");
		return;
	}
	_prices c;
	for (i64 i = 0; i < sss.size; i++)
	{
		sss.read(i, c);
		if (c != stock_statistics[i])
		{
			t->add_text(L"не совпало!! ");
			return;
		}
	}
	t->add_text(L"OK полное совадение! ");*/
}

i64 can_trade    = -6; // разрешенное количество сделок (купить-продать = 2 сделки), отрицательное - неактивно
int vrema_prodat =  0; // время когда нужно продать

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
	stock_statistics.push_back(a);
	index.update();

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

void expand_elements_graph()
{
	graph->size_el++;
	graph->cha_area();
}

void narrow_graph_elements()
{
	if (graph->size_el > 1) graph->size_el--;
	graph->cha_area();
}

void save_stock_statistics()
{
	stock_statistics.save_to_file(exe_path + file_stock_statistics);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

_sable_graph::~_sable_graph()
{
	for (auto i : curve2) delete i;
}

bool _sable_graph::mouse_down_left2(_xy r)
{
	x_tani = (i64)r.x;
	return true;
}

void _sable_graph::mouse_move_left2(_xy r)
{
	i64 dx = ((i64)r.x - x_tani) / size_el;
	if (dx == 0) return;
	x_tani += dx * size_el;

	_g_scrollbar* polz = find1<_g_scrollbar>(flag_part); 
	if (!polz) return;
	double* ii = &polz->position;
	if (!ii) return;
	*ii -= double(dx) / v_vib;
	if (*ii < 0) *ii = 0;
	if (*ii > 1) *ii = 1;
	polz->cha_area();
	polz->run(0, polz, flag_run);
}

void _sable_graph::run(_tetron* tt0, _tetron* tt, u64 flags)
{
	cha_area();
}

_sable_graph::_sable_graph()
{
	graph = this;
	local_area = { {0, 200}, {0, 100} };
}

std::string date_to_ansi_string(time_t time)
{
	std::string res = "22.12.20";
	tm a;
	localtime_s(&a, &time);
	res[0] = (a.tm_mday / 10) + '0';
	res[1] = (a.tm_mday % 10) + '0';
	int m = a.tm_mon + 1;
	res[3] = (m / 10) + '0';
	res[4] = (m % 10) + '0';
	int g = (a.tm_year + 1900) % 100;
	res[6] = (g / 10) + '0';
	res[7] = (g % 10) + '0';
	return res;
}

void _sable_graph::ris2(_trans tr, bool final)
{
	_area a = tr(local_area);
	y_graph = a.y;
	_interval y_; // диапазон у (grid)
	static std::vector<time_t> time_; // отсчеты времени (grid)

	double polzi_ = 0; // !! ползунок

	_g_scrollbar* sb = find1<_g_scrollbar>(flag_part);
	if (sb)	polzi_ = sb->position;

	i64 ll2 = curve2.size();

	i64 k_el = local_area.x.length() / size_el;
	if (k_el < 1) return;
	double r_el = a.x.length() / k_el;

	i64 n = index.data.size();
	if (n == 0) return;
	v_vib = n - 1;
	if (v_vib < 0) v_vib = 0;
	int vib = (int)(polzi_ * v_vib + 0.5); // !! ползунок

	int period = 60;
	// 1-й проход - вычисление zmin, zmax
	double zmin = 1E100;
	double zmax = -1E100;
	for (i64 i = 0; i < k_el; i++)
	{
		i64 ii = i + vib;
		if (ii >= (i64)index.data.size()) break;
		for (i64 j = 0; j < ll2; j++)
		{
			_interval il = curve2[j]->get_y(ii);
			if (il.min < zmin) zmin = il.min;
			if (il.max > zmax) zmax = il.max;
		}
	}
	if (zmin == zmax) zmax = zmin + 1.0;
	y_ = { zmin, zmax };
	y_graph_re = y_;
	time_.clear();
	// 2-й проход - рисование
	for (i64 i = 0; i < k_el; i++)
	{
		i64 ii = i + vib;
		if (ii >= (i64)index.data.size()) break;
		time_.push_back(index.data[ii].time);
		for (i64 j = 0; j < ll2; j++)
		{
			_interval il = curve2[j]->get_y(ii);
			double ymi = a.y.max - (il.min - zmin) * a.y.length() / (zmax - zmin);
			double yma = a.y.max - (il.max - zmin) * a.y.length() / (zmax - zmin);
			double x = r_el * i + a.x.min;
			curve2[j]->draw(ii, { {x, x + r_el}, {yma, ymi} });
		}
	}
	// рисование сетки
	uint col_setka      = cc2 - 0xE0000000; // цвет сетки
	uint col_setka_font = cc1;              // цвет подписи сетки
	if (time_.size() < 1)
	{
		master_bm.line({ a.x.min, a.y.min }, { a.x.max, a.y.max }, 0xFF800000);
		master_bm.line({ a.x.min, a.y.max }, { a.x.max, a.y.min }, 0xFF800000);
		return;
	}
	// рисование горизонтальных линий сетки с подписями
	i64 dex = 33; // длина подписи
	i64 maxN = a.y.length() / 15;
	if (maxN > 1)
	{
		double mi, step;
		os_pordis(y_.min, y_.max, maxN, mi, step, c_unpak);
		for (double y = mi; y < y_.max; y += step)
		{
			double yy = a.y.max - (y - y_.min) * a.y.length() / (y_.max - y_.min);
			master_bm.line({ a.x.min + dex, yy }, { a.x.max - dex, yy }, col_setka);
			master_bm.text16({ std::max(a.x.min, 0.0) + 2, (i64)(yy - 6) }, double_to_string(y, 2), col_setka_font);
			master_bm.text16({ std::min((i64)a.x.max, master_bm.size.x) - dex, (i64)(yy - 6) }, double_to_string(y, 2),
				col_setka_font);
		}
	}
	// рисование вертикальных линий сетки с подписями
	static int g_delta_time[] = {
		1, 2, 3, 5, 10, 15, 20, 30,                    // секунды
		60, 120, 180, 300, 600, 900, 1200, 1800,       // минуты
		3600, 7200, 10800, 14400, 21600, 28800, 43200, // часы
		86400, 172800, 345600, 691200, 1382400,        // дни
		2764800, 5529600, 8294400, 11059200, 16588800, // месяца
		33177600 };                                    // год

	double rel = r_el;
	dex = 26;
	int stept = (((int)(dex / rel)) + 1) * period;
	int ks = sizeof(g_delta_time) / sizeof(g_delta_time[0]);
	for (int i = 0; i < ks; i++)
		if (g_delta_time[i] >= stept)
		{
			stept = g_delta_time[i];
			break;
		}
	int dele[] = { 1, 60, 3600, 86400, 2764800, 33177600 };
	int ost[]  = { 60, 60, 24, 32, 12, 1000 };
	int ido = 0;
	if (stept % 33177600) ido = 4;
	if (stept %  2764800) ido = 3;
	if (stept %    86400) ido = 2;
	if (stept %     3600) ido = 1;
	if (stept %       60) ido = 0;
	std::string s = "00:00";
	int mintime = 0;
	int pr_time = 0;
	for (uint i = 0; i < time_.size(); i++)
	{
		if (time_[i] == 0) continue;
		if (mintime == 0) mintime = time_[i];
		bool sca = ((pr_time > 0) && (time_[i] - pr_time > 36000));
		pr_time = time_[i];
		if (time_[i] % stept == 0)
		{ // вертикальная линия с подписью
			double x = rel * i;
			if ((x <= dex) || (x >= a.x.length() - dex)) continue;
			uint cl = (sca) ? (0x80FF0000) : col_setka;
			master_bm.line({ i64(a.x.min + x), a.y.min }, { a.x.min + x, a.y.max }, cl);
			if ((x - 13 <= dex) || (x + 13 >= a.x.length() - dex)) continue;
			tm t3;
			localtime_s(&t3, &time_[i]);
			s[4] = '0' + (t3.tm_min % 10);
			s[3] = '0' + (t3.tm_min / 10);
			s[1] = '0' + (t3.tm_hour % 10);
			s[0] = '0' + (t3.tm_hour / 10);
/*			int ii = (time_[i] / dele[ido]) % ost[ido];
			s[4] = '0' + (ii % 10);
			s[3] = '0' + (ii / 10);
			ii = (time_[i] / dele[ido + 1]) % ost[ido + 1];
			s[1] = '0' + (ii % 10);
			s[0] = '0' + (ii / 10);*/
			master_bm.text16({ x - 11 + a.x.min, std::min((i64)a.y.max, master_bm.size.y) - 13 }, s, col_setka_font);
			master_bm.text16({ x - 11 + a.x.min, std::max(a.y.min, 0.0) }, s, col_setka_font);
			continue;
		}
		if (sca)
		{
			double x = rel * i;
			if ((x <= dex) || (x >= a.x.length() - dex)) continue;
			master_bm.line({ a.x.min + x, a.y.min }, { a.x.min + x, a.y.max }, 0x80FF0000);
		}
	}
	// рисование даты
	master_bm.text16n(std::max(a.x.min, 0.0) + dex + 10, std::max(a.y.min, 0.0) + 10,
		date_to_ansi_string(mintime).data(), 4, cc2 - 0x80000000);
	// рисование количества элементов
	master_bm.text16n(std::max(a.x.min, 0.0) + dex + 10, std::max(a.y.min, 0.0) + 60,
		std::to_string(stock_statistics.size()).data(), 2, 0x60ff0000);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool _index_data::update()
{
	i64 vcc = 0;
	if (!data.empty()) vcc = data.back().ncc.max;
	if (vcc == (i64)stock_statistics->size()) return false; // ничего не изменилось
	if (vcc > (i64)stock_statistics->size())
	{
		data.clear(); // обработанных данных больше, чем исходных, потому пусть будет полный перерасчет
		vcc = 0;
	}
	if (stock_statistics->size() < 2) return false; // мало данных для обработки
	i64 back_minute = stock_statistics->back().time_to_minute();
	if (!data.empty())
	{
		if (back_minute == data.back().time + 1) return false; // еще рано
		if (back_minute <= data.back().time) // так быть не должно, полный перерасчет
		{
			data.clear();
			vcc = 0;
		}
	}
	if (stock_statistics->size() - vcc < 2) return false; // мало данных для обработки
	time_t t = 0;
	_index cp;
	for (i64 i = vcc; i < (i64)stock_statistics->size(); i++)
	{
		const _supply_and_demand& cc = stock_statistics[i];
		time_t t2 = cc.time_to_minute();
		if (t2 == t)
		{
			double aa = ((i64)cc.demand.offer[0].price + cc.supply.offer[0].price) * (c_unpak * 0.5);
			if (aa < cp.min) cp.min = aa;
			if (aa > cp.max) cp.max = aa;
			if (cc.demand.offer[size_offer - 1].price * c_unpak < cp.minmin) cp.minmin = cc.demand.offer[size_offer - 1].price * c_unpak;
			if (cc.supply.offer[size_offer - 1].price * c_unpak > cp.maxmax) cp.maxmax = cc.supply.offer[size_offer - 1].price * c_unpak;
			cp.ncc.max++;
			cp.last = aa;
			if (cc.time % 60 == 3)
			{
				cp.c3_buy = cc.demand.offer[0].price * c_unpak;
				cp.c3_sale = cc.supply.offer[0].price * c_unpak;
			}
			continue;
		}
		if (t != 0)
		{
			data.push_back(cp);
		}
		if (t2 == back_minute) break; // последнюю минуту пока не трогать
		t = t2;
		cp.time = t;
		cp.ncc.min = i;
		cp.ncc.max = i + 1;
		cp.max = cp.min = cp.last = cp.first = ((i64)cc.demand.offer[0].price + cc.supply.offer[0].price) * (c_unpak * 0.5);
		cp.minmin = cc.demand.offer[size_offer - 1].price * c_unpak;
		cp.maxmax = cc.supply.offer[size_offer - 1].price * c_unpak;
		cp.c3_buy = cc.demand.offer[0].price * c_unpak;
		cp.c3_sale = cc.supply.offer[0].price * c_unpak;
	}
	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void _candle_curve::draw(i64 n, _area area)
{
	auto aa = &index.data[n];
	double min_ = aa->min * c_unpak;
	double max_ = aa->max * c_unpak;
	double first_ = aa->first * c_unpak;
	double last_ = aa->last * c_unpak;

	_iinterval xx = area.x;
	xx.min++;
	xx.max--;
	if (xx.empty()) return;

	constexpr uint col_rost = 0xff28A050; // цвет ростущей свечки
	constexpr uint col_pade = 0xff186030; // цвет падающей свечки
	double yfi, yla;
	if (min_ < max_)
	{
		yfi = area.y.max - area.y.length() * (first_ - min_) / (max_ - min_);
		yla = area.y.max - area.y.length() * (last_ - min_) / (max_ - min_);
	}
	else
	{
		yfi = yla = area.y.min;
	}
	if (first_ <= last_)
	{
		master_bm.fill_rectangle({ xx, {yla, yfi} }, col_rost);
		master_bm.line({ xx.center(), area.y.max }, { xx.center(), area.y.min }, col_rost);
	}
	else
	{
		master_bm.fill_rectangle({ xx, {yfi, yla} }, col_pade);
		master_bm.line({ xx.center(), area.y.max }, { xx.center(), area.y.min }, col_pade);
	}
}

_interval _candle_curve::get_y(i64 n)
{
	auto a = &index.data[n];
	return { a->min, a->max };
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void _prices_curve::draw(i64 n, _area area)
{
	static _supply_and_demand pri[61]; // цены
	double min, max; // разброс по y
	for (auto& i : pri) i.clear();

	for (i64 i = index.data[n].ncc.min; i < index.data[n].ncc.max; i++)
	{
		if (i < begin_ss)
		{
			i64 delta = begin_ss - i;
			if (delta >= max_part)
				part_ss.clear();
			else
			{
				_supply_and_demand w;
				w.clear();
				for (int i_ = 0; i_ < delta; i_++)
				{
					part_ss.push_front(w);
					if (part_ss.size() > max_part) part_ss.pop_back();
				}
			}
			begin_ss = i;
		}
		if (i >= begin_ss + (i64)part_ss.size())
		{
			_supply_and_demand w;
			w.clear();
			i64 delta = i - (begin_ss + (int)part_ss.size()) + 1;
			if (delta >= max_part)
			{
				part_ss.clear();
				part_ss.push_back(w);
				begin_ss = i;
			}
			else
				for (int i_ = 0; i_ < delta; i_++)
				{
					part_ss.push_back(w);
					if (part_ss.size() > max_part)
					{
						part_ss.pop_front();
						begin_ss++;
					}
				}
		}
		i64 ii = i - begin_ss;
		if (part_ss[ii].empty()) part_ss[ii] = stock_statistics[i];
		pri[part_ss[ii].time % 60] = part_ss[ii];
	}
	min = index.data[n].minmin - c_unpak;
	max = index.data[n].maxmax;
	_iinterval xx = area.x;
	xx.min++;
	i64 dx = xx.size();
	if (dx < 2) return;
	i64 step = 60;
	if (dx >= 4) step = 30;
	if (dx >= 6) step = 20;
	if (dx >= 8) step = 15;
	if (dx >= 10) step = 12;
	if (dx >= 12) step = 10;
	if (dx >= 20) step = 6;
	if (dx >= 24) step = 5;
	if (dx >= 30) step = 4;
	if (dx >= 40) step = 3;
	if (dx >= 60) step = 2;
	if (dx >= 120) step = 1;
	i64 kol = 60 / step;
	double dd = max - min;
	double ddy = area.y.max - area.y.min;
	for (i64 i = 0; i < kol; i++)
	{
		i64 ss_ = i * step;
		while (pri[ss_].empty())
		{
			if (ss_ + 1 >= (i + 1) * step) break;
			ss_++;
		}
		if (pri[ss_].empty()) continue;
		i64 xx1 = xx.min + dx * i / kol;
		i64 xx2 = xx.min + dx * (i + 1) / kol - 1;
		if (pri[ss_].supply.offer[3].price < pri[ss_].demand.offer[3].price)
		{
			//		xx1++;
		}
		for (int j = size_offer - 1; j >= 0; j--)
		{
			double ce = pri[ss_].supply.offer[j].price * c_unpak;
			_iinterval yy(area.y.min + (max - ce) * ddy / dd, area.y.min + (max - ce + c_unpak) * ddy / dd);
			yy.min++;
			yy.max--;
			if (yy.empty()) continue;
			uint q = (uint)sqrt(pri[ss_].supply.offer[j].number) + 32;
			if (q > 255) q = 255;
			uint cc = (q << 8) + (q << 16) + 0x60000000;
			master_bm.fill_rectangle({ {xx1, xx2}, yy }, cc);
		}
		for (int j = 0; j < size_offer; j++)
		{
			double ce = pri[ss_].demand.offer[j].price * c_unpak;
			_iinterval yy(area.y.min + (max - ce) * ddy / dd, area.y.min + (max - ce + c_unpak) * ddy / dd);
			yy.min++;
			yy.max--;
			if (yy.empty()) continue;
			uint q = (uint)sqrt(pri[ss_].demand.offer[j].number) + 32;
			if (q > 255) q = 255;
			uint cc = q + (q << 8) + 0x60000000;
			master_bm.fill_rectangle({ {xx1, xx2}, yy }, cc);
		}
	}
}

_interval _prices_curve::get_y(i64 n)
{
	auto a = &index.data[n];
	return { a->minmin - c_unpak, a->maxmax };
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void _prices_curve2::draw(i64 n, _area area)
{
	static _supply_and_demand pri[61]; // цены
	double min, max; // разброс по y
	for (auto& i : pri) i.clear();

	for (i64 i = index.data[n].ncc.min; i < index.data[n].ncc.max; i++)
	{
		if (i < begin_ss)
		{
			i64 delta = begin_ss - i;
			if (delta >= max_part)
				part_ss.clear();
			else
			{
				_supply_and_demand w;
				w.clear();
				for (int i_ = 0; i_ < delta; i_++)
				{
					part_ss.push_front(w);
					if (part_ss.size() > max_part) part_ss.pop_back();
				}
			}
			begin_ss = i;
		}
		if (i >= begin_ss + (i64)part_ss.size())
		{
			_supply_and_demand w;
			w.clear();
			i64 delta = i - (begin_ss + (int)part_ss.size()) + 1;
			if (delta >= max_part)
			{
				part_ss.clear();
				part_ss.push_back(w);
				begin_ss = i;
			}
			else
				for (int i_ = 0; i_ < delta; i_++)
				{
					part_ss.push_back(w);
					if (part_ss.size() > max_part)
					{
						part_ss.pop_front();
						begin_ss++;
					}
				}
		}
		i64 ii = i - begin_ss;
		if (part_ss[ii].empty()) part_ss[ii] = stock_statistics[i];
		pri[part_ss[ii].time % 60] = part_ss[ii];
	}
	min = index.data[n].minmin - c_unpak;
	max = index.data[n].maxmax;
	_iinterval xx = area.x;
	xx.min++;
	i64 dx = xx.size();
	if (dx < 2) return;
	i64 step = 60;
	if (dx >= 4) step = 30;
	if (dx >= 6) step = 20;
	if (dx >= 8) step = 15;
	if (dx >= 10) step = 12;
	if (dx >= 12) step = 10;
	if (dx >= 20) step = 6;
	if (dx >= 24) step = 5;
	if (dx >= 30) step = 4;
	if (dx >= 40) step = 3;
	if (dx >= 60) step = 2;
	if (dx >= 120) step = 1;
	i64 kol = 60 / step;
	double dd = max - min;
	double ddy = area.y.max - area.y.min;
	i64 ss_pr = -1;
	for (i64 i = 0; i < kol; i++)
	{
		i64 ss_ = i * step;
		while (pri[ss_].empty())
		{
			if (ss_ + 1 >= (i + 1) * step) break;
			ss_++;
		}
		if (pri[ss_].empty()) continue;
		i64 xx1 = xx.min + dx * i / kol;
		i64 xx2 = xx.min + dx * (i + 1) / kol - 1;
		for (int j = size_offer - 1; j >= 0; j--)
		{
			double ce = pri[ss_].supply.offer[j].price * c_unpak;
			_iinterval yy(area.y.min + (max - ce) * ddy / dd, area.y.min + (max - ce + c_unpak) * ddy / dd);
			yy.min++;
			yy.max--;
			if (yy.empty()) continue;

			uint cc = 0x40ffffff;
			if (ss_pr >= 0)
			{
				if (pri[ss_].supply.offer[j].number > pri[ss_pr].supply.offer[j].number) cc = 0x9000ff00;
				if (pri[ss_].supply.offer[j].number < pri[ss_pr].supply.offer[j].number) cc = 0x70ff0000;
			}
			if (cc != 0x40ffffff) master_bm.fill_rectangle({ {xx1, xx2}, yy }, cc);
		}
		ss_pr = ss_;
	}
}

_interval _prices_curve2::get_y(i64 n)
{
	auto a = &index.data[n];
	return { a->minmin - c_unpak, a->maxmax };
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void _label_statistics::calc()
{
	std::deque<std::pair<time_t,i64>> data;
	label.clear();
	for (i64 i = prediction_basis - 1; i < (i64)index.data.size(); i++) // i - последняя минута базиса
	{
		time_t ti = index.data[i].time;
		if (ti - index.data[i - (prediction_basis - 1)].time == (prediction_basis - 1) * 60)
			data.push_back({ ti + prediction_depth * 60,  i - (prediction_basis - 1) });
		while (!data.empty())
		{
			if (data.front().first > ti) break;
			if (data.front().first == ti) label.push_back({ data.front().second, i });
			data.pop_front();
		}
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void calc_all_prediction(std::function<i64(i64)> o, i64& vv, double& k)
{
	i64 rez = 0; // 0 - ожидание, 2 - продажа
	i64 t_end = 0;
	i64 cena = 0;
	i64 cena2 = 0;
	vv = 0;
	k = 1;
	for (i64 i = 1; i < (i64)index.data.size(); i++)
	{
		if (rez == 2)
		{
			if (index.data[i].time < t_end) continue; // еще не время
			if (index.data[i].time > t_end) { rez = 0; continue; }; // что-то не так
			cena2 = index.data[i].c3_buy;
			rez = 0;
			vv++;
			k *= (cena2 * 0.999) / cena;
		} // после продажи, возможна покупка в ту-же минуту
		i64 t = o(i - 1);
		if (t <= 0) continue;
		t_end = index.data[i].time + t * 60;
		rez = 2;
		cena = index.data[i].c3_sale;
	}
}

/*void calc_all_prediction(_basic_curve& o, i64 &nn, double &kk)
{
	_sable_stat ss_old = sss;
	sss.clear();
	_prices pr;
	i64 rez = 0; // 0 - ожидание, 1 - покупка, 2 - продажа
	i64 t_start = 0;
	i64 t_end   = 0;
	i64 cena = 0;
	i64 cena2 = 0;
	i64 vv = 0;
	double k = 1;
	for (i64 i = 0; i < ss_old.size; i++)
	{
		o.recovery();
		ss_old.read(i, pr);
		sss.add(pr);
		if (rez == 1)
		{
			if (pr.time < t_start + 2) continue; // 2 секунды пауза между решением и действием
			if (pr.time > t_start + 60) { rez = 0; continue; } // что-то не так
			rez = 2;
			cena = pr.sale[0].value;
			continue;
		}
		if (rez == 2)
		{
			if (pr.time < t_end) continue; // еще не время
			if (pr.time > t_end + 60) { rez = 0; continue; }; // что-то не так
			cena2 = pr.buy[0].value;
			rez = 0;
			vv++;
			k *= (cena2 * 0.999) / cena;
			continue;
		}
		i64 t = o.prediction();
		if (t <= 0) continue;
		t_start = pr.time;
		t_end = pr.time + t * 60;
		rez = 1;
	}
	nn = vv;
	kk = k;
}*/

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
