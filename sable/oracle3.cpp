#include "oracle3.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

constexpr wchar_t oracle3_file[] = L"..\\..\\sable\\oracle3.bin";

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void _oracle3::get_n_info(i64 n, _element_chart* e)
{
	if (n >= (i64)zn.size())
	{
		e->n = -1;
		return;
	}
	e->n = n;
	e->time = zn[n].time;
	e->min = (zn[n].min - 1) * sss.c_unpak;
	e->max = zn[n].max * sss.c_unpak;
}

void _oracle3::get_t_info(time_t t, _element_chart* e)
{
	auto x = lower_bound(zn.begin(), zn.end(), t);
	if (x == zn.end())
	{
		e->n = -1;
		return;
	}
	int xx = (int)(x - zn.begin());
	e->n = xx;
	e->time = zn[xx].time;
	e->min = (zn[xx].min - 1) * sss.c_unpak;
	e->max = zn[xx].max * sss.c_unpak;
}

void _oracle3::recovery()
{
	i64 vcc = 0;
	if (zn.size()) vcc = zn.back().ncc.max;
	i64 ssvcc = sss.size;
	if (ssvcc == vcc) return; // ничего не изменилось
	if (vcc < ssvcc) // добавились несколько цен
	{
		_prices cc;
		int t = 0;
		_element_oracle* cp = 0;
		if (zn.size())
		{
			cp = &zn.back();
			t = cp->time;
		}
		for (i64 i = vcc; i < ssvcc; i++)
		{
			sss.read(i, cc);
			int t2 = cc.time_to_minute();
			if (t2 != t)
			{
				t = t2;
				_element_oracle we;
				we.time = t;
				we.ncc.min = i;
				we.ncc.max = i + 1;
				we.max = cc.sale[roffer - 1].value;
				we.min = cc.buy[roffer - 1].value;
				zn.push_back(we);
				cp = &zn.back();
			}
			else
			{
				if (cp == 0) continue; // для паранойи компилятора
				if (cc.buy[roffer - 1].value < cp->min) cp->min = cc.buy[roffer - 1].value;
				if (cc.sale[roffer - 1].value > cp->max) cp->max = cc.sale[roffer - 1].value;
				cp->ncc.max++;
			}
		}
		return;
	}
	_prices cc; // уменьшились цены, полный пересчет
	zn.clear();
	int t = 0;
	_element_oracle* cp = 0;
	for (i64 i = 0; i < ssvcc; i++)
	{
		sss.read(i, cc);
		int t2 = cc.time_to_minute();
		if (t2 != t)
		{
			t = t2;
			_element_oracle we;
			we.time = t;
			we.ncc.min = i;
			we.ncc.max = i + 1;
			we.max = cc.sale[roffer - 1].value;
			we.min = cc.buy[roffer - 1].value;
			zn.push_back(we);
			cp = &zn.back();
		}
		else
		{
			if (cp == 0) continue; // для паранойи компилятора
			if (cc.buy[roffer - 1].value < cp->min) cp->min = cc.buy[roffer - 1].value;
			if (cc.sale[roffer - 1].value > cp->max) cp->max = cc.sale[roffer - 1].value;
			cp->ncc.max++;
		}
	}
}

void _oracle3::draw(i64 n, _area area)
{
	static _prices pri[61]; // цены
	static i64 min, max; // разброс по y
	min = 0;
	max = 1;
	for (auto& i : pri) i.clear();

	for (i64 i = zn[n].ncc.min; i < zn[n].ncc.max; i++)
	{
		if (i < begin_ss)
		{
			i64 delta = begin_ss - i;
			if (delta >= max_part)
				part_ss.clear();
			else
			{
				_prices w;
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
			_prices w;
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
		if (part_ss[ii].empty()) sss.read(i, part_ss[ii]);
		pri[part_ss[ii].time % 60] = part_ss[ii];
		min = zn[n].min - 1;
		max = zn[n].max;
	}
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
	i64 dd = max - min;
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
		if (pri[ss_].sale[3].value < pri[ss_].buy[3].value)
		{
	//		xx1++;
		}
		for (int j = roffer - 1; j >= 0; j--)
		{
			i64 ce = pri[ss_].sale[j].value;
			_iinterval yy(area.y.min + (max - ce) * ddy / dd, area.y.min + (max - ce + 1) * ddy / dd);
			yy.min++;
			yy.max--;
			if (yy.empty()) continue;
			uint q = (uint)sqrt(pri[ss_].sale[j].number) + 32;
			if (q > 255) q = 255;
			uint cc = (q << 8) + (q << 16) + 0x60000000;
			if ((pri[ss_].sale[j].number == 250)) cc = 0xffff00ff;
			if ((pri[ss_].sale[j].number == 250) && (j == 0)) cc = 0xffffffff;
			master_bm.fill_rectangle({ {xx1, xx2}, yy }, cc);
		}
		for (int j = 0; j < roffer; j++)
		{
			i64 ce = pri[ss_].buy[j].value;
			_iinterval yy(area.y.min + (max - ce) * ddy / dd, area.y.min + (max - ce + 1) * ddy / dd);
			yy.min++;
			yy.max--;
			if (yy.empty()) continue;
			uint q = (uint)sqrt(pri[ss_].buy[j].number) + 32;
			if (q > 255) q = 255;
			uint cc = q + (q << 8) + 0x60000000;
			master_bm.fill_rectangle({ {xx1, xx2}, yy }, cc);
		}
	}
}

void _oracle3::save_to_file()
{
	_stack mem;
	mem << zn;
	mem.save_to_file(exe_path + oracle3_file);
}

void _oracle3::load_from_file()
{
	_stack mem;
	if (!mem.load_from_file(exe_path + oracle3_file)) return;
	mem >> zn;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

