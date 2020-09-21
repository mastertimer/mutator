/*//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

!восстановление не в правильном месте (не сработает продажа)
пиксель фроглифа в колесе исчезает
быстро создавать цвета
правильная альтернатива _g_tetron
упорядочить функции _picture
сделать видимую сетку при увеличении
!!пляшет график цен при увеличении, тоже сделать видимой сетку

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////*/

#include <ctime>

#include "set.h"
#include "mutator.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

_stack clipboard; // буффер обмена

void fun03(_tetron* tt0, _tetron* tt, u64 flags);

void init_shuba2()
{
	if (!n_act) return;
	_g_rect* rr = *n_ramk2;
	uint c1 = 0, c2 = 0;
	if (rr != nullptr)
	{
		c1 = rr->get_c();
		c2 = rr->get_c2();
	}
	delete_hvost(n_ramk2);
	if (rr != nullptr)
	{
		rr->set_c(c1);
		rr->set_c2(c2);
	}
	if (!n_act) return;

	_g_tetron* a = new _g_tetron;
	a->add_flags(n_act, flag_specialty);

	_t_trans* tr = n_ramk2->operator _t_basic_go * ()->set_t_trans(a, flag_part + flag_sub_go);
	tr->trans.offset = { -27, 0 };

	_t_function* fu = new _t_function(17);
	fu->add_flags(n_fun_tani, flag_parent);
	a->add_flags(fu, flag_part + flag_information);
	fu = new _t_function(26);
	fu->add_flags(n_fun_tani1, flag_parent);
	a->add_flags(fu, flag_part + flag_information);
	_tetron* b = new _tetron;
	b->add_flags(n_fun_tani0, flag_parent);
	a->add_flags(b, flag_part + flag_information);
}

void smena_avt()
{
	n_ko->operator _t_basic_go* ()->set_t_trans(n_ramk2, 0);
	if (n_act)
	{
		_t_go* g = *n_act;
		if (g != nullptr)
			if ((n_ko != g) && (n_ramk != g) && (n_ramk2 != g) &&
				(n_ramk2 != g->find1<_t_go>(flag_part << 32)))
			{
				_t_go* ra = *n_ramk2;

				_area a2 = g->local_area;
				if (a2.empty()) a2 = g->calc_area();
				a2 = n_ko->operator _t_trans * ()->trans.inverse(g->oko_trans()(a2).expansion(3.0));

				_t_trans* rat = n_ko->operator _t_basic_go * ()->set_t_trans(n_ramk2, flag_sub_go);
				rat->trans.offset = { a2.x.min, a2.y.min };
				rat->trans.scale = 1.0 / n_ko->operator _t_trans * ()->trans.scale;
				ra->local_area = { {0, a2.x.length() / rat->trans.scale}, {0, a2.y.length() / rat->trans.scale} };

				ra->area_definite = false;
				init_shuba2();
				ra->add_area();
			}
	}
	else
	{ //!!! продублировано из шубы
		_g_rect* rr = *n_ramk2;
		uint c1 = 0, c2 = 0;
		if (rr != nullptr)
		{
			c1 = rr->get_c();
			c2 = rr->get_c2();
		}
		delete_hvost(rr);
		if (rr != nullptr)
		{
			rr->set_c(c1);
			rr->set_c2(c2);
		}

	}
	n_pot_act = 0ULL;
	if (n_act)
		fun03(0, 0, 0);
	else
		set_cursorx((*n_perenos->operator i64 * ()) ? (_cursor::size_all) : ((*n_s_right->operator i64 * ()) ?
			_cursor::hand_point : _cursor::normal));
}

void master_mouse_move(_xy r)
{
	_t_trans* a = *n_ko;
	if (n_act)
	{
		_t_go* ra2 = n_ramk2->operator _t_go * ();
		_t_trans* rat2 = ra2->ttrans();
		if (rat2)
			if (ra2->test_local_area(rat2->trans.inverse(a->trans.inverse(r))))
			{
				if (n_go_move != n_ramk2)
				{
					if (n_go_move) n_go_move->operator _t_go* ()->mouse_finish_move();
					n_go_move = n_ramk2;
					set_cursorx(_cursor::drag);
				}
				return;
			}
	}
	master_chain_go.clear();
	if (!a->mouse_move(a->trans, false))
		if (n_go_move)
		{
			_t_go* xx = *n_go_move;
			if (xx)
			{
				if (n_ramk2 == xx) set_cursorx(_cursor::normal); else xx->mouse_finish_move();
			}
			n_go_move = 0ULL;
		}
}

void init_shuba()
{
	if (!n_pot_act) return;
	_g_rect* rr = *n_ramk;
	uint c1 = 0, c2 = 0;
	if (rr != nullptr)
	{
		c1 = rr->get_c();
		c2 = rr->get_c2();
	}
	delete_hvost(n_ramk);
	if (rr != nullptr)
	{
		rr->set_c(c1);
		rr->set_c2(c2);
	}
	_g_tetron* a = new _g_tetron;
	a->add_flags(n_pot_act, flag_specialty);
	a->local_area.x.min -= 27; //чтоб на заморачиваться с транформацией
	a->local_area.x.max -= 27;
	n_ramk->add_flags(a, flag_part + flag_sub_go);
}

void find_pot_act()
{
	n_pot_act = 0ULL;
	_xy r = mouse_xy;
	_t_trans* bb = *n_ko;
	r = bb->trans.inverse(r);
	bb->find_pot_act(r);
}

void find_ris_pot_act()
{
	_tetron* pred = n_pot_act;
	find_pot_act();
	if (n_pot_act == pred) return;
	if (n_pot_act)
	{
		_t_go* ra = *n_ramk;
		if (pred) ra->del_area();
		_t_go* g = *n_pot_act;
		_area a2 = g->local_area;
		if (a2.empty()) a2 = g->calc_area();
		a2 = n_ko->operator _t_trans * ()->trans.inverse(g->oko_trans()(a2).expansion(3.0));
		_t_trans* rat = n_ko->operator _t_basic_go * ()->set_t_trans(ra, flag_sub_go);
		rat->trans.offset = { a2.x.min, a2.y.min };
		rat->trans.scale = 1.0 / n_ko->operator _t_trans * ()->trans.scale;
		ra->local_area = { {0, a2.x.length() / rat->trans.scale}, {0, a2.y.length() / rat->trans.scale} };
		ra->area_definite = false;
		init_shuba();
		ra->add_area();
		/*		if (pred)
					ra->add_area();
				else
					link[n_ko].tetron->set_t_trans(ra, flag_sub_go);*/
	}
	else
		n_ko->operator _t_basic_go* ()->set_t_trans(n_ramk, 0);
}

