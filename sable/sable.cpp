/*//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

          | результат | итераций | 1 итерация
---------------------------------------------
0.999     |   0.941        61       0.99900
min(rnd)  |   0.873        55       0.99753
max(rnd)  |   1.058        58       1.00097
было(err) |   0.925        61       0.99872
было      |   0.955        21       0.99779

~ коэфиициент - насколько числа красивые? у кого красивее - у покупки или у продажи? (у всех и у 10-15)

/*//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <chrono>

#include "mediator.h"
#include "oracle0.h"
#include "oracle1.h"
#include "oracle3.h"
#include "oracle4.h"
#include "oracle5.h"
#include "sable.h"

constexpr wchar_t sss_file[]   = L"..\\..\\sable\\base.c2";
constexpr wchar_t index_file[] = L"..\\..\\sable\\index.bin";

_sable_graph *graph = nullptr; // график

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct _index // разнообразные минутные коэффициенты
{
	_iinterval ncc; // диапазон цен
	i64    time    = 0; // время (с обнуленной секундой (time%60 = 0))
	double min     = 0; // минимальная цена
	double max     = 0; // макимальная цена
	double first   = 0; // первая цена
	double last    = 0; // последняя цена
	double c3_buy  = 0; // цена покупки на 3-й секунде
	double c3_sale = 0; // цена продажи на 3-й секунде
};

struct _index_data // все коэффициенты
{
	std::vector<_index> data; // поминутный вектор

