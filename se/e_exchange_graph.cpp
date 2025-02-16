﻿/*//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

рисование ломанной линии
выделить нулевые оси у графика
не подписывать сетку < 0, если все данные > 0 (и наоборот)
сделать отображение секунд при увеличении графика цен
преобразование координат при выводе графика

~ коэфиициент - насколько числа красивые? у кого красивее - у покупки или у продажи? (у всех и у 10-15)

распределение "вероятностей" вместо одного числа 0..+5 -> 0.2, +5..+10 -> 0.8, ... +50..+inf -> 0.01%

/*//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "e_exchange_graph.h"
#include "exchange_data.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void load_se()
{
	if (ed.empty()) ed.load_from_file();
	update_index_data();
	if (graph2) graph2->cha_area();
}

void expand_elements_graph2()
{
	graph2->size_el++;
	graph2->cha_area();
}

void narrow_graph_elements2()
{
	if (graph2->size_el > 1) graph2->size_el--;
	graph2->cha_area();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

_e_exchange_graph::_e_exchange_graph(_ui* ui_) : _ui_element(ui_)
{
	graph2 = this;
	local_area = { {0, 200}, {0, 100} };
}

_e_exchange_graph::~_e_exchange_graph()
{
	for (auto i : curve) delete i;
}

bool _e_exchange_graph::mouse_down_left2(_xy r)
{
	x_tani = (i64)r.x;
	return true;
}

void _e_exchange_graph::mouse_move_left2(_xy r)
{
	i64 dx = ((i64)r.x - x_tani) / size_el;
	if (dx == 0) return;
	x_tani += dx * size_el;

	auto polz = find1<_e_scrollbar>();
	if (!polz) return;
	double* ii = &polz->position;
	if (!ii) return;
	*ii -= double(dx) / v_vib;
	if (*ii < 0) *ii = 0;
	if (*ii > 1) *ii = 1;
	polz->cha_area();
	update();
}

void _e_exchange_graph::update()
{
	cha_area();
}

void _e_exchange_graph::draw(_trans tr)
{
	_area a = tr(local_area);

	if (curve.empty())
	{
		ui->canvas.rectangle(a, ui->cc1);
		return;
	}

	_interval y_; // диапазон у (grid)
	static std::vector<time_t> time_; // отсчеты времени (grid)

	double polzi_ = 0; // !! ползунок

	auto sb = find1<_e_scrollbar>();
	if (sb)	polzi_ = sb->position;

	i64 ll2 = curve.size();

	i64 k_el = i64(local_area.x.length() / size_el);
	if (k_el < 1) return;
	double r_el = a.x.length() / k_el;

	i64 n = index_data.size();
	if (n == 0) return;
	v_vib = int(n - 1);
	if (v_vib < 0) v_vib = 0;
	int vib = (int)(polzi_ * v_vib + 0.5); // !! ползунок

	int period = 60;
	// 1-й проход - вычисление zmin, zmax
	double zmin = 1E100;
	double zmax = -1E100;
	for (i64 i = 0; i < k_el; i++)
	{
		i64 ii = i + vib;
		if (ii >= (i64)index_data.size()) break;
		for (i64 j = 0; j < ll2; j++)
		{
			_interval il = curve[j]->get_y(ii);
			if (il.min < zmin) zmin = il.min;
			if (il.max > zmax) zmax = il.max;
		}
	}
	if (zmin == zmax) zmax = zmin + 1.0;
	y_ = { zmin, zmax };
	time_.clear();
	// 2-й проход - рисование
	for (i64 i = 0; i < k_el; i++)
	{
		i64 ii = i + vib;
		if (ii >= (i64)index_data.size()) break;
		time_.push_back(index_data[ii].time);
		for (i64 j = 0; j < ll2; j++)
		{
			_interval il = curve[j]->get_y(ii);
			double ymi = a.y.max - (il.min - zmin) * a.y.length() / (zmax - zmin);
			double yma = a.y.max - (il.max - zmin) * a.y.length() / (zmax - zmin);
			double x = r_el * i + a.x.min;
			curve[j]->draw(ui->canvas, ii, { {x, x + r_el}, {yma, ymi} });
		}
	}
	// рисование сетки
	uint col_setka = ui->cc2 - 0xE0000000; // цвет сетки
	uint col_setka_font = ui->cc1;              // цвет подписи сетки
	if (time_.size() < 1)
	{
		ui->canvas.line({ a.x.min, a.y.min }, { a.x.max, a.y.max }, 0xFF800000);
		ui->canvas.line({ a.x.min, a.y.max }, { a.x.max, a.y.min }, 0xFF800000);
		return;
	}
	// рисование горизонтальных линий сетки с подписями
	i64 dex = 33; // длина подписи
	i64 maxN = i64(a.y.length() / 15);
	if (maxN > 1)
	{
		double mi, step;
		os_pordis(y_.min, y_.max, maxN, mi, step, c_unpak);
		for (double y = mi; y < y_.max; y += step)
		{
			double yy = a.y.max - (y - y_.min) * a.y.length() / (y_.max - y_.min);
			ui->canvas.line({ a.x.min + dex, yy }, { a.x.max - dex, yy }, col_setka);
			ui->canvas.text16({ std::max(a.x.min, 0.0) + 2, (i64)(yy - 6) }, double_to_string(y, 2), col_setka_font);
			ui->canvas.text16({ std::min((i64)a.x.max, ui->canvas.get_size().x) - dex, (i64)(yy - 6) }, double_to_string(y, 2),
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
	int ost[] = { 60, 60, 24, 32, 12, 1000 };
	int ido = 0;
	if (stept % 33177600) ido = 4;
	if (stept % 2764800) ido = 3;
	if (stept % 86400) ido = 2;
	if (stept % 3600) ido = 1;
	if (stept % 60) ido = 0;
	std::string s = "00:00";
	time_t mintime = 0;
	time_t pr_time = 0;
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
			ui->canvas.line({ i64(a.x.min + x), a.y.min }, { a.x.min + x, a.y.max }, cl);
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
			ui->canvas.text16({ x - 11 + a.x.min, std::min((i64)a.y.max, ui->canvas.get_size().y) - 13 }, s, col_setka_font);
			ui->canvas.text16({ x - 11 + a.x.min, std::max(a.y.min, 0.0) }, s, col_setka_font);
			continue;
		}
		if (sca)
		{
			double x = rel * i;
			if ((x <= dex) || (x >= a.x.length() - dex)) continue;
			ui->canvas.line({ a.x.min + x, a.y.min }, { a.x.min + x, a.y.max }, 0x80FF0000);
		}
	}
	// рисование даты
	ui->canvas.text16n(std::max(i64(a.x.min), 0i64) + dex + 10, std::max(i64(a.y.min), 0i64) + 11,
		date_to_ansi_string(mintime).data(), 4, ui->cc2 - 0x80000000, 0xA0000000);
	// рисование количества элементов
	ui->canvas.text16n(std::max(i64(a.x.min), 0i64) + dex + 10, std::max(i64(a.y.min), 0i64) + 61,
		std::to_string(ed.size()).data(), 2, 0x80ff0000, 0xA0000000);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void _candle_curve2::draw(_bitmap& bm, i64 n, _area area)
{
	auto aa = &index_data[n];
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
		bm.fill_rectangle(_iarea{ xx, {yla, yfi} }, { col_rost });
		bm.line({ xx.center(), area.y.max }, { xx.center(), area.y.min }, col_rost);
	}
	else
	{
		bm.fill_rectangle(_iarea{ xx, {yfi, yla} }, { col_pade });
		bm.line({ xx.center(), area.y.max }, { xx.center(), area.y.min }, col_pade);
	}
}

_interval _candle_curve2::get_y(i64 n)
{
	auto a = &index_data[n];
	return { a->min, a->max };
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void _prices_curve3::draw(_bitmap& bm, i64 n, _area area)
{
	static _supply_and_demand pri[61]; // цены
	double min, max; // разброс по y
	for (auto& i : pri) i.clear();

	for (i64 i = index_data[n].ncc.min; i < index_data[n].ncc.max; i++)
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
		if (part_ss[ii].empty()) part_ss[ii] = ed[i];
		pri[part_ss[ii].time % 60] = part_ss[ii];
	}
	min = index_data[n].minmin - c_unpak;
	max = index_data[n].maxmax;
	_iinterval xx = area.x;
	xx.min++;
	i64 dx = xx.length();
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
		if (pri[ss_].supply[3].price < pri[ss_].demand[3].price)
		{
			//		xx1++;
		}
		for (int j = size_offer - 1; j >= 0; j--)
		{
			double ce = pri[ss_].supply[j].price * c_unpak;
			_iinterval yy(area.y.min + (max - ce) * ddy / dd, area.y.min + (max - ce + c_unpak) * ddy / dd);
			yy.min++;
			yy.max--;
			if (yy.empty()) continue;
			uint q = (uint)sqrt(pri[ss_].supply[j].number) + 32;
			if (q > 255) q = 255;
			uint cc = (q << 8) + (q << 16) + 0x60000000;
			bm.fill_rectangle(_iarea{ {xx1, xx2}, yy }, { cc });
		}
		for (int j = 0; j < size_offer; j++)
		{
			double ce = pri[ss_].demand[j].price * c_unpak;
			_iinterval yy(area.y.min + (max - ce) * ddy / dd, area.y.min + (max - ce + c_unpak) * ddy / dd);
			yy.min++;
			yy.max--;
			if (yy.empty()) continue;
			uint q = (uint)sqrt(pri[ss_].demand[j].number) + 32;
			if (q > 255) q = 255;
			uint cc = q + (q << 8) + 0x60000000;
			bm.fill_rectangle(_iarea{ {xx1, xx2}, yy }, { cc });
		}
	}
}

_interval _prices_curve3::get_y(i64 n)
{
	auto a = &index_data[n];
	return { a->minmin - c_unpak, a->maxmax };
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void _prices_curve4::draw(_bitmap& bm, i64 n, _area area)
{
	static _supply_and_demand pri[61]; // цены
	double min, max; // разброс по y
	for (auto& i : pri) i.clear();

	for (i64 i = index_data[n].ncc.min; i < index_data[n].ncc.max; i++)
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
		if (part_ss[ii].empty()) part_ss[ii] = ed[i];
		pri[part_ss[ii].time % 60] = part_ss[ii];
	}
	min = index_data[n].minmin - c_unpak;
	max = index_data[n].maxmax;
	_iinterval xx = area.x;
	xx.min++;
	i64 dx = xx.length();
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
			double ce = pri[ss_].supply[j].price * c_unpak;
			_iinterval yy(area.y.min + (max - ce) * ddy / dd, area.y.min + (max - ce + c_unpak) * ddy / dd);
			yy.min++;
			yy.max--;
			if (yy.empty()) continue;

			uint cc = 0x40ffffff;
			if (ss_pr >= 0)
			{
				if (pri[ss_].supply[j].number > pri[ss_pr].supply[j].number) cc = 0x9000ff00;
				if (pri[ss_].supply[j].number < pri[ss_pr].supply[j].number) cc = 0x70ff0000;
			}
			if (cc != 0x40ffffff) bm.fill_rectangle(_iarea{ {xx1, xx2}, yy }, { cc });
		}
		ss_pr = ss_;
	}
}

_interval _prices_curve4::get_y(i64 n)
{
	auto a = &index_data[n];
	return { a->minmin - c_unpak, a->maxmax };
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

