#include "tetron.h"
#include "sable_stat.h"
#include "oracle2.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

constexpr wchar_t oracle2_file[] = L"..\\..\\sable\\oracle2.bin";

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

i64 _nervous_oracle2::prediction()
{
	// return i64(rnd(15000) == 13) * 60; // случайный
	if (zn.size() < 10) return 0;
	if (zn.back().time + 60 != sss.last_cc.time_to_minute()) return 0;
	return get_latest_events(zn.size() - 1).start();
}

void _nervous_oracle2::get_n_info(i64 n, _element_chart* e)
{
	if (n >= (i64)zn.size())
	{
		e->n = -1;
		return;
	}
	e->n = n;
	e->time = zn[n].time;
	e->min = e->max = ((double)zn[n].max_pro + (double)zn[n].min_pok) * 0.5 * sss.c_unpak;
}

void _nervous_oracle2::get_t_info(int t, _element_chart* e)
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
	e->min = e->max = ((double)zn[xx].max_pro + (double)zn[xx].min_pok) * 0.5 * sss.c_unpak;
}

_latest_events _nervous_oracle2::get_latest_events(i64 nn)
{
	const i64 k = 4;
	_latest_events e;
	e.event[0] = e.event[1] = e.event[2] = e.event[3] = 0;
	e.minute[0] = e.minute[1] = e.minute[2] = e.minute[3] = 0;
	e.x[0] = e.x[1] = e.x[2] = e.x[3] = 0.0;
	if (nn < 10) return e;
	i64 ii = std::max(k, nn - 40);
	int ee = 0;
	for (i64 n = nn; n >= ii; n--)
	{
		if ((i64)zn[n].time - zn[n - k].time != k * 60) continue;
		bool rost_pro = true;
		bool rost_pok = true;
		bool pade_pro = true;
		bool pade_pok = true;
		for (i64 i = n - k; i < n; i++)
		{
			if (zn[i].r_pro >= zn[i + 1].r_pro) rost_pro = false;
			if (zn[i].r_pok >= zn[i + 1].r_pok) rost_pok = false;
			if (zn[i].r_pro <= zn[i + 1].r_pro) pade_pro = false;
			if (zn[i].r_pok <= zn[i + 1].r_pok) pade_pok = false;
		}
		char a = 0;
		if ((rost_pro || rost_pok) && (pade_pro || pade_pok))
			a = 7;
		else
		{
			if (rost_pok) a += 1;
			if (rost_pro) a += 2;
			if (pade_pro || pade_pok) a += 3;
			if (pade_pok) a += 1;
			if (pade_pro) a += 2;
		}
		if (a == 0) continue;
		e.event[ee] = a;
		e.minute[ee] = (int)(nn - n);
		e.x[ee] = ((double)zn[n].max_pro + (double)zn[n].min_pok) * 0.5 * sss.c_unpak;
		ee++;
		if (ee == 4) break;
	}
	return e;
}

void _nervous_oracle2::draw(i64 n, _area area)
{
	const i64 k = 4;
	if (n < k) return;
	if ((i64)zn[n].time - zn[n - k].time != k * 60) return;
	bool rost_pro = true;
	bool rost_pok = true;
	bool pade_pro = true;
	bool pade_pok = true;
	for (i64 i = n - k; i < n; i++)
	{
		if (zn[i].r_pro >= zn[i + 1].r_pro) rost_pro = false;
		if (zn[i].r_pok >= zn[i + 1].r_pok) rost_pok = false;
		if (zn[i].r_pro <= zn[i + 1].r_pro) pade_pro = false;
		if (zn[i].r_pok <= zn[i + 1].r_pok) pade_pok = false;
	}
	uint c = 0xFF808080;

	if (rost_pro) c += 0x70;
	if (rost_pok) c += 0x700000;
	if (pade_pro) c -= 0x70;
	if (pade_pok) c -= 0x700000;

	double r = area.x.length() * 0.5 * 2;
	if (get_latest_events(n).start())
	{
		c = 0xFFFF0000;
		r *= 2;
	}

	if (c == 0xFF808080) return;

	master_bm.fill_ring(area.center(), r, r * 0.1, c, c);
}