	bool update(); // обновить ранные, вызывать после обновления sss
	void start(); // начальная инициализация
	void save_to_file();
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct _candle2 : public _basic_curve2
{
	void draw(i64 n, _area area) override; // нарисовать 1 элемент
	_interval get_y(i64 n) override; // дипазон рисования по y
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

_index_data index; // все расчетные данные

std::vector<_basic_curve*> oracle; // все оракулы и графики

_basic_curve* super_oracle = nullptr; // оракул для предсказания

void add_oracle(_basic_curve* o, bool gr = true, bool sup = false)
{
	o->load_from_file();
	o->recovery();
	oracle.push_back(o);
	if (gr)	graph->curve.push_back(o);
	if (sup) super_oracle = o;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void fun13(_tetron* tt0, _tetron* tt, u64 flags)
{
	static bool first = true; if (!first) return; first = false;

	sss.load_from_file((exe_path + sss_file).c_str());

	if (!graph) return;
	if (!graph->find1<_g_scrollbar>(flag_part))
	{
		_g_scrollbar* sb = new _g_scrollbar;
		sb->vid = 2;
		graph->add_flags(sb, flag_sub_go + flag_part + (flag_run << 32));
	}
	graph->cha_area();

	add_oracle(new _mctds_candle);
	add_oracle(new _oracle3);
	add_oracle(new _oracle5, true, true);
	index.start();
	graph->curve2.push_back(new _candle2);
}

void fun15(_tetron* tt0, _tetron* tt, u64 flags)
{
	for (_frozen i(n_timer1000, flag_run); i; i++)
	{
		_t_function* f = *i;
		if (!f) continue;
		if (f->a == 16) delete f;
	}
	_g_button* b = *tt0;
	if (b->checked) n_timer1000->add_flags(new _t_function(16), flag_run);
}

i64 can_trade    = -6; // разрешенное количество сделок (купить-продать = 2 сделки), отрицательное - нельзя
int vrema_prodat =  0; // время когда нужно продать

void fun16(_tetron* tt0, _tetron* tt, u64 flags)
{
	if (zamok_pokupki) return;
	_prices a;
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
	sss.add(a);
	for (auto i : oracle) i->recovery();
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
	time_t ti = super_oracle->prediction();


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

void fun19(_tetron* tt0, _tetron* tt, u64 flags)
{
	_g_button* b = *tt0;
	can_trade = (b->checked) ? abs(can_trade) : -abs(can_trade);
}

void fun20(_tetron* tt0, _tetron* tt, u64 flags)
{
	if (zamok_pokupki) return;
	zamok_pokupki = true;
	_t_function* fu = new _t_function(35);
	fu->run(0, fu, flag_run);
}

void fun21(_tetron* tt0, _tetron* tt, u64 flags)
{
	if (zamok_pokupki) return;
	zamok_pokupki = true;
	_t_function* fu = new _t_function(36);
	fu->run(0, fu, flag_run);
}

void fun22(_tetron* tt0, _tetron* tt, u64 flags)
{
	graph->size_el++;
	graph->cha_area();
}

void fun30(_tetron* tt0, _tetron* tt, u64 flags)
{
	if (graph->size_el > 1) graph->size_el--;
	graph->cha_area();
}

void fun31(_tetron* tt0, _tetron* tt, u64 flags)
{
	sss.save_to_file((exe_path + sss_file).c_str());
	for (auto i : oracle) i->save_to_file();
	index.save_to_file();
}

void fun35(_tetron* tt0, _tetron* tt, u64 flags)
{
	buy_stock(tt, true);
}

void fun36(_tetron* tt0, _tetron* tt, u64 flags)
{
	buy_stock(tt, false);
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

	i64 ll = curve.size();
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
	_basic_curve::_element_chart* al = new _basic_curve::_element_chart[ll]; // элементы линий
	// 1-й проход - вычисление zmin, zmax
	double zmin = 1E100;
	double zmax = -1E100;
	int timelast = index.data[vib].time;
	for (int i = 0; i < ll; i++) curve[i]->get_t_info(timelast, &al[i]);
	timelast -= period;
	int ke = 0; // количество построенных элементов
	while (ke < k_el)
	{
		int timenext = 2000000000; // следующее время
		for (int i = 0; i < ll; i++)
			if (al[i].n >= 0)
				if (al[i].time < timenext) timenext = al[i].time;
		if (timenext == 2000000000) break;
		ke++;
		timelast = timenext;

		for (int i = 0; i < ll; i++)
		{
			if (al[i].n < 0) continue;
			if (al[i].time == timelast)
			{ // сработало
				if (al[i].min < zmin) zmin = al[i].min;
				if (al[i].max > zmax) zmax = al[i].max;
				curve[i]->get_n_info(al[i].n + 1i64, &al[i]);
			}
		}
	}
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
	time_.clear();
	// 2-й проход - рисование
	timelast = index.data[vib].time;
	for (int i = 0; i < ll; i++) curve[i]->get_t_info(timelast, &al[i]);
	timelast -= period;
	ke = 0; // количество построенных элементов
	while (ke < k_el)
	{
		int timenext = 2000000000; // следующее время
		for (int i = 0; i < ll; i++)
			if (al[i].n >= 0)
				if (al[i].time < timenext) timenext = al[i].time;
		if (timenext == 2000000000) break;
		ke++;
		timelast = timenext;
		time_.push_back(timelast);

		for (int i = 0; i < ll; i++)
		{
			if (al[i].n < 0) continue;
			if (al[i].time == timelast)
			{ // сработало
				double ymi = a.y.max - (al[i].min - zmin) * a.y.length() / (zmax - zmin);
				double yma = a.y.max - (al[i].max - zmin) * a.y.length() / (zmax - zmin);
				double x = r_el * (ke - 1i64) + a.x.min;
				curve[i]->draw(al[i].n, { {x, x + r_el}, {yma, ymi} });
				curve[i]->get_n_info(al[i].n + 1i64, &al[i]);
			}
		}
	}
	delete[] al;
	// рисование сетки
	uint col_setka      = c_max - 0xE0000000; // цвет сетки
	uint col_setka_font = c_def;              // цвет подписи сетки
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
		os_pordis(y_.min, y_.max, maxN, mi, step, sss.c_unpak);
		for (double y = mi; y < y_.max; y += step)
		{
			double yy = a.y.max - (y - y_.min) * a.y.length() / (y_.max - y_.min);
			master_bm.line({ a.x.min + dex, yy }, { a.x.max - dex, yy }, col_setka);
			master_bm.text16(std::max(a.x.min, 0.0) + 2, (i64)(yy - 6), double_to_astring(y, 2), col_setka_font);
			master_bm.text16(std::min((i64)a.x.max, master_bm.size.x) - dex, (i64)(yy - 6), double_to_astring(y, 2),
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
			master_bm.text16(x - 11 + a.x.min, std::min((i64)a.y.max, master_bm.size.y) - 13, s, col_setka_font);
			master_bm.text16(x - 11 + a.x.min, std::max(a.y.min, 0.0), s, col_setka_font);
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
		date_to_ansi_string(mintime).data(), 4, c_max - 0x80000000);
	// рисование количества элементов
	master_bm.text16n(std::max(a.x.min, 0.0) + dex + 10, std::max(a.y.min, 0.0) + 60,
		std::to_string(sss.size).data(), 2, 0x60ff0000);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void calc_all_prediction(_basic_curve& o, i64 &nn, double &kk)
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
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void _index_data::save_to_file()
{
	_stack mem;
	mem << data;
	mem.save_to_file(exe_path + index_file);
}

void _index_data::start()
{
	_stack mem;
	if (mem.load_from_file(exe_path + index_file)) mem >> data;
	update();
}

bool _index_data::update()
{
	i64 vcc = 0;
	if (!data.empty()) vcc = data.back().ncc.max;
	if (vcc == sss.size) return false; // ничего не изменилось
	if (vcc > sss.size)
	{
		data.clear(); // обработанных данных больше, чем исходных, потому пусть будет полный перерасчет
		vcc = 0;
	}
	if (sss.size < 2) return false; // мало данных для обработки
	i64 back_minute = sss.back.time_to_minute();
	if (!data.empty())
	{
		if (back_minute == data.back().time + 1) return false; // еще рано
		if (back_minute <= data.back().time) // так быть не должно, полный перерасчет
		{
			data.clear();
			vcc = 0;
		}
	}
	if (sss.size - vcc < 2) return false; // мало данных для обработки
	_prices cc;
	time_t t = 0;
	_index cp;
	for (i64 i = vcc; i < sss.size; i++)
	{
		sss.read(i, cc);
		time_t t2 = cc.time_to_minute();
		if (t2 == t)
		{
			double aa = (cc.buy[0].value + cc.sale[0].value) * (sss.c_unpak * 0.5);
			if (aa < cp.min) cp.min = aa;
			if (aa > cp.max) cp.max = aa;
			cp.ncc.max++;
			cp.last = aa;
			continue;
		}
		if (t != 0) data.push_back(cp);
		if (t2 == back_minute) break; // последнюю минуту пока не трогать
		t = t2;
		cp.time = t;
		cp.ncc.min = i;
		cp.ncc.max = i + 1;
		cp.max = cp.min = cp.last = cp.first = (cc.buy[0].value + cc.sale[0].value) * (sss.c_unpak * 0.5);
	}
	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void _candle2::draw(i64 n, _area area)
{
	auto aa = &index.data[n];
	double min_ = aa->min * sss.c_unpak;
	double max_ = aa->max * sss.c_unpak;
	double first_ = aa->first * sss.c_unpak;
	double last_ = aa->last * sss.c_unpak;

	_iinterval xx = area.x;
	xx.min++;
	xx.max--;
	xx.min++;
	xx.max--;
	if (xx.empty()) return;

	constexpr uint col_rost = 0xff0000ff;// 0xFF28A050; // цвет ростущей свечки
	constexpr uint col_pade = 0xffff0000;// 0xFF186030; // цвет падающей свечки
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

_interval _candle2::get_y(i64 n)
{
	auto a = &index.data[n];
	return { a->min, a->max };
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
