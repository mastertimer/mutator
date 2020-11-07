#include "tetron.h"
#include "sable_stat.h"
#include "oracle0.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

constexpr wchar_t oracle0_file[] = L"..\\..\\sable\\oracle0.bin";

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void _mctds_candle::get_n_info(i64 n, _element_chart* e)
{
	if (n >= (i64)cen1m.size())
	{
		e->n = -1;
		return;
	}
	e->n = n;
	e->time = cen1m[n].time;
	e->min = cen1m[n].min * sss.c_unpak;
	e->max = cen1m[n].max * sss.c_unpak;
}

void _mctds_candle::get_t_info(int t, _element_chart* e)
{
	e->n = -1; // !! написать, когда потребуется!!
}

void _mctds_candle::draw(i64 n, _area area)
{
	double min_ = cen1m[n].min * sss.c_unpak;
	double max_ = cen1m[n].max * sss.c_unpak;
	double first_ = cen1m[n].first * sss.c_unpak;
	double last_ = cen1m[n].last * sss.c_unpak;

	_iinterval xx = area.x;
	xx.min++;
	xx.max--;
	if (xx.empty()) return;

	constexpr uint col_rost = 0xFF28A050; // цвет ростущей свечки
	constexpr uint col_pade = 0xFF186030; // цвет падающей свечки
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

void _mctds_candle::recovery()
{
	i64 vcc = 0;
	if (cen1m.size()) vcc = cen1m.back().ncc.max;
	i64 ssvcc = sss.size;
	if (ssvcc == vcc) return; // ничего не изменилось
	if (vcc < ssvcc) // добавились несколько цен
	{
		_prices cc;
		int t = 0;
		_cen_pak* cp = 0;
		if (cen1m.size())
		{
			cp = &cen1m.back();
			cp->cc *= ((double)cp->ncc.max - cp->ncc.min);
			t = cp->time;
		}
		for (i64 i = vcc; i < ssvcc; i++)
		{
			sss.read(i, cc);
			int t2 = cc.time_to_minute();
			if (t2 != t)
			{
				t = t2;
				if (cp)	cp->cc /= ((double)cp->ncc.max - cp->ncc.min);
				_cen_pak we;
				we.time = t;
				we.ncc.min = i;
				we.ncc.max = i + 1;
				we.first = ((int)cc.buy[0].value + (int)cc.sale[0].value) / 2;
				we.last = we.first;
				we.min = we.first;
				we.max = we.first;
				we.cc = we.first;
				cen1m.push_back(we);
				cp = &cen1m.back();
			}
			else
			{
				if (cp == 0) continue; // для паранойи компилятора
				int aa = ((int)cc.buy[0].value + (int)cc.sale[0].value) / 2;
				cp->cc += aa;
				if (aa < cp->min) cp->min = aa;
				if (aa > cp->max) cp->max = aa;
				cp->ncc.max++;
				cp->last = aa;
			}
		}
		if (cp)	cp->cc /= ((double)cp->ncc.max - cp->ncc.min);
		return;
	}
	_prices cc; // уменьшились цены, полный пересчет
	cen1m.clear();
	int t = 0;
	_cen_pak* cp = 0;
	for (i64 i = 0; i < ssvcc; i++)
	{
		sss.read(i, cc);
		int t2 = cc.time_to_minute();
		if (t2 != t)
		{
			t = t2;
			if (cp)	cp->cc /= ((double)cp->ncc.max - cp->ncc.min);
			_cen_pak we;
			we.time = t;
			we.ncc.min = i;
			we.ncc.max = i + 1;
			we.first = ((int)cc.buy[0].value + (int)cc.sale[0].value) / 2;
			we.last = we.first;
			we.min = we.first;
			we.max = we.first;
			we.cc = we.first;
			cen1m.push_back(we);
			cp = &cen1m.back();
		}
		else
		{
			if (cp == 0) continue; // для паранойи компилятора
			int aa = ((int)cc.buy[0].value + (int)cc.sale[0].value) / 2;
			cp->cc += aa;
			if (aa < cp->min) cp->min = aa;
			if (aa > cp->max) cp->max = aa;
			cp->ncc.max++;
			cp->last = aa;
		}
	}
	if (cp)	cp->cc /= ((double)cp->ncc.max - cp->ncc.min);
}

void _mctds_candle::save_to_file()
{
	_stack mem;
	mem << cen1m;
	mem.save_to_file(exe_path + oracle0_file);
}

void _mctds_candle::load_from_file()
{
	_stack mem;
	if (!mem.load_from_file(exe_path + oracle0_file)) return;
	mem >> cen1m;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
