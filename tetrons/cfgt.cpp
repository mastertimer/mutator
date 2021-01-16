#include <math.h>

#include "cfgt.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void _view_tetron::ris2(_trans tr, bool final)
{
	_area my_area = tr(local_area);
	_xy center = my_area.center();
	double radius = my_area.radius();
	double width_ring = log(1.001 + radius) * 0.5;
	double width_rim = radius * 0.2;
	uint color = c_min;
	master_bm.fill_ring(center, radius, width_ring, color, c_background);

//	_tetron* main_tetron = find1<_tetron>(flag_specialty);
//	if (!main_tetron) return;

	master_bm.fill_ring(center, radius - width_rim, width_ring, color, c_background);

	i64 links_count = 222;
//	for (auto& i : main_tetron->link)
//		if (!(*i)(main_tetron)->test_flags(n_temp_go, flag_parent)) links_count++;

	constexpr double kr = 0.333; // 0.4 растояние между тетронами = kr * radius
	i64 orbits_count = 0;
	double radius_mini_tetrons = 0;
	for (;;)
	{
		orbits_count++;
		double r_max = (width_rim - width_ring) / (orbits_count * (2.0 + kr));
//		double radius0 = radius - 3 * r_max - kr * r_max;
		double radius0 = radius - width_ring - (width_rim - width_ring) * (orbits_count - 1) / orbits_count - r_max;
		i64 n_max = orbits_count * pi / asin(r_max / radius0);
		double rr = r_max;
		if (links_count > n_max)
		{
			i64 nn = (links_count + orbits_count - 1) / orbits_count;
//			rr = radius0 * sin(pi / nn);
			rr = (radius - width_ring - (width_rim - width_ring) * (orbits_count - 1) / orbits_count) * sin(pi / nn) / (1 + sin(pi / nn));
		}
		if (rr < radius_mini_tetrons)
		{
			orbits_count--;
			break;
		}
		radius_mini_tetrons = rr;
	}
	i64 nn = (links_count + orbits_count - 1) / orbits_count;
	for (i64 i = 0; i < links_count; i++)
	{
		i64 number_orbit = i / nn;
		i64 j = i % nn;
		_xy v = { radius - width_ring - (width_rim - width_ring) * (orbits_count - number_orbit - 1) / orbits_count - radius_mini_tetrons, 0.0 };
		master_bm.fill_ring(center + v.rotation(-2 * pi * (j + 0.5 * (number_orbit & 1)) / nn), radius_mini_tetrons, width_ring * 0.5, 0xffff0000, c_background);
	}
}

/*void _view_tetron::ris2(_trans tr, bool final)
{
	_area a = tr(local_area);
	_xy p = a.center();
	double r = a.radius();
	double d = log(1.001 + r) * 0.5;
	//	double y = 0.7 * (r - d) * 2;
	uint c = c_min;
	master_bm.fill_ring(p, r, d, c, c_background);
	_tetron* t2 = find1<_tetron>(flag_specialty); // связанный тетрон
	if (final || (t2 == nullptr))
	{
		mo.clear();
		return;
	}

	if (mo.empty())
	{
		SuperDelTetron2::PodgSpUnikSv(t2);
		SuperDelTetron2::ud->push_back(t2); // т.к. в списке главного тетрона нет

		for (auto i : *SuperDelTetron2::ud)
		{
			_molecule m;
			m.tetron = i;
			mo.push_back(m);
		}
				//for (auto i : *SuperDelTetron2::ud)
				//{
				//	molecule m;
				//	m.c = C_MIN;
				//	for (int j = 0; j < i->vlink_; j++)
				//	{
				//		Tetron* a = i->link_[j].tetron_;
				//		if (!a) continue;
				//		m.tetron = a;
				//		m.p.x = (prng.Random(2001) * 0.001 - 1.0);
				//		m.p.y = (prng.Random(2001) * 0.001 - 1.0);
				//		mo.insert(m);
				//	}
				//}
		SuperDelTetron2::FreeBank();
		int k = (int)mo.size();
		int ki = 0;
		for (auto& i : mo)
		{
			_t_trans* ttr = new _t_trans;
			i.tr_t = ttr;
			ttr->trans.offset = local_area.center() + _xy{ 10 * cos(ki * pi * 2 / k), 10 * sin(ki * pi * 2 / k) };
			ttr->trans.scale = 0.2;
			_g_tetron* g = new _g_tetron;
			g->add_flags(i.tetron, flag_specialty);
			ttr->add_flags(g, flag_part | flag_sub_go);
			add_flags(ttr, flag_part | flag_sub_go);
			ki++;
		}
	}
}*/

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