void fun01(_tetron* tt0, _tetron* tt, u64 flags) // нажата правая кнопка мышки
{
	if (*n_perenos->operator i64 * ()) return;
	if (*n_s_left->operator i64 * ())
	{ // правая при зажатой левой
//		oko->MouseDownLeftRight(oko->mouse_xy_);
		return;
	}
	set_cursorx(_cursor::hand_point);
	n_ko->operator _t_basic_go* ()->set_t_trans(n_ramk, 0);
	n_pot_act = 0ULL;
	n_move->run(0, n_move, flags);
}

void fun02(_tetron* tt0, _tetron* tt, u64 flags)
{
	_t_go* a = tt->find1<_t_go>(flag_part);
	if (!a) return;
	_t_function* fu = tt->find1<_t_function>(flag_part);
	if (!fu) return;
	_t_trans* b = new _t_trans;
	b->add_flags(a, flag_parent);
	b->trans = n_ko->operator _t_trans * ()->trans.inverse();
	b->trans.offset.x += mouse_xy.x * b->trans.scale;
	b->trans.offset.y += mouse_xy.y * b->trans.scale;
	n_ko->add_flags(b, flag_part + flag_sub_go);
	fu->add_flags(b, flag_part);
	fun03(0, 0, flags);
}

void fun03(_tetron* tt0, _tetron* tt, u64 flags)
{
	_xy to = mouse_xy;
	if (n_tani)
	{
		if (!*n_s_left->operator i64 * ())
		{
			n_up_left->run(0, n_up_left, flag_run); // или fun06()??
			return;
		}
		n_tani->operator _t_go* ()->mouse_move_left2(master_trans_go.inverse(to));
		mouse_xy_pr = mouse_xy;
		return;
	}
	if (*n_perenos->operator i64 * ())
	{
		if (*n_s_left->operator i64 * ())
		{
			_t_trans* kor = *n_ko;
			kor->cha_area(kor->calc_area());
			kor->trans.offset.x += mouse_xy.x - mouse_xy_pr.x;
			kor->trans.offset.y += mouse_xy.y - mouse_xy_pr.y;
			kor->cha_area(kor->calc_area());
			mouse_xy_pr = mouse_xy;
			n_move_all->run(0, n_move_all, flag_run);
		}
		return; // закомментировать - колесо будет работать, но курсор будет сбрасываться при выделении
	}
	if (*n_s_right->operator i64 * ())
	{ // зажато правая - выделение
		find_ris_pot_act();
		return;
	}
	if (n_go_move == n_ramk2)
	{
		if (*n_s_left->operator i64 * ())
		{
			_t_go* ra = *n_ramk2;
			_t_trans* rat = ra->ttrans();
			ra->del_area();
			if (g_cursor == _cursor::drag)
			{
				_t_basic_go* raa = *n_act;
				_t_trans* rat2 = (raa) ? raa->ttrans() : nullptr;
				if (rat2) // пока запрещено, потом возможно надо менять local_area??
				{
					_trans t2 = ra->oko_trans() / rat->trans;
					rat->trans.offset.x += (mouse_xy.x - mouse_xy_pr.x) / t2.scale;
					rat->trans.offset.y += (mouse_xy.y - mouse_xy_pr.y) / t2.scale;
					ra->add_area();
					raa->del_area();
					t2 = raa->oko_trans() / rat2->trans;
					rat2->trans.offset.x += (mouse_xy.x - mouse_xy_pr.x) / t2.scale;
					rat2->trans.offset.y += (mouse_xy.y - mouse_xy_pr.y) / t2.scale;
					raa->add_area();
				}
			}
			if (g_cursor == _cursor::size_we)
			{
				_trans t2 = ra->oko_trans();
				ra->local_area.x.max += (mouse_xy.x - mouse_xy_pr.x) / t2.scale;
				if (ra->local_area.x.max < ra->local_area.x.min)
					ra->local_area.x.max = ra->local_area.x.min;
				ra->area_definite = false;
				ra->add_area();
				ra = *n_act;
				if (ra)
				{
					ra->del_area();
					t2 = ra->oko_trans();
					double q = ra->local_area.x.max;
					ra->local_area.x.max += (mouse_xy.x - mouse_xy_pr.x) / t2.scale;
					if (ra->local_area.x.max <= ra->local_area.x.min) ra->local_area.x.max = q; // или выделять нулевую ширину
					ra->area_definite = false;
					ra->add_area();
					ra->resize();
				}
			}
			if (g_cursor == _cursor::size_ns)
			{
				_trans t2 = ra->oko_trans();
				ra->local_area.y.max += (mouse_xy.y - mouse_xy_pr.y) / t2.scale;
				if (ra->local_area.y.max < ra->local_area.y.min)
					ra->local_area.y.max = ra->local_area.y.min;
				ra->area_definite = false;
				ra->add_area();
				ra = *n_act;
				if (ra)
				{
					ra->del_area();
					t2 = ra->oko_trans();
					double q = ra->local_area.y.max;
					ra->local_area.y.max += (mouse_xy.y - mouse_xy_pr.y) / t2.scale;
					if (ra->local_area.y.max <= ra->local_area.y.min) ra->local_area.y.max = q; // или выделять нулевую ширину
					ra->area_definite = false;
					ra->add_area();
					ra->resize();
				}
			}
			mouse_xy_pr = mouse_xy;
			return;
		}
		_t_go* r2 = *n_ramk2;
		_area a = r2->oko_trans()(r2->local_area);
		_area pr = { {a.x.max - 5, a.x.max}, {a.y(0.1), a.y(0.9)} };
		_area ni = { {a.x(0.1), a.x(0.9)}, {a.y.max - 5, a.y.max} };
		if (pr.test(to))
			set_cursorx(_cursor::size_we);
		else
			if (ni.test(to))
				set_cursorx(_cursor::size_ns);
			else
				set_cursorx(_cursor::drag);
	}
	master_mouse_move(to);
}

void fun04(_tetron* tt0, _tetron* tt, u64 flags)
{
	if (*n_perenos->operator i64 * ())
	{
		_xy tr = mouse_xy;
		_t_trans* kor = *n_ko;
		double kk = *n_wheel->operator i64 * ();
		if ((kk > 0) && (kor->trans.scale > 1E12)) return;
		kor->cha_area(kor->calc_area());
		kor->trans.MasToch(tr, pow(1.1, kk));
		kor->cha_area(kor->calc_area());
		n_move_all->run(0, n_move_all, flag_run);
		return;
	}
	if (*n_s_right->operator i64 * ())
	{
		if (!(*n_perenos->operator i64 * ())) {};
		return;
	}
	if (n_go_move == n_ramk2)
	{
		_t_basic_go* raa = *n_act;
		_t_trans* rat2 = (raa) ? raa->ttrans() : nullptr;
		if (rat2)
		{
			_xy tr = mouse_xy;
			_t_go* ra = *n_ramk2;
			_t_trans* rat = ra->ttrans();
			ra->del_area();
			rat->trans.MasToch((ra->oko_trans() / rat->trans).inverse(tr), pow(1.1, *n_wheel->operator i64 * ()));
			ra->add_area();
			raa->del_area();
			rat2->trans.MasToch((raa->oko_trans() / rat2->trans).inverse(tr), pow(1.1, *n_wheel->operator i64 * ()));
			raa->add_area();
		}
		return;
	}
	_t_trans* aa = *n_ko;
	aa->mouse_wheel(aa->trans);
}

