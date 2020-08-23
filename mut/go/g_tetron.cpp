#include "go_lite.h"
#include "g_tetron.h"

bool _g_tetron::mouse_move2(_xy r)
{
	if (n_hint) return true; // есть подсказка
	_tetron* a = find1<_tetron>(flag_specialty);
	std::wstring re;
	if (a) re = string_to_wstring(a->name()) + L" " + uint64_to_wstr_hex(a->id);
	if (hint.size())
	{
		if (re.size()) re += L": ";
		re += hint;
	}
	if (re.size()) add_hint(re, this);
	return true;
}

void _g_tetron::mouse_finish_move()
{
	del_hint();
}

bool _g_tetron::cmp_drawn()
{
	if (master_chain_go.chain.size() > drawn_create_star.size()) return false;
	for (uint i = 0; i < master_chain_go.chain.size(); i++)
		if (master_chain_go.chain[i] != drawn_create_star[i])
			return false;
	return true;
}

bool _g_tetron::test_local_area(_xy b)
{
	if (!local_area.test(b)) return false;
	_xy pp = local_area.center();
	double r = std::min(pp.x - local_area.x.min, pp.y - local_area.y.min);
	return ((b - pp).len2() <= r * r);
}

void _g_tetron::ris2(_trans tr, bool final)
{
	_area_old a = tr(local_area);
	_xy p = a.center();
	double r = a.radius();
	double d = log(1.001 + r) * 0.5;
	double y = 0.7 * (r - d) * 2;
	uint c = c_min;

	_tetron* t2 = find1<_tetron>(flag_specialty); // связанный тетрон
	bool im_temp = test_flags(n_temp_go, flag_parent); // текущий _g_tetron - временный
	// выделение красным цветом
	static _tetron* root_tetron = nullptr;
	if (!im_temp)
		root_tetron = t2;
	else
		if (t2 == root_tetron) c = 0xFFFF0000;

	master_bm.fill_ring(p, r, d, c, (im_temp) ? 0 : c_background);

	_t_trans* ttr = nullptr; // звезда
	if (star) ttr = *star;

	if (t2 == nullptr) // нет связанного тетрона
	{
		if (star) delete_hvost(star, true, false);
		return;
	}

	if (final) // нужно заканчивать
	{
		if (star)
			if (cmp_drawn())
				delete_hvost(star, true, false);
		int fr = t2->get_froglif();
		master_bm.froglif(p - _xy{ y / 2, y / 2 }, y, (uchar*)&fr, 2, c_max);
		return;
	}

	if (star) return;

	_g_tetron* gg2;
	// ищет существующий временный _g_tetron
	_g_tetron* aa = t2->find_intermediate<_g_tetron>(n_temp_go, inverted_flags(flag_specialty), flag_parent);
	if (aa)
		if (aa != this)
		{ // может уже есть звезда?
			gg2 = aa; // не this
			ttr = nullptr; // подходящая звезда для использования
			if (gg2->star) ttr = *gg2->star;
			if (ttr)
			{
				drawn_create_star = master_chain_go.chain;
				_t_trans* ttr3 = new _t_trans;
				ttr3->add_flags(n_temp_go, flag_parent);
				ttr3->trans.scale = std::min(local_area.x.length(), local_area.y.length()) * 0.5;
				ttr3->trans.offset = { local_area.x(0.5), local_area.y(0.5) };
				star = ttr3;
				ttr3->add_flags(ttr, flag_part + flag_sub_go, false);
				add_flags(ttr3, flag_part + flag_sub_go, false);
				return;
			}
		}
	drawn_create_star = master_chain_go.chain;
	constexpr double kr = 0.333; // 0.4 растояние между тетронами = kr * radius
	int vl = 0;
	for (auto& i : t2->link)
		if (!i->pairr(t2)->test_flags(n_temp_go, flag_parent)) vl++;
	int vl2 = vl;
	int orbit = 0;
	while (vl2 > 0)
	{
		orbit++;
		vl2 -= (int)(pi * orbit * (2 + kr));
	}
	if (orbit == 0) orbit = 1;
	double radius = 0.98 / (orbit * (2.0 + kr) + 1.0); // 0.95
	ttr = new _t_trans;
	ttr->add_flags(n_temp_go, flag_parent);
	ttr->trans.scale = radius;
	_t_trans* ttr2 = new _t_trans;
	ttr2->add_flags(n_temp_go, flag_parent);
	ttr->add_flags(ttr2, flag_part + flag_sub_go, false);

	if (im_temp)
		star = ttr;
	else
	{
		_t_trans* ttr3 = new _t_trans;
		ttr3->add_flags(n_temp_go, flag_parent);
		ttr3->trans.scale = std::min(local_area.x.length(), local_area.y.length()) * 0.5;
		ttr3->trans.offset = { local_area.x(0.5), local_area.y(0.5) };
		star = ttr3;
		ttr3->add_flags(ttr, flag_part + flag_sub_go, false);
	}
	add_flags(star, flag_part + flag_sub_go, false);

	if (aa)
		gg2 = aa;
	else
	{
		gg2 = new _g_tetron;
		gg2->add_flags(n_temp_go, flag_parent);
		gg2->add_flags(t2, flag_specialty, false);
		gg2->local_area = _area_old(-1.0, 1.0, -1.0, 1.0);
		gg2->star = ttr;
		gg2->add_flags(ttr, flag_part + flag_sub_go, false);
	}

	ttr2->add_flags(gg2, flag_part + flag_sub_go, false);

	int uze_nar = 0;
	uint ilink = 0;
	for (int n_orb = 1; n_orb <= orbit; n_orb++)
	{
		int nn = (int)(pi * n_orb * (2 + kr));
		if (nn > vl - uze_nar) nn = vl - uze_nar;
		int inn = 0;
		for (; (ilink < t2->link.size()) && (inn < nn); ilink++)
		{
			_tetron* t = t2->link[ilink]->pairr(t2);
			if (t->test_flags(n_temp_go, flag_parent)) continue;
			aa = t->find_intermediate<_g_tetron>(n_temp_go, inverted_flags(flag_specialty), flag_parent);
			_g_tetron* gg;
			if (aa)
				gg = aa;
			else
			{
				gg = new _g_tetron;
				gg->add_flags(n_temp_go, flag_parent);
				gg->add_flags(t, flag_specialty, false);
				gg->local_area = _area_old(-1.0, 1.0, -1.0, 1.0);
			}
			ttr2 = new _t_trans;
			ttr2->add_flags(n_temp_go, flag_parent);
			double rad = n_orb * (2.0 + kr);
			ttr2->trans.offset.x = rad * cos(2.0 * pi * inn / nn);
			ttr2->trans.offset.y = -rad * sin(2.0 * pi * inn / nn);
			ttr2->add_flags(gg, flag_part + flag_sub_go, false);
			ttr->add_flags(ttr2, flag_part + flag_sub_go, false);

			_g_link* gl = new _g_link;
			gl->add_flags(n_temp_go, flag_parent);
			gl->add_flags(gg, flag_specialty, false);
			gl->add_flags(gg2, flag_specialty2, false);
			_t_trans* ttr3 = new _t_trans;
			ttr3->set_layer(-1.0);
			ttr3->add_flags(n_temp_go, flag_parent);
			ttr3->add_flags(gl, flag_part + flag_sub_go, false);
			ttr->add_flags(ttr3, flag_part + flag_sub_go, false);
			gl->k = kr / (n_orb * (2 + kr) - 2); // 0==0 k=1, 0= 0 k=0.5
			gl->calc_local_area();
			gl->area = _tarea::indefinite;
			inn++;
		}
		uze_nar += nn;
	}
	return;
}

