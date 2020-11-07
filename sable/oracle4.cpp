#include "sable_stat.h"
#include "oracle4.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

constexpr wchar_t oracle4_file[] = L"..\\..\\sable\\oracle4.bin";

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void _view_stat::get_n_info(i64 n, _element_chart* e)
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

void _view_stat::get_t_info(time_t t, _element_chart* e)
{
	auto x = lower_bound(cen1m.begin(), cen1m.end(), t);
	if (x == cen1m.end())
	{
		e->n = -1;
		return;
	}
	i64 xx = (x - cen1m.begin());
	e->n = xx;
	e->time = cen1m[xx].time;
	e->min = cen1m[xx].min * sss.c_unpak;
	e->max = cen1m[xx].max * sss.c_unpak;
}

void _view_stat::draw(i64 n, _area area)
{
	double r = cen1m[n].k * area.x.length() * 0.15;
	uint c = 0x80ff0000;
	master_bm.fill_ring(area.center(), r, r * 0.1, c, c);
}

void _view_stat::recovery()
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
			t = cp->time;
		}
		for (i64 i = vcc; i < ssvcc; i++)
		{
			sss.read(i, cc);
			int t2 = cc.time_to_minute();
			if (t2 != t)
			{
				t = t2;
				_cen_pak we;
				we.time = t;
				we.ncc.min = i;
				we.ncc.max = i + 1;
				we.min = ((int)cc.buy[0].value + (int)cc.sale[0].value) / 2;
				we.max = we.min;
				we.k = (cc.sale[1].number == 570);
				cen1m.push_back(we);
				cp = &cen1m.back();
			}
			else
			{
				if (cp == 0) continue; // для паранойи компилятора
				int aa = ((int)cc.buy[0].value + (int)cc.sale[0].value) / 2;
				if (aa < cp->min) cp->min = aa;
				if (aa > cp->max) cp->max = aa;
				if (cc.sale[1].number == 570) cp->k++;
				cp->ncc.max++;
			}
		}
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
			_cen_pak we;
			we.time = t;
			we.ncc.min = i;
			we.ncc.max = i + 1;
			we.min = ((int)cc.buy[0].value + (int)cc.sale[0].value) / 2;
			we.max = we.min;
			we.k = (cc.sale[1].number == 570);
			cen1m.push_back(we);
			cp = &cen1m.back();
		}
		else
		{
			if (cp == 0) continue; // для паранойи компилятора
			int aa = ((int)cc.buy[0].value + (int)cc.sale[0].value) / 2;
			if (aa < cp->min) cp->min = aa;
			if (aa > cp->max) cp->max = aa;
			if (cc.sale[1].number == 570) cp->k++;
			cp->ncc.max++;
		}
	}
}

void _view_stat::save_to_file()
{
	_stack mem;
	mem << cen1m;
	mem.save_to_file(exe_path + oracle4_file);
}

void _view_stat::load_from_file()
{
	_stack mem;
	if (!mem.load_from_file(exe_path + oracle4_file)) return;
	mem >> cen1m;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