void _nervous_oracle2::recovery()
{
	i64 vcc = 0;
	if (zn.size()) vcc = zn.back().ncc.max;
	i64 ssvcc = sss.size;
	if (ssvcc == vcc) return; // ничего не изменилось
	if (vcc < ssvcc) // добавились несколько цен
	{
		_prices cc;
		_sable_stat::_info_pak inf;
		int t = 0;
		_element_nervous* cp = 0;
		if (zn.size())
		{
			cp = &zn.back();
			if (cp->v_r)
			{
				cp->r *= cp->v_r;
				cp->r_pok *= cp->v_r;
				cp->r_pro *= cp->v_r;
			}
			t = cp->time;
		}
		for (i64 i = vcc; i < ssvcc; i++)
		{
			sss.read(i, cc, &inf);
			int t2 = cc.time_to_minute();
			if (t2 != t)
			{
				t = t2;
				if (cp)
				{
					if (cp->v_r)
					{
						cp->r /= cp->v_r;
						cp->r_pok /= cp->v_r;
						cp->r_pro /= cp->v_r;
					}
				}
				_element_nervous we;
				we.time = t;
				we.ncc.min = i;
				we.ncc.max = i + 1;
				we.max_pok = we.min_pok = cc.buy[0].value;
				we.max_pro = we.min_pro = cc.sale[0].value;
				if (inf.ok)
				{
					we.v_r = 1;
					we.r = inf.r;
					we.r_pok = inf.r_pok;
					we.r_pro = inf.r_pro;
				}
				else
					we.v_r = 0;
				zn.push_back(we);
				cp = &zn.back();
			}
			else
			{
				if (cp == 0) continue; // для паранойи компилятора
				if (cc.buy[0].value < cp->min_pok) cp->min_pok = cc.buy[0].value;
				if (cc.buy[0].value > cp->max_pok) cp->max_pok = cc.buy[0].value;
				if (cc.sale[0].value < cp->min_pro) cp->min_pro = cc.sale[0].value;
				if (cc.sale[0].value > cp->max_pro) cp->max_pro = cc.sale[0].value;
				cp->ncc.max++;
				if (inf.ok)
				{
					if (cp->v_r)
					{
						cp->v_r++;
						cp->r += inf.r;
						cp->r_pok += inf.r_pok;
						cp->r_pro += inf.r_pro;
					}
					else
					{
						cp->v_r = 1;
						cp->r = inf.r;
						cp->r_pok = inf.r_pok;
						cp->r_pro = inf.r_pro;
					}
				}
			}
		}
		if (cp)
		{
			if (cp->v_r)
			{
				cp->r /= cp->v_r;
				cp->r_pok /= cp->v_r;
				cp->r_pro /= cp->v_r;
			}
		}
		return;
	}
	_prices cc; // уменьшились цены, полный пересчет
	_sable_stat::_info_pak inf;
	zn.clear();
	int t = 0;
	_element_nervous* cp = 0;
	for (i64 i = 0; i < ssvcc; i++)
	{
		sss.read(i, cc, &inf);
		int t2 = cc.time_to_minute();
		if (t2 != t)
		{
			t = t2;
			if (cp)
			{
				if (cp->v_r)
				{
					cp->r /= cp->v_r;
					cp->r_pok /= cp->v_r;
					cp->r_pro /= cp->v_r;
				}
			}
			_element_nervous we;
			we.time = t;
			we.ncc.min = i;
			we.ncc.max = i + 1;
			we.max_pok = we.min_pok = cc.buy[0].value;
			we.max_pro = we.min_pro = cc.sale[0].value;
			if (inf.ok)
			{
				we.v_r = 1;
				we.r = inf.r;
				we.r_pok = inf.r_pok;
				we.r_pro = inf.r_pro;
			}
			else
				we.v_r = 0;
			zn.push_back(we);
			cp = &zn.back();
		}
		else
		{
			if (cp == 0) continue; // для паранойи компилятора
			if (cc.buy[0].value < cp->min_pok) cp->min_pok = cc.buy[0].value;
			if (cc.buy[0].value > cp->max_pok) cp->max_pok = cc.buy[0].value;
			if (cc.sale[0].value < cp->min_pro) cp->min_pro = cc.sale[0].value;
			if (cc.sale[0].value > cp->max_pro) cp->max_pro = cc.sale[0].value;
			cp->ncc.max++;
			if (inf.ok)
			{
				if (cp->v_r)
				{
					cp->v_r++;
					cp->r += inf.r;
					cp->r_pok += inf.r_pok;
					cp->r_pro += inf.r_pro;
				}
				else
				{
					cp->v_r = 1;
					cp->r = inf.r;
					cp->r_pok = inf.r_pok;
					cp->r_pro = inf.r_pro;
				}
			}
		}
	}
	if (cp)
	{
		if (cp->v_r)
		{
			cp->r /= cp->v_r;
			cp->r_pok /= cp->v_r;
			cp->r_pro /= cp->v_r;
		}
	}
}

void _nervous_oracle2::save_to_file()
{
	_stack mem;
	mem << zn;
	mem.save_to_file(exe_path + oracle2_file);
}

void _nervous_oracle2::load_from_file()
{
	_stack mem;
	if (!mem.load_from_file(exe_path + oracle2_file)) return;
	mem >> zn;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