void fun05(_tetron* tt0, _tetron* tt, u64 flags)
{
	mouse_xy_pr = mouse_xy;
	if ((*n_perenos->operator i64 * ()) || (n_go_move == n_ramk2))
		return;
	_t_trans* a = *n_ko;
	a->mouse_down_left(a->trans);
}

void fun06(_tetron* tt0, _tetron* tt, u64 flags)
{
	if (!n_tani) return;
	n_tani->operator _t_go* ()->mouse_up_left2(master_trans_go.inverse(mouse_xy));
	n_tani = 0ULL;
}

void fun07(_tetron* tt0, _tetron* tt, u64 flags) // отжата правая кнопка мышки
{
	if (*n_perenos->operator i64 * ()) return;
	if (n_go_move == n_ramk2) n_go_move = 0ULL;
	if (n_act == n_pot_act)
		n_act = 0ULL;
	else
		n_act = n_pot_act;
	n_pot_act = 0ULL;
	n_ko->operator _t_basic_go* ()->set_t_trans(n_ramk, 0);
	smena_avt();
	tt->_tetron::run(0, n_act, flags);
}

void fun08(_tetron* tt0, _tetron* tt, u64 flags)
{
	if (!n_act_key) return;
	_t_go* a = *n_act_key;
	if (!a) return;
	a->key_down((ushort)(*n_down_key->operator i64 * ()));
}

void fun09(_tetron* tt0, _tetron* tt, u64 flags)
{
	if (!n_act_key) return;
	_t_go* a = *n_act_key;
	if (!a) return;
	a->key_press((ushort)(*n_press_key->operator i64 * ()));
}

void fun10(_tetron* tt0, _tetron* tt, u64 flags)
{
	_t_int* a = tt->find1<_t_int>(flag_information2);
	if (a)
		for (_frozen i(tt, flag_information); i; i++) tt0->del_flags(i, a->a);
	tt->_tetron::run(0, tt, flags); // продолжить выполнение
}

void fun11(_tetron* tt0, _tetron* tt, u64 flags)
{
	_t_int* a = tt->find1<_t_int>(flag_information2);
	if (a)
		for (_frozen i(tt, flag_information); i; i++) tt0->add_flags(i, a->a);
	tt->_tetron::run(0, tt, flags); // продолжить выполнение
}

void fun12(_tetron* tt0, _tetron* tt, u64 flags)
{
	if (n_go_move)
	{
		_t_go* go = *n_go_move;
		if (go) go->mouse_up_middle();
	}
	_t_basic_go* a = tt->find1<_t_basic_go>(flag_part);
	if (!a) return;
	delete a;
}

void fun13(_tetron* tt0, _tetron* tt, u64 flags);

void fun14(_tetron* tt0, _tetron* tt, u64 flags)
{
	i64* nn = tt->find1<i64>(flag_part);
	if (!nn) return;
	_t_go* a = (create_tetron((uchar)(*nn)))->operator _t_go * ();
	if (!a) return;
	_t_trans* ttr = new _t_trans;
	//	a->trans_.sm_ = {*g_oko->F_x()->Uint()-a->local_area_.x_.L()*0.5, *g_oko->F_y()->Uint()-a->local_area_.y_.L()*0.5};
	ttr->trans.offset = { mouse_xy.x - 5, mouse_xy.y - 5 };
	ttr->trans = n_ko->operator _t_trans * ()->trans.inverse() * ttr->trans;
	ttr->add_flags(a, flag_sub_go + flag_part);
	n_ko->add_flags(ttr, flag_sub_go + flag_part);
	n_act = a;
	smena_avt();
	tt->_tetron::run(0, n_act, flags);
}

void fun15(_tetron* tt0, _tetron* tt, u64 flags);

void fun16(_tetron* tt0, _tetron* tt, u64 flags);

void fun17(_tetron* tt0, _tetron* tt, u64 flags)
{
	if (!tt0) return;
	_t_go* a = *tt0;
	if (!a) return;
	_t_trans* ttr = a->ttrans();
	a->del_area();
	master_trans_go /= ttr->trans;
	ttr->trans.offset.x += par_koo1.x - 10;
	ttr->trans.offset.y += par_koo1.y - 10;
	master_trans_go *= ttr->trans;
	a->add_area();
	find_ris_pot_act();
}

void fun18(_tetron* tt0, _tetron* tt, u64 flags)
{
	if (!tt0) return;
	_g_tetron* g = *tt0; // можно только оттуда вызывать 
	if (!g) return;
	_t_trans* aa = g->find1<_t_trans>((flag_sub_go + flag_part) << 32);
	if (!aa) return;
	_t_go* a = aa->find1<_t_go>((flag_sub_go + flag_part) << 32);
	if (!a) return;
	_t_basic_go* det = a->find1<_t_basic_go>(flag_parent << 32);
	_tetron* t2 = g->find1<_tetron>(flag_specialty);
	if (t2)
	{
		if (!det)
		{
			_t_trans* b = new _t_trans;
			_t_function* ff = new _t_function(24);
			b->add_flags(ff, flag_part + flag_run);
			b->add_flags(a, flag_parent);
			n_ko->add_flags(b, flag_part + flag_sub_go);
			b->run(0, b, flag_run);
			n_move_all->add_flags(b, flag_run);
		}
	}
	else
		if (det)
		{ // !! опасно если объект зациклен
			det->clear_go_rod();
			delete_hvost(det, true);
		}
	a->cha_area();
}

void fun19(_tetron* tt0, _tetron* tt, u64 flags);

void fun20(_tetron* tt0, _tetron* tt, u64 flags);

void fun21(_tetron* tt0, _tetron* tt, u64 flags);

void fun22(_tetron* tt0, _tetron* tt, u64 flags);

void fun23(_tetron* tt0, _tetron* tt, u64 flags)
{
	/*	_tetron* t = tt->find1<_tetron>(flag_part);
		_g_picture* tp = *t;
		if (tp)
		{
			auto fn = dialog::open_file(L"*.bmp");
			if (!exists(fn)) return;
			tp->load_from_file(fn);
		}*/
}

