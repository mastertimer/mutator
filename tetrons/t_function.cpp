#include "t_function.h"
#include "g_exchange_graph.h"
#include "exchange_trade.h"
#include "mediator.h"
#include "rnd.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace
{
	_stack clipboard; // буфер обмена
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

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
		set_cursor((*n_perenos->operator i64 * ()) ? (_cursor::size_all) : ((*n_s_right->operator i64 * ()) ?
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
					set_cursor(_cursor::drag);
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
				if (n_ramk2 == xx) set_cursor(_cursor::normal); else xx->mouse_finish_move();
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
	set_cursor(_cursor::hand_point);
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
		_area pr = { {a.x.max - 5, a.x.max}, {0.1 * a.y, 0.9 * a.y} };
		_area ni = { {0.1 * a.x, 0.9 * a.x}, {a.y.max - 5, a.y.max} };
		if (pr.test(to))
			set_cursor(_cursor::size_we);
		else
			if (ni.test(to))
				set_cursor(_cursor::size_ns);
			else
				set_cursor(_cursor::drag);
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
		kor->trans.scale_up(tr, pow(1.1, kk));
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
			rat->trans.scale_up((ra->oko_trans() / rat->trans).inverse(tr), pow(1.1, *n_wheel->operator i64 * ()));
			ra->add_area();
			raa->del_area();
			rat2->trans.scale_up((raa->oko_trans() / rat2->trans).inverse(tr), pow(1.1, *n_wheel->operator i64 * ()));
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

void fun13(_tetron* tt0, _tetron* tt, u64 flags)
{
	start_stock();
}

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

void fun16(_tetron* tt0, _tetron* tt, u64 flags)
{
	scan_supply_and_demand();
}

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

void fun19(_tetron* tt0, _tetron* tt, u64 flags)
{
	_g_button* b = *tt0;
	change_can_trade(b->checked);
}

void fun20(_tetron* tt0, _tetron* tt, u64 flags)
{
	buy_shares();
}

void fun21(_tetron* tt0, _tetron* tt, u64 flags)
{
	sell_shares();
}

void fun22(_tetron* tt0, _tetron* tt, u64 flags)
{
	expand_elements_graph();
}

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
	kor->trans.scale_up(tr, 1 / kor->trans.scale);
	kor->trans.scale = 1; // точно единица
	kor->cha_area(kor->calc_area());
	n_move_all->run(0, n_move_all, flag_run);
}

void fun28(_tetron* tt0, _tetron* tt, u64 flags)
{
	*n_perenos->operator i64* () = !(*n_perenos->operator i64 * ());
	set_cursor((*n_perenos->operator i64 * ()) ? (_cursor::size_all) : ((*n_s_right->operator i64 * ()) ?
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

void fun30(_tetron* tt0, _tetron* tt, u64 flags)
{
	narrow_graph_elements();
}

void fun31(_tetron* tt0, _tetron* tt, u64 flags)
{
	ed.save_to_file();
}

void fun32(_tetron* tt0, _tetron* tt, u64 flags)
{
}

void show_message(std::string_view s, double b)
{
	MessageBoxA(0, ((std::string)s + " = " + std::to_string(b)).c_str(), "информация", MB_OK | MB_TASKMODAL);
}

void test2()
{
	double p[256] = { 1 }; // частоты (ненормированные)
	for (i64 i = 1; i < 21; i++) p[i] = 0.0064;

	constexpr i64 n = 1000000; // длина последовательности

	double s = 0;
	for (auto i : p) s += i;
	for (auto& i : p) i /= s;

	i64 c[256]; // количество символов
	i64 ss = 0;
	for (i64 i = 255; i > 0; i--)
	{
		c[i] = p[i] * n + 0.5;
		ss += c[i];
	}
	c[0] = n - ss;

	double e0 = 0; // идеальный размер
	for (auto i : c)
		if (i > 0)
			e0 += i * log(((double)i) / n);
	e0 /= -log(2.0);
	show_message("e0 (%)", 100.0 * e0 / (8 * n));

	//	return;

	std::vector<uchar> a;

	i64 nn = n;
	while (nn) // пока не исчерпаны цифры
	{
		i64 ii = rnd(nn);
		i64 q = 0;
		for (i64 i = 0; i < 256; i++) // i - сработавший символ
		{
			q += c[i];
			if (ii >= q) continue;
			a.push_back(i);
			c[i]--;
			break;
		}
		nn--;
	}

	auto calc_de = [&](double k)
	{
		double pp[256];
		for (auto& i : pp) i = k;
		double summ_pp = k * 256;

		double e = 0; // реальный размер

		for (auto i : a) // сгенерированная заранее последовательность
		{
			e += log(pp[i] / summ_pp);
			pp[i]++;
			summ_pp++;
		}
		e /= -log(2.0);
		return (e - e0) / 8;
	};

	// подбор идельного k
	double dk = 2;
	double k = 1;
	double e = calc_de(k);
	for (;;)
	{
		double k1 = k * dk;
		double e1 = calc_de(k1);
		if (e1 < e)
		{
			e = e1;
			k = k1;
			continue;
		}
		k1 = k / dk;
		e1 = calc_de(k1);
		if (e1 < e)
		{
			e = e1;
			k = k1;
			continue;
		}
		break;
	}
	// еще разочек
	dk = 1.02;
	for (;;)
	{
		double k1 = k * dk;
		double e1 = calc_de(k1);
		if (e1 < e)
		{
			e = e1;
			k = k1;
			continue;
		}
		k1 = k / dk;
		e1 = calc_de(k1);
		if (e1 < e)
		{
			e = e1;
			k = k1;
			continue;
		}
		break;
	}

	show_message("k", k);
	show_message("e", e);
}

void fun33(_tetron* tt0, _tetron* tt, u64 flags)
{
	test2();
}

void fun34(_tetron* tt0, _tetron* tt, u64 flags)
{
}

void fun35(_tetron* tt0, _tetron* tt, u64 flags)
{
	buy_stock(tt, true);
}

void fun36(_tetron* tt0, _tetron* tt, u64 flags)
{
	buy_stock(tt, false);
}

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
	g->pic.line(r1, r2, (*n_s_shift->operator i64 * ()) ? 0 : cc1, true);
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

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

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
	case 31: fun31(tt0, tt, flags); return; // сохраниние цен
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