void _g_tetron::add_unique_flags(_tetron* t, uint64 flags, bool after)
{
	if (star) delete_hvost(star, true, false);
	_tetron::add_unique_flags(t, flags, after);
	//	cha_area();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void _g_tetron2::ris2(_trans tr, bool final)
{
	_area_old a = tr(local_area);
	_xy p = a.center();
	double r = a.radius();
	double d = log(1.001 + r) * 0.5;
	double y = 0.7 * (r - d) * 2;
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
		/*		for (auto i : *SuperDelTetron2::ud)
				{
					molecule m;
					m.c = C_MIN;
					for (int j = 0; j < i->vlink_; j++)
					{
						Tetron* a = i->link_[j].tetron_;
						if (!a) continue;
						m.tetron = a;
						m.p.x = (prng.Random(2001) * 0.001 - 1.0);
						m.p.y = (prng.Random(2001) * 0.001 - 1.0);
						mo.insert(m);
					}
				}*/
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
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void _g_link::ris2(_trans tr, bool final)
{
	_xy pp11 = tr(p11);
	_xy pp12 = tr(p12);
	_xy pp21 = tr(p21);
	_xy pp22 = tr(p22);

	_g_tetron* g1 = find1<_g_tetron>(flag_specialty);
	_g_tetron* g2 = find1<_g_tetron>(flag_specialty2);
	_tetron* t1 = g1->find1<_tetron>(flag_specialty);
	_tetron* t2 = g2->find1<_tetron>(flag_specialty);
	uint64 f = t1->get_flags(t2);

	constexpr uint cc[32] = { 0xFF0080FD, 0xFFEF0000, 0xFF9E3BFF, 0xFF938700, 0xFF12AA00, 0xFFD34E0D, 0xFF7470DC,
		0xFF4D9682, 0xFFC80FCE, 0xFFB06381,	0xFFE22653, 0xFF009D93, 0xFFC4439D, 0xFF258ECB, 0xFF659B00, 0xFF3F77FF,
		0xFF887E87, 0xFF9659DD, 0xFFB57000, 0xFF39A337, 0xFF5D84BE, 0xFFAF1FFB, 0xFFDE028A, 0xFF768F51, 0xFFB400FC,
		0xFFE23700, 0xFFCB29B2, 0xFFA3755A, 0xFFC45D37, 0xFFEB171B, 0xFF966DA8, 0xFFAC4ACF };

	_xy v = p21 - p11;
	v /= v.len();
	double b = (pp12 - pp11).len() * 0.5;
	double r = b / sin(dalpha);
	int al = act_li;
	if (tr != master_trans_go_move) al = -1;
	for (int i = 0; i < v_link; i++)
	{
		double bb = b * (v_link - 0.5 - i) / v_link;
		double dl = sqrt(r * r - bb * bb) - r * cos(dalpha);
		double dl2 = (k > 0.9) ? dl : 0;
		uint c = (f & (1ULL << i)) ? cc[i] : cclow(cc[i]);
		double t = 1;
		if (al == i)
		{
			t = 2;
			c = brighten(c);
		}
		if ((al != -1) || (f & (1ULL << i)))
			master_bm.lines(pp11 + (pp12 - pp11) * ((i + 0.5) / (v_link * 2)) + v * dl,
				pp21 + (pp22 - pp21) * ((i + 0.5) / (v_link * 2)) - v * dl2, t, c);
		c = (f & (1ULL << (i + 32))) ? cc[i] : cclow(cc[i]);
		t = 1;
		if (al == (v_link * 2 - 1 - i))
		{
			t = 2;
			c = brighten(c);
		}
		if ((al != -1) || (f & (1ULL << (i + 32))))
			master_bm.lines(pp11 + (pp12 - pp11) * ((v_link * 2 - 0.5 - i) / (v_link * 2)) + v * dl,
				pp21 + (pp22 - pp21) * ((v_link * 2 - 0.5 - i) / (v_link * 2)) - v * dl2, t, c);
	}
}

bool _g_link::test_local_area(_xy b)
{
	if (!local_area.test(b)) return false;
	if (((test_line(p11, p12, b) + test_line(p21, p22, b) + test_line(p11, p21, b) + test_line(p12, p22, b)) & 1) == 0) return false;
	_xy v = p21 - p11;
	v /= v.len();
	_xy c1 = (p11 + p12) * 0.5;
	double d = (p12 - p11).len() * 0.5;
	double ot = d / tan(dalpha);
	double r = d / sin(dalpha);
	_xy p1 = c1 - v * ot;
	if ((b - p1).len2() < r * r) return false;
	if (k > 0.9)
	{
		c1 = (p21 + p22) * 0.5;
		p1 = c1 + v * ot;
		if ((b - p1).len2() < r * r) return false;
	}
	return true;
}

void _g_link::calc_local_area()
{
	local_area = _tarea::empty;
	_g_tetron* g1 = find1<_g_tetron>(flag_specialty);
	_g_tetron* g2 = find1<_g_tetron>(flag_specialty2);
	if ((g1 == nullptr) || (g2 == nullptr)) return;
	_t_trans* par = find1<_t_trans>(flag_sub_go << 32);
	if (par == nullptr) return;
	_t_basic_go* parpar = par->find1<_t_basic_go>(flag_sub_go << 32);
	if (parpar == nullptr) return;
	_t_trans* tr1 = parpar->find_intermediate<_t_trans>(g1, flag_sub_go, flag_sub_go);
	_t_trans* tr2 = parpar->find_intermediate<_t_trans>(g2, flag_sub_go, flag_sub_go);
	if ((tr1 == nullptr) || (tr2 == nullptr)) return;
	_area_old a1 = tr1->trans(g1->local_area);
	_area_old a2 = tr2->trans(g2->local_area);
	par->trans = _trans();
	_xy p1 = a1.center();
	double r1 = a1.radius();
	_xy p2 = a2.center();
	double r2 = a2.radius();
	_xy d = p2 - p1;
	if ((d.y == 0) && (d.x == 0)) return;
	double alpha = atan2(d.y, d.x);
	double dv0 = d.len();
	double dv = (dv0 - (r1 + r2) * cos(dalpha)) * k;
	if (k < 0.9)
		dv = (dv0 - (r1 + r2)) * k + r1 * (1 - cos(dalpha));
	double kk = dv / dv0;
	local_area += p11 = _xy{ p1.x + r1 * cos(alpha - dalpha), p1.y + r1 * sin(alpha - dalpha) };
	local_area += p12 = _xy{ p1.x + r1 * cos(alpha + dalpha), p1.y + r1 * sin(alpha + dalpha) };
	p21 = { p11.x + d.x * kk, p11.y + d.y * kk };
	p22 = { p12.x + d.x * kk, p12.y + d.y * kk };
	local_area += p21;
	local_area += p22;
}

bool _g_link::mouse_down_left2(_xy r)
{
	if (act_li < 0) return true;

	_g_tetron* g1 = find1<_g_tetron>(flag_specialty);
	_g_tetron* g2 = find1<_g_tetron>(flag_specialty2);
	_tetron* t1 = g1->find1<_tetron>(flag_specialty);
	_tetron* t2 = g2->find1<_tetron>(flag_specialty);
	uint64 f;
	if (act_li < v_link)
		f = (1ULL << act_li);
	else
		f = (1ULL << (32 + 2 * v_link - 1 - act_li));
	t1->xor_flags(t2, f);
	cha_area(master_trans_go);
	return true;
}

bool _g_link::mouse_move2(_xy r)
{
	static constexpr std::wstring_view nh[v_link] = { L"parent", L"part", L"run", L"???", L"sub_go", L"information",
		L"information2", L"specialty", L"specialty2" };
	_xy v = p12 - p11;
	double b = v.len();
	double a = (r - p11).scalar(v) / b;
	double k = a / b; // 0..1
	int ii = act_li;
	act_li = (int)(k * v_link * 2);
	if (act_li < 0) act_li = 0;
	if (act_li >= v_link * 2) act_li = act_li = v_link * 2 - 1;

	if (ii != act_li)
	{
		change_hint((act_li < v_link) ? nh[act_li] : nh[2 * v_link - 1 - act_li]);
		cha_area(master_trans_go);
	}
	if (!n_hint) add_hint((act_li < v_link) ? nh[act_li] : nh[2 * v_link - 1 - act_li], this);
	return true;
}

void _g_link::mouse_finish_move()
{
	act_li = -1;
	cha_area(master_trans_go_move);
	del_hint();
}