void fun24(_tetron* tt0, _tetron* tt, u64 flags)
{
	if (!tt0) return;
	_t_trans* g = *tt0;
	if (!g) return;
	g->del_area();
	g->trans = _trans();
	g->trans = g->oko_trans().inverse();
	g->trans.offset.x += 16 * g->trans.scale;
	g->trans.offset.y += 16 * g->trans.scale;
	g->add_area();
}

void fun25(_tetron* tt0, _tetron* tt, u64 flags)
{
	_g_picture* p = tt->find1<_g_picture>(flag_information);
	i64* r = tt->find1<i64>(flag_information);
	if ((!p) || (!r)) return;
	p->new_size((int)*r, (int)*r);
}

void fun26(_tetron* tt0, _tetron* tt, u64 flags)
{
	if (!tt0) return;
	_t_go* a = *tt0;
	if (!a) return;
	_t_trans* ttr = a->ttrans();
	a->del_area();
	ttr->trans.offset.x = -21;
	ttr->trans.offset.y = 0;
	a->add_area();
	if (n_pot_act)
	{
		_t_go* b = *n_pot_act;
		n_pot_act = 0ULL;
		n_ko->operator _t_basic_go* ()->set_t_trans(n_ramk, 0);
		b->priem_gv();
	}
	else
	{
		_t_trans* b = *n_ko;
		b->priem_gv();
	}
}

void fun27(_tetron* tt0, _tetron* tt, u64 flags)
{
	_xy tr = mouse_xy;
	_t_trans* kor = *n_ko;
	kor->cha_area(kor->calc_area());
	kor->trans.MasToch(tr, 1 / kor->trans.scale);
	kor->trans.scale = 1; // точно единица
	kor->cha_area(kor->calc_area());
	n_move_all->run(0, n_move_all, flag_run);
}

void fun28(_tetron* tt0, _tetron* tt, u64 flags)
{
	*n_perenos->operator i64* () = !(*n_perenos->operator i64 * ());
	set_cursorx((*n_perenos->operator i64 * ()) ? (_cursor::size_all) : ((*n_s_right->operator i64 * ()) ?
		_cursor::hand_point : _cursor::normal));

	if ((!*n_perenos->operator i64 * ()) && (n_act))
	{
		if (n_go_move == n_ramk2) n_go_move = 0ULL;
		fun03(0, 0, flags);
	}
}

void fun29(_tetron* tt0, _tetron* tt, u64 flags)
{
	time_ris = !time_ris;
	master_obl_izm = { {0.0, (double)master_bm.size.x}, {0.0, (double)master_bm.size.y} };
}

void fun30(_tetron* tt0, _tetron* tt, u64 flags);

void fun31(_tetron* tt0, _tetron* tt, u64 flags)
{
}

void fun32(_tetron* tt0, _tetron* tt, u64 flags)
{
}

void fun33(_tetron* tt0, _tetron* tt, u64 flags)
{
	_g_graph* g = new _g_graph;
	n_ko->operator _t_basic_go *()->set_t_trans(g, flag_sub_go + flag_part);
	std::vector<i64> k;
	test_ss(19, k);
	g->add(_matrix(266, [&k](i64 n) { return (double)k[n]; }));
	test_ss(11, k);
	g->add(_matrix(266, [&k](i64 n) { return (double)k[n]; }));

/*	for (i64 f = 0; f < 20; f++)
	{
		std::vector<i64> k;
		test_ss(f, k);
		i64 s = 0;
		for (i64 i = 0; i < (i64)k.size(); i++) s += k[i];
		s = s * 31 / 32;
		i64 ii = 0;
		for (i64 i = 0; i < (i64)k.size(); i++)
		{
			s -= k[i];
			if (s <= 0)
			{
				ii = i;
				break;
			}
		}

		MessageBox(0, (std::to_wstring(f) + L" = " + std::to_wstring(ii)).c_str(), L"упс", MB_OK | MB_TASKMODAL);
	}*/

/*	_nervous_oracle2 rr;
	i64 nn = 0;
	double kk = 1;
	calc_all_prediction(rr, nn, kk);
	MessageBox(0, std::to_wstring(nn).c_str(), L"упс", MB_OK | MB_TASKMODAL);
	MessageBox(0, std::to_wstring(kk).c_str(), L"упс", MB_OK | MB_TASKMODAL);
	if (nn == 0) return;
	kk = pow(kk, 1.0 / nn);
	MessageBox(0, std::to_wstring(kk).c_str(), L"упс", MB_OK | MB_TASKMODAL);*/

	//	_g_tetron* g = new _g_tetron;
	//	g->add_flags(n_checkbox, flag_specialty);
	//	_t_trans* tr = n_ko->operator _t_basic_go *()->set_t_trans(g, flag_sub_go + flag_part);
}

void fun34(_tetron* tt0, _tetron* tt, u64 flags)
{
}

void fun35(_tetron* tt0, _tetron* tt, u64 flags);

void fun36(_tetron* tt0, _tetron* tt, u64 flags);

void fun37(_tetron* tt0, _tetron* tt, u64 flags)
{
	i64* nn = tt->find1<i64>(flag_information);
	if (!nn) return;
	_tetron* a = create_tetron((uchar)(*nn));
	if (!a) return;
	tt->_tetron::run(0, a, flags);
}

void fun38(_tetron* tt0, _tetron* tt, u64 flags)
{
	i64* a = *tt0;
	if (a)
	{
		i64* nn = tt->find1<i64>(flag_information);
		if (nn) *a = *nn;
	}
	tt->_tetron::run(0, tt0, flags);
}

void fun39(_tetron* tt0, _tetron* tt, u64 flags)
{
	_g_tetron* g = tt->find1<_g_tetron>(flag_information);
	double* m = tt->find1<double>(flag_information);
	if ((m == nullptr) || (g == nullptr)) return;
	if (*m <= 0) return;
	_tetron* t2 = g->find1<_tetron>(flag_specialty);
	if (t2 == nullptr) return;
	_t_basic_go* gg = *t2;
	if (gg == nullptr) return;
	gg->del_area();
	gg->ttrans()->trans.scale = *m;
	gg->add_area();
}

void fun40(_tetron* tt0, _tetron* tt, u64 flags)
{
	for (auto i : tt->link)
	{
		_tetron* a = (*i)(tt);
		if (!i->test_flags(tt, flag_part)) continue;
		tt->_tetron::run(0, a, flags);
	}
}

void fun41(_tetron* tt0, _tetron* tt, u64 flags)
{
	if (tt0) tt0->run(0, tt0, flags);
}

