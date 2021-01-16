#include "cfgt.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void _view_tetron::ris2(_trans tr, bool final)
{
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