void fun42(_tetron* tt0, _tetron* tt, u64 flags)
{
}

void fun43(_tetron* tt0, _tetron* tt, u64 flags)
{
}

void fun44(_tetron* tt0, _tetron* tt, u64 flags)
{
}

void fun45(_tetron* tt0, _tetron* tt, u64 flags)
{
}

void fun46(_tetron* tt0, _tetron* tt, u64 flags)
{
}

void fun47(_tetron* tt0, _tetron* tt, u64 flags)
{
	_g_picture* g = tt->find1<_g_picture>(flag_information);
	i64* c = tt->find1<i64>(flag_information);
	if ((c == nullptr) || (g == nullptr)) return;
	g->pic.clear((uint)(*c));
	g->cha_area();
}

void fun48(_tetron* tt0, _tetron* tt, u64 flags)
{
	if (tt0 == nullptr) return;
	_g_picture* g = *tt0;
	if (g == nullptr) return;
	_xy r1 = master_trans_go.inverse(mouse_xy_pr);
	_xy r2 = master_trans_go.inverse(mouse_xy);
	g->pic.line(r1, r2, (*n_s_shift->operator i64 * ()) ? 0 : c_def, true);
	g->cha_area();
}

void fun49(_tetron* tt0, _tetron* tt, u64 flags)
{
}

void fun50(_tetron* tt0, _tetron* tt, u64 flags)
{
	clipboard.clear();
	_g_tetron* g = tt->find1<_g_tetron>(flag_information);
	if (g == nullptr) return;
	_tetron* t2 = g->find1<_tetron>(flag_specialty);
	if (t2 == nullptr) return;
	_picture* p = *t2;
	if (p == nullptr) return;
	std::wstring s = L"Picture";
	clipboard << s << *p;
}

void fun51(_tetron* tt0, _tetron* tt, u64 flags)
{
}

void fun52(_tetron* tt0, _tetron* tt, u64 flags)
{
	_g_tetron* g = tt->find1<_g_tetron>(flag_information);
	if (g == nullptr) return;
	_tetron* t2 = g->find1<_tetron>(flag_specialty);
	if (t2 == nullptr) return;
	std::wstring s;
	clipboard.adata = 0;
	clipboard >> s;
	if (s == L"Picture")
	{
		_picture* p = *t2;
		clipboard >> *p;
		_t_go* gg = *t2;
		if (gg) gg->cha_area();
		return;
	}
}

void fun53(_tetron* tt0, _tetron* tt, u64 flags)
{ // !! флаги читать из параметра
	for (_frozen i(tt, flag_information); i; i++) i->add_unique_flags(tt0, flag_specialty);
	tt->_tetron::run(0, tt, flags); // продолжить выполнение
}

void fun54(_tetron* tt0, _tetron* tt, u64 flags)
{
	i64* n = tt->find1<i64>(flag_part);
	if (!n) return;
	_speed<_hash_table_tetron> ht;
	for (uint i = 0; i < master_chosen.size(); i++)
		if (*master_chosen[i])
			(*master_chosen[i])->traversal(ht, 0);
	int v = 0;
	_tetron* b = nullptr; // пример функции
	for (auto& i : *ht)
	{
		_tetron* a = i.tetron;
		if (a->type() != 32) continue;
		if (*a->operator i64 * () != *n) continue;
		v++;
		b = a;
	}
	MessageBox(0, (L"найдено функций: " + std::to_wstring(v)).c_str(), L"упс", MB_OK | MB_TASKMODAL);
	if (b)
	{
		n_act = b;
		smena_avt();
		_g_tetron* g = tt->find1<_g_tetron>(flag_information);
		if (!g) return;
		g->add_unique_flags(b, flag_specialty);
		g->run(0, g, flag_run);
	}
}

void fun55(_tetron* tt0, _tetron* tt, u64 flags)
{
	_tetron* a = n_act;
	if (!a) return;
	_stack mem;
	a->push(&mem);
	mem.save_to_file(L"tetron.act");
}

void fun56(_tetron* tt0, _tetron* tt, u64 flags)
{
	_tetron* a = n_act;
	if (!a) return;
	_stack mem;
	mem.load_from_file(L"tetron.act");
	a->pop(&mem);
	a->run(0, a, flag_run); // чтоб восстановился
}

void fun57(_tetron* tt0, _tetron* tt, u64 flags)
{
	_g_tetron* g1 = tt->find1<_g_tetron>(flag_information);
	_g_tetron* g2 = tt->find1<_g_tetron>(flag_information2);
	if ((g1 == nullptr) || (g2 == nullptr)) return;
	_tetron* t2 = g2->find1<_tetron>(flag_specialty);
	g1->add_unique_flags(t2, flag_specialty);
	tt->_tetron::run(0, tt, flags); // продолжить выполнение
}

void fun58(_tetron* tt0, _tetron* tt, u64 flags)
{
	_g_edit64bit* b1 = tt->find1<_g_edit64bit>(flag_information);
	_g_list_link* ll = tt->find1<_g_list_link>(flag_information);
	_g_tetron* g = tt->find1<_g_tetron>(flag_information);
	if ((b1 == nullptr) || (ll == nullptr) || (g == nullptr)) return;
	_tetron* b = g->find1<_tetron>(flag_specialty);
	if (b == nullptr) return;
	_g_tetron* g0 = ll->find1<_g_tetron>(flag_specialty);
	if (g0 == nullptr) return;
	_tetron* a = g0->find1<_tetron>(flag_specialty);
	if (a == nullptr) return;
	a->add_flags(b, b1->a);
	ll->cha_area();
}

void fun59(_tetron* tt0, _tetron* tt, u64 flags)
{
	_g_tetron* g = tt->find1<_g_tetron>(flag_information);
	if (g == nullptr) return;
	_tetron* a = g->find1<_tetron>(flag_specialty);
	if (a == nullptr) return;
	delete a;
	g->run(0, g, flag_run);
}

void fun60(_tetron* tt0, _tetron* tt, u64 flags)
{
	_g_tetron* g = tt->find1<_g_tetron>(flag_information);
	if (g == nullptr) return;
	_tetron* a = g->find1<_tetron>(flag_specialty);
	if (a == nullptr) return;
	if (_t_basic_go* g_ = *a) g_->clear_go_rod(); // всегда ли?
	delete_hvost(a, true);
	g->run(0, g, flag_run);
}

void fun61(_tetron* tt0, _tetron* tt, u64 flags)
{
	_g_tetron* g = tt->find1<_g_tetron>(flag_information);
	if (g == nullptr) return;
	_tetron* a = g->find1<_tetron>(flag_specialty);
	if (a == nullptr) return;
	a->run(0, a, flag_run);
}

void fun62(_tetron* tt0, _tetron* tt, u64 flags)
{
	_g_tetron* g = tt->find1<_g_tetron>(flag_information);
	if (g == nullptr) return;
	_tetron* a = g->find1<_tetron>(flag_specialty);
	if (a == nullptr) return;
	_t_basic_go* g2 = *a;
	if (g2 == nullptr) return;
	_t_basic_go* b = *g2->copy_plus();
	_t_trans* ttr = *b;
	if (ttr == nullptr)
	{
		ttr = new _t_trans;
		ttr->add_flags(b, flag_sub_go + flag_part);
	}
	ttr->trans = n_ko->operator _t_trans * ()->trans.inverse() * g2->oko_trans();
	ttr->trans.offset.x += 16;
	ttr->trans.offset.y += 16;
	n_ko->add_flags(ttr, flag_sub_go);
	g->add_unique_flags(b, flag_specialty);
	g->run(0, g, flag_run);
}

void fun63(_tetron* tt0, _tetron* tt, u64 flags)
{
	_g_list_link* ll = tt->find1<_g_list_link>(flag_information);
	if (ll == nullptr) return;
	_g_tetron* g0 = ll->find1<_g_tetron>(flag_specialty);
	if (g0 == nullptr) return;
	_tetron* a = g0->find1<_tetron>(flag_specialty);
	if (a == nullptr) return;
	if ((ll->cursor < 0) || (ll->cursor >= (int)a->link.size())) return;
	a->link[ll->cursor]->~_link();
	ll->cha_area();
}

void fun64(_tetron* tt0, _tetron* tt, u64 flags)
{
	_g_edit64bit* b1 = tt->find1<_g_edit64bit>(flag_information);
	_g_list_link* ll = tt->find1<_g_list_link>(flag_information);
	if ((b1 == nullptr) || (ll == nullptr)) return;
	//	_g_button* mesto = tt->find1<_g_button>(flag_information);
	_g_tetron* g0 = ll->find1<_g_tetron>(flag_specialty);
	if (g0 == nullptr) return;
	_tetron* a = g0->find1<_tetron>(flag_specialty);
	if (a == nullptr) return;
	if ((ll->cursor < 0) || (ll->cursor >= (int)a->link.size())) return;
	a->link[ll->cursor]->set_flags(a, b1->a);
	ll->cha_area();
}

void fun65(_tetron* tt0, _tetron* tt, u64 flags)
{
	_g_edit64bit* b1 = tt->find1<_g_edit64bit>(flag_information);
	_g_list_link* ll = tt->find1<_g_list_link>(flag_information);
	_g1list* tip = tt->find1<_g1list>(flag_information);
	if ((b1 == nullptr) || (ll == nullptr) || (tip == nullptr)) return;
	_g_tetron* g0 = ll->find1<_g_tetron>(flag_specialty);
	if (g0 == nullptr) return;
	_tetron* a = g0->find1<_tetron>(flag_specialty);
	if (a == nullptr) return;
	i64* n = tip->find1<_t_string>(flag_specialty)->find1<i64>(flag_part);
	if (n == nullptr) return;
	uchar nn = 255;
	a->add_flags(create_tetron((uchar)(*n)), b1->a, nn);
	ll->cha_area();
}


void _t_function::run(_tetron* tt0, _tetron* tt, u64 flags)
{
	switch (a)
	{
	case  1: fun01(tt0, tt, flags); return; // нажата правая кнопка мышки
	case  2: fun02(tt0, tt, flags); return; // нажато колесо мышки
	case  3: fun03(tt0, tt, flags); return; // перемещение указателя мышки
	case  4: fun04(tt0, tt, flags); return; // поворот колеса мышки
	case  5: fun05(tt0, tt, flags); return; // нажата левая кнопка мышки
	case  6: fun06(tt0, tt, flags); return; // отжата левая кнопка мышки
	case  7: fun07(tt0, tt, flags); return; // отжата правая кнопка мышки
	case  8: fun08(tt0, tt, flags); return; // нажата кнопка клавиатуры
	case  9: fun09(tt0, tt, flags); return; // введен символ с клавиатуры
	case 10: fun10(tt0, tt, flags); return; // ! к вызывающему удалить флаги(inf2) к тетронам(inf1)
	case 11: fun11(tt0, tt, flags); return; // ! к вызывающему добавить флаги(inf2) к тетронам(inf1)
	case 12: fun12(tt0, tt, flags); return; // отжато колесо мышки
	case 13: fun13(tt0, tt, flags); return; // загрузка цен
	case 14: fun14(tt0, tt, flags); return; // создать графический объект
	case 15: fun15(tt0, tt, flags); return; // переключение режима сканирования цен
	case 16: fun16(tt0, tt, flags); return; // сканирование цен
	case 17: fun17(tt0, tt, flags); return; // перетаскивание гвоздя активного объекта
	case 18: fun18(tt0, tt, flags); return; // НОВОЕ изменение активного тетрона
	case 19: fun19(tt0, tt, flags); return; // режим торговли
	case 20: fun20(tt0, tt, flags); return; // кнопка купить акции
	case 21: fun21(tt0, tt, flags); return; // кнопка продать акции
	case 22: fun22(tt0, tt, flags); return; // size_el++
	case 23: fun23(tt0, tt, flags); return; // открыть картинку
	case 24: fun24(tt0, tt, flags); return; // центрирование графического объекта постоянно висячим
	case 25: fun25(tt0, tt, flags); return; // задать размер GPicture
	case 26: fun26(tt0, tt, flags); return; // отпускание гвоздя активного объекта
	case 27: fun27(tt0, tt, flags); return; // глобальный масштаб = 1
	case 28: fun28(tt0, tt, flags); return; // смена режима переноса
	case 29: fun29(tt0, tt, flags); return; // смена режима отображения времени
	case 30: fun30(tt0, tt, flags); return; // size_el--
	case 31: fun31(tt0, tt, flags); return;
	case 32: fun32(tt0, tt, flags); return;
	case 33: fun33(tt0, tt, flags); return; // РАЗНОЕ
	case 34: fun34(tt0, tt, flags); return;
	case 35: fun35(tt0, tt, flags); return; // купить акции
	case 36: fun36(tt0, tt, flags); return; // продать акции
	case 37: fun37(tt0, tt, flags); return; // ! создать тетрон
	case 38: fun38(tt0, tt, flags); return; // ! присвоить внутренние значения
	case 39: fun39(tt0, tt, flags); return; // задать масштаб графического объекта
	case 40: fun40(tt0, tt, flags); return; // ! выполнить от лица параметров
	case 41: fun41(tt0, tt, flags); return; // ! выполнить tt0
	case 42: fun42(tt0, tt, flags); return;
	case 43: fun43(tt0, tt, flags); return;
	case 44: fun44(tt0, tt, flags); return;
	case 45: fun45(tt0, tt, flags); return;
	case 46: fun46(tt0, tt, flags); return;
	case 47: fun47(tt0, tt, flags); return; // залить картинку цветом
	case 48: fun48(tt0, tt, flags); return; // рисование на картинке
	case 49: fun49(tt0, tt, flags); return;
	case 50: fun50(tt0, tt, flags); return; // занести картинку в буффер обмена
	case 51: fun51(tt0, tt, flags); return;
	case 52: fun52(tt0, tt, flags); return; // извлечь данные из буфера обмена в активный тетрон
	case 53: fun53(tt0, tt, flags); return; // ! создать уникальную связь - к параметром добавить вызывающего
	case 54: fun54(tt0, tt, flags); return; // поиск функций
	case 55: fun55(tt0, tt, flags); return; // сохранить активный тетрон в файл
	case 56: fun56(tt0, tt, flags); return; // загрузить активный тетрон из файла
	case 57: fun57(tt0, tt, flags); return; // копировать ссылку из GTetron в GTetron
	case 58: fun58(tt0, tt, flags); return; // создать связь
	case 59: fun59(tt0, tt, flags); return; // удалить активный тетрон
	case 60: fun60(tt0, tt, flags); return; // удалить активный тетрон с хвостами
	case 61: fun61(tt0, tt, flags); return; // выполнить активный тетрон
	case 62: fun62(tt0, tt, flags); return; // копировать активный графический тетрон с хвостами
	case 63: fun63(tt0, tt, flags); return; // удалить связь
	case 64: fun64(tt0, tt, flags); return; // изменить связь
	case 65: fun65(tt0, tt, flags); return; // создать тетрон
	}
}

_tetron* cr_one()
{
	return new _t_xy;
}

_tetron* create_tetron(const std::string& name)
{
#define make(name) {""#name, []() -> _tetron* { return new name; }}
	static std::map<std::string, std::function<_tetron* ()>> ss_tetron =
	{
		make(_tetron),
		make(_g_edit64bit),
		make(_g1list),
		make(_t_multi_string),
		make(_g_edit_int),
		make(_t_trans),
		make(_g_test_graph),
		make(_t_double),
		make(_g_link),
		make(_g_circle),
		make(_g_line),
		make(_t_xy),
		make(_g_rect),
		make(_g_froglif),
		make(_g_scrollbar),
		make(_g_color_ring),
		make(_g_tetron2),
		make(_t_string),
		make(_g_edit_string),
		make(_t_int),
		make(_g_list_link),
		make(_g_text),
		make(_g_edit_multi_string),
		make(_g_picture),
		make(_t_function),
		make(_g_edit_double),
		make(_g_button),
		make(_g_tetron),
		make(_set_graph),
		make(_g_graph)
	};
	auto fun = ss_tetron.find(name);
	if (fun == ss_tetron.end()) return nullptr;
	return fun->second();
#undef make
}

_tetron* create_tetron(uchar tipt)
{
	switch (tipt)
	{
	case  0: return new _tetron;              // 0x20
	case  1: return new _g_edit64bit;         // 0xFE
	case  2: return new _g1list;              // 0xF7
	case  3: return new _t_multi_string;      // 0x90
	case  4: return new _g_edit_int;          // 0xF9
	case  5: return new _t_trans;             // 0xF0
	case  6: return new _g_test_graph;        // 0xF2
	case  7: return new _t_double;            // 0x50
	case  8: return new _g_link;              // 0x7A
	case  9: return new _set_graph;           // 0x71
	case 10: return new _g_circle;            // 0x74
	case 11: return new _g_line;              // 0x70
	case 12: return new _g_rect;              // 0xF4
	case 13: return new _g_graph;             // 0xFC
	case 14: return new _g_froglif;           // 0x7F
	case 15: return new _t_xy;                // 0x30
	case 16: return new _g_scrollbar;         // 0xF8
	case 17: return new _g_color_ring;        // 0x72
	case 18: return new _g_tetron2;           // 0xFF

	case 24: return new _t_string;            // 0x10
	case 25: return new _g_edit_string;       // 0xFB
	case 26: return new _t_int;               // 0x40

	case 28: return new _g_list_link;         // 0xF5
	case 29: return new _g_text;              // 0xF1
	case 30: return new _g_edit_multi_string; // 0xFD
	case 31: return new _g_picture;           // 0xF6
	case 32: return new _t_function;          // 0xA0
	case 33: return new _g_edit_double;       // 0x79
	case 34: return new _g_button;            // 0xF3

	case 38: return new _g_tetron;            // 0xFA
	}
	return nullptr;
}

namespace mutator
{
	void save_to_txt_file(wstr fn)
	{
		_wjson                     tet(fn);
		std::map<u64, _tetron*> tt; // чтобы упорядочить тетроны
		for (auto i : all_tetron) tt[i->id] = i;
		tet.arr("tetrons");
		for (auto i : tt)
		{
			_tetron* t = i.second;
			tet.str().add("name", t->name()).add("id", i.first);
			t->push(tet);
			tet.arr("links");
			std::map<u64, _link*> li;
			for (auto j : t->link) li[(*j)(t)->id] = j;
			for (auto j : li) tet.str("", true).add("id", (*j.second)(t)->id).add_hex("flags", j.second->get_flags(t)).end();
			tet.end().end();
		}
		tet.end().arr("chosen");
		for (auto i : master_chosen) tet.add(i->id);
		tet.end().add("id_tetron", id_tetron);
	}

	void init_layers()
	{
		master_layers.clear();
		if (!n_go_layer) return;
		for (auto i : n_go_layer->link)
		{
			_tetron* a = (*i)(n_go_layer);
			if (!i->test_flags(a, flag_parent)) continue;
			_t_double* b = *a;
			if (b == nullptr) continue;
			double n = b->a;
			master_layers[n] = b;
		}
	}

	bool load_from_txt_file(wstr fn)
	{
		_rjson tet(fn);
		tet.arr("tetrons");
		while (!tet.error)
		{
			tet.obj();
			if (tet.null) break;
			std::string name = tet.read_string("name");
			u64 my_id;
			tet.read("id", my_id);
			id_tetron = my_id;
			_tetron* tt = create_tetron(name);
			tt->pop(tet);
			tet.arr("links");
			while (!tet.error)
			{
				tet.obj();
				if (tet.null) break;
				u64 id;
				tet.read("id", id);
				u64 flags;
				tet.read("flags", flags);
				if (id <= my_id) _id(my_id)->add_flags(_id(id), flags, false);
				tet.end();
			}
			tet.end();
			tet.end();
		}
		tet.end();
		tet.arr("chosen");
		for (uint i = 0; i < master_chosen.size(); i++)	tet.read("", master_chosen[i]->id);
		tet.end();
		tet.read("id_tetron", id_tetron);
		init_layers();
		return (tet.error == 0);
	}

	void draw(_isize r)
	{
		if (master_bm.resize(r)) master_obl_izm = r;
		if (master_obl_izm.empty()) return;
		master_bm.set_area(master_obl_izm);
		master_obl_izm &= master_bm.size;
		master_bm.clear(c_background);
		master_chain_go.clear();
		n_ko->operator _t_trans* ()->ris(_trans(), false);
		master_obl_izm.clear();
	}

	bool start(wstr fn)
	{
		master_chosen.push_back(&n_ko);                // !!корневой графический объект (трансформация)
		master_chosen.push_back(&n_s_shift);           // !!зажата клавиша Shift
		master_chosen.push_back(&n_s_alt);             // !!зажата клавиша Alt
		master_chosen.push_back(&n_s_ctrl);            // !!зажата клавиша Ctrl
		master_chosen.push_back(&n_s_left);            // зажата левая кнопка мышки
		master_chosen.push_back(&n_s_right);           // зажата правая кнопка мышки
		master_chosen.push_back(&n_s_middle);          // зажато колесо мышки
		master_chosen.push_back(&n_s_double);          // двойной щелчок мышки
		master_chosen.push_back(&n_down_left);         // нажата левая кнопка мышки
		master_chosen.push_back(&n_down_right);        // нажата правая кнопка мышки
		master_chosen.push_back(&n_down_middle);       // нажата средняя кнопка мышки (колесо)
		master_chosen.push_back(&n_up_left);           // отжата левая кнопка мышки
		master_chosen.push_back(&n_up_right);          // отжата правая кнопка мышки
		master_chosen.push_back(&n_up_middle);         // отжата средняя кнопка мышки (колесо)
		master_chosen.push_back(&n_move);              // перемещен курсор мышки
		master_chosen.push_back(&n_wheel);             // повернуто колесо мышки
		master_chosen.push_back(&n_hint);              // подсказка для элемента под курсором мышки
		master_chosen.push_back(&n_hex);               // АБСТРАКТНЫЙ ПАРАЗИТ 16-ричная система счисления
		master_chosen.push_back(&n_go_move);           // !!цепочка!! GO, над которым перемещается мышка
		master_chosen.push_back(&n_perenos);           // режим переноса
		master_chosen.push_back(&n_move_all);          // вызывается после глобального смещения или масштабирования
		master_chosen.push_back(&n_ramk);              // рамка выделения потенциально активного
		master_chosen.push_back(&n_ramk2);             // рамка выделения активного
		master_chosen.push_back(&n_pot_act);           // потенциально активный тетрон
		master_chosen.push_back(&n_act);               // активный тетрон
		master_chosen.push_back(&n_zagolovok);         // АБСТРАКТНЫЙ ПРЕДОК - заголовки
		master_chosen.push_back(&n_checkbox);          // АБСТРАКТНЫЙ ПРЕДОК - переключатель
		master_chosen.push_back(&n_go_layer);          // АБСТРАКТНЫЙ ПРЕДОК - графический слой
		master_chosen.push_back(&n_color_line);        // АБСТРАКТНЫЙ ПРЕДОК - цвет линий и текста
		master_chosen.push_back(&n_color_bg);          // АБСТРАКТНЫЙ ПРЕДОК - цвет фона
		master_chosen.push_back(&n_act_key);           // активный тетрон для управления клавиатурой
		master_chosen.push_back(&n_down_key);          // нажата кнопка клавиатуры
		master_chosen.push_back(&n_press_key);         // введен символ
		master_chosen.push_back(&n_timer1000);         // таймер с периодом 1000
		master_chosen.push_back(&n_tani);              // объект, который тянется, перемещается
		master_chosen.push_back(&n_fun_tani0);         // АБСТРАКТНЫЙ ПРЕДОК функция начала тяни-толкай
		master_chosen.push_back(&n_fun_tani);          // АБСТРАКТНЫЙ ПРЕДОК функция тяни-толкай
		master_chosen.push_back(&n_fun_tani1);         // АБСТРАКТНЫЙ ПРЕДОК функция конца тяни-толкай
		master_chosen.push_back(&n_temp_go);           // АБСТРАКТНЫЙ ПРЕДОК временный графический объект
		master_chosen.push_back(&n_center);            // АБСТРАКТНЫЙ ПРЕДОК центр
		master_chosen.push_back(&n_radius);            // АБСТРАКТНЫЙ ПРЕДОК радиус
		master_chosen.push_back(&n_width);             // АБСТРАКТНЫЙ ПРЕДОК толщина
		master_chosen.push_back(&n_begin);             // АБСТРАКТНЫЙ ПРЕДОК начало
		master_chosen.push_back(&n_end);               // АБСТРАКТНЫЙ ПРЕДОК конец
		master_chosen.push_back(&n_mouse_inactive);    // АБСТРАКТНАЯ ИНФОРМАЦИЯ неактивный для перемещения мышки
		master_chosen.push_back(&n_start_mouse_move);  // АБСТРАКТНЫЙ ПРЕДОК функция начала перемещения мышки над объектом
		master_chosen.push_back(&n_mouse_move);        // АБСТРАКТНЫЙ ПРЕДОК функция перемещения мышки над объектом
		master_chosen.push_back(&n_finish_mouse_move); // АБСТРАКТНЫЙ ПРЕДОК функция конца перемещения мышки над объектом
		master_chosen.push_back(&n_fun_up_middle);     // АБСТРАКТНЫЙ ПРЕДОК функция отжато колесо мышки
		master_chosen.push_back(&n_timer250);          // АБСТРАКТНЫЙ ПРЕДОК функция отжато колесо мышки

		return load_from_txt_file(fn);
	}

	double get_main_scale()
	{
		_t_trans* tr = *n_ko;
		return tr->trans.scale;
	}

	void mouse_button_left(bool pressed)
	{
		*n_s_left->operator i64* () = pressed;
		if (pressed) n_down_left->run(0, n_down_left, flag_run); else n_up_left->run(0, n_up_left, flag_run);
	}

	void mouse_button_right(bool pressed)
	{
		*n_s_right->operator i64* () = pressed;
		if (pressed) n_down_right->run(0, n_down_right, flag_run); else n_up_right->run(0, n_up_right, flag_run);
	}

	void mouse_button_middle(bool pressed)
	{
		*n_s_middle->operator i64* () = pressed;
		if (pressed) n_down_middle->run(0, n_down_middle, flag_run); else n_up_middle->run(0, n_up_middle, flag_run);
	}

}