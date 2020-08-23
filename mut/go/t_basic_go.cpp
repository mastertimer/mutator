#include "basic_tetrons.h"
#include "g_scrollbar.h"
#include "t_basic_go.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

_trans    master_trans_go;      // трансформация тяни-толкай объекта, или объекта под мышкой
_chain_go master_chain_go;      // активная цепочка графических объектов
bool      time_ris = false;     // отображать время рисования
_trans    master_trans_go_move; // трансформация n_go_move
_xy       par_koo1;             // .....вспомогательная переменная  !!!ИЗБАВИТЬСЯ!!!

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

_area_old _t_basic_go::calc_area()
{ // рекурсия невозможна
	if (area.type != _tarea::indefinite) return area;
	if (_t_go* tgo = *this)
		area = tgo->local_area;
	else
		area = _tarea::empty;
	for (auto i : link)
	{
		_tetron* a = i->pairr(this);
		if (!(i->get_flags(this) & (flag_parent + flag_sub_go))) continue;
		_t_basic_go* b = *a; if (b == nullptr) continue;
		b->calc_area();
		if (i->test_flags(this, flag_parent))
			area += b->area;
		else
		{
			if (_t_trans* ttr = *b)
				area += ttr->trans(b->area);
			else
				area += b->area;
		}
	}
	return area;
}

void _t_basic_go::add_area(_area_old a)
{
	static _speed<_hash_table_tetron> hash(false);
	bool start = (hash.a == nullptr);
	if (start) hash.start();
	_t_go* tgo = *this;
	_t_trans* ttr = *this;
	if (a.type == _tarea::indefinite)
		a = calc_area();
	else
	{
		if (!(a <= area)) area.type = _tarea::indefinite;
		if (n_ko == this)
		{
			add_obl_izm((tgo) ? a : ttr->trans(a));
			if (start) hash.stop(); // лишнее (предохранитель на будущее)
			return;
		}
	}
	hash->insert(this);
	for (auto i : link)
	{
		_tetron* t = i->pairr(this);
		_t_basic_go* b = *t; if (b == nullptr) continue;
		if (hash->find(t)) continue; // рекурсия не допускается
		if (i->test_flags(t, flag_sub_go)) b->add_area((tgo) ? a : ttr->trans(a)); // можно ли
		if (i->test_flags(t, flag_parent)) b->add_area(a); // И то И то?
	}
	hash->erase(this);
	if (start) hash.stop();
}

void _t_basic_go::cha_area(_trans tr)
{
	_t_go* tgo = *this;
	if (!tgo) return;
	add_obl_izm(tr(tgo->local_area));
}

void _t_basic_go::cha_area(_area_old a)
{
	static _speed<_hash_table_tetron> hash(false);
	bool start = (hash.a == nullptr);
	if (start) hash.start();
	_t_go* tgo = *this;
	_t_trans* ttr = *this;
	if (n_ko == this)
	{
		add_obl_izm((tgo) ? a : ttr->trans(a));
		if (start) hash.stop(); // лишнее (предохранитель на будущее)
		return;
	}
	if (a.type == _tarea::indefinite)
		if (tgo) a = tgo->local_area;
	hash->insert(this);
	for (auto i : link)
	{
		_tetron* t = i->pairr(this);
		_t_basic_go* b = *t; if (b == nullptr) continue;
		if (hash->find(t)) continue; // рекурсия не допускается
		if (i->test_flags(t, flag_sub_go)) b->cha_area((tgo) ? a : ttr->trans(a)); // можно ли
		if (i->test_flags(t, flag_parent)) b->cha_area(a); // И то И то?
	}
	hash->erase(this);
	if (start) hash.stop();
}

void _t_basic_go::del_area(_area_old a)
{
	static _speed<_hash_table_tetron> hash(false);
	bool start = (hash.a == nullptr);
	if (start) hash.start();
	_t_go* tgo = *this;
	_t_trans* ttr = *this;
	if (a.type == _tarea::indefinite)
		a = calc_area();
	else
	{
		if (!(a < area)) area.type = _tarea::indefinite;
		if (n_ko == this)
		{
			add_obl_izm((tgo) ? a : ttr->trans(a));
			if (start) hash.stop(); // лишнее (предохранитель на будущее)
			return;
		}
	}
	hash->insert(this);
	for (auto i : link)
	{
		_tetron* t = i->pairr(this);
		_t_basic_go* b = *t; if (b == nullptr) continue;
		if (hash->find(t)) continue; // рекурсия не допускается
		if (i->test_flags(t, flag_sub_go)) b->del_area((tgo) ? a : ttr->trans(a)); // можно ли
		if (i->test_flags(t, flag_parent)) b->del_area(a); // И то И то?
	}
	hash->erase(this);
	if (start) hash.stop();
}

_trans _t_basic_go::oko_trans(bool* ko)
{
	_trans a;
	_tetron* b = this;
	_speed<_hash_table_tetron> hash;
	bool nai = true;
	while (nai)
	{
		if (_t_trans* bb = *b) a = bb->trans * a;
		hash->insert(b);
		nai = false;
		for (auto i : b->link)
		{
			_tetron* aa = i->pairr(b);
			if (!i->test_flags(aa, flag_sub_go)) continue;
			if (hash->find(aa)) continue;
			if (!aa->operator _t_basic_go * ()) continue;
			b = aa;
			nai = true;
			break;
		}
	}
	if (ko) *ko = (n_ko == b);
	return a;
}

void _t_basic_go::after_create_link(_link* li)
{
	_tetron::after_create_link(li);
	if (li->test_flags(this, inverted_flags(flag_sub_go)))
		if (li->pairr(this)->operator _t_basic_go * ()) add_area();
}

void _t_basic_go::before_delete_link(_link* li)
{
	_tetron::before_delete_link(li);
	if (li->test_flags(this, inverted_flags(flag_sub_go)))
		if (li->pairr(this)->operator _t_basic_go * ()) del_area();
}

void _t_basic_go::set_layer(int n)
{
	_id tt = master_layers[n];
	_t_int* t;
	if (!tt)
	{
		t = new _t_int;
		t->a = n;
		t->add_flags(n_go_layer, flag_parent);
		master_layers[n] = t;
	}
	else
		t = *tt;
	t->add_flags(this, flag_information);
}

bool _t_basic_go::mouse_down_left(_trans tr)
{
	for (int i = (int)link.size() - 1; i >= 0; i--)
	{
		_tetron* a = link[i]->pairr(this);
		if (!link[i]->test_flags(this, flag_sub_go)) continue;
		if (_t_basic_go* aa = *a)
		{
			_trans tr2;
			if (_t_go* tgo = *a)
				tr2 = tr;
			else
				tr2 = tr * a->operator _t_trans * ()->trans;
			if (!aa->area.test(tr2.inverse(mouse_xy))) continue;
			if (aa->mouse_down_left(tr2)) return true;
		}
	}
	_xy r = tr.inverse(mouse_xy);
	if (_t_go* tgo = *this)
		if (tgo->test_local_area(r))
		{
			if (tgo->key_fokus)
			{
				_tetron* b = 0;
				if (n_act_key)
					if (n_act_key != this)
						b = n_act_key;
				n_act_key = this;
				if (b) b->operator _t_basic_go* ()->cha_area();
				cha_area();
			}
			if (tgo->mouse_down_left2(r))
			{
				n_tani = this;
				master_trans_go = tr;
				return true;
			}
			if (tgo->key_fokus) return true;
		}
	for (int i = (int)link.size() - 1; i >= 0; i--)
	{
		_tetron* a = link[i]->pairr(this);
		if (!link[i]->test_flags(this, flag_parent)) continue;
		if (_t_basic_go* aa = *a)
		{
			if (!aa->area.test(r)) continue;
			if (aa->mouse_down_left(tr)) return true;
		}
	}
	return false;
}

void _t_basic_go::find_pot_act(_xy r)
{
	for (int i = (int)link.size() - 1; i >= 0; i--)
	{
		_tetron* a = link[i]->pairr(this);
		if (!link[i]->test_flags(this, flag_sub_go)) continue;
		if (_t_basic_go* aa = *a)
		{
			if (n_ramk == aa) continue;//?только здесь
			if (n_ramk2 == aa) continue;
			_xy r2;
			if (_t_go* tgo = *aa)
				r2 = r;
			else
				r2 = aa->operator _t_trans * ()->trans.inverse(r);
			if (!aa->calc_area().test(r2)) continue;
			aa->find_pot_act(r2);
			if (n_pot_act) return;
			continue;
		}
	}
	if (_t_go* tgo = *this)
		if (tgo->test_local_area(r))
			n_pot_act = this;
}

bool _t_basic_go::mouse_wheel(_trans tr)
{
	for (int i = (int)link.size() - 1; i >= 0; i--)
	{
		_tetron* a = link[i]->pairr(this);
		if (!link[i]->test_flags(this, flag_sub_go)) continue;
		if (_t_basic_go* aa = *a)
		{
			_trans tr2;
			if (_t_go* tgo = *a)
				tr2 = tr;
			else
				tr2 = tr * a->operator _t_trans * ()->trans;
			if (!aa->area.test(tr2.inverse(mouse_xy))) continue;
			if (aa->mouse_wheel(tr2)) return true;
		}
	}
	_xy r = tr.inverse(mouse_xy);
	if (_t_go* tgo = *this)
		if (tgo->test_local_area(r)) // ДЕЙСТВИЕ
		{
			master_trans_go = tr;
			if (tgo->mouse_wheel2(r)) return true;
		}
	for (int i = (int)link.size() - 1; i >= 0; i--)
	{
		_tetron* a = link[i]->pairr(this);
		if (!link[i]->test_flags(this, flag_parent)) continue;
		if (_t_basic_go* aa = *a)
		{
			if (!aa->area.test(r)) continue;
			if (aa->mouse_wheel(tr)) return true;
		}
	}
	return false;
}

bool _t_basic_go::final_fractal(const _trans& tr)
{
	if (tr(calc_area()).radius() < final_radius()) return true;
	auto h = master_chain_go.hash.find(this);
	if (!h) return false;
	return (tr.scale >= h->a.tr.scale);
}

void _t_basic_go::priem_gv()
{
	_t_go* c = *n_act;
	if (c == this) return;
	_trans tr = c->oko_trans();
	c->clear_go_rod();
	_t_trans* trr = set_t_trans(c, flag_part + flag_sub_go);
	trr->del_area();
	trr->trans = oko_trans().inverse() * tr;
	trr->area = _tarea::indefinite;
	trr->add_area();
}

_t_trans* _t_basic_go::set_t_trans(_tetron* go, uint64 flags)
{
	_t_trans* ttr = nullptr;
	if (link.size() <= go->link.size())
	{
		for (auto i : link)
			if (_t_trans* b = *i->pairr(this))
				if (b->get_flags(go))
				{
					ttr = b;
					break;
				}
	}
	else
	{
		for (auto i : go->link)
			if (_t_trans* b = *i->pairr(go))
				if (b->get_flags(this))
				{
					ttr = b;
					break;
				}
	}
	if (flags == 0)
	{
		delete ttr;
		return nullptr;
	}
	if (!ttr) ttr = new _t_trans;
	ttr->add_flags(go, flags);
	add_flags(ttr, flags);
	return ttr;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void _t_trans::ris(_trans tr, bool final)
{
	tr *= trans;
	if (!((tr(calc_area())) & master_obl_izm)) return;
	if (!final) final = final_fractal(tr);
	master_chain_go.push(this, tr);
	for (_frozen_link a(this, flag_parent); a; a++)
	{
		if (_t_go * b = *a)
			b->ris(tr, final);
		else
			if (_t_trans * b = *a)
				b->ris(tr / b->trans, final);
	}
	for (_layers_go a(this); a; a++) a->ris(tr, final);
	master_chain_go.pop();
}

void _t_trans::clear_go_rod()
{
	for (int i = (int)link.size() - 1; i >= 0; i--)
	{
		_tetron* a = link[i]->pairr(this);
		//		if (!a->operator TGO*()) continue;
		if (!link[i]->test_flags(a, flag_sub_go)) continue;
		link[i]->~_link();
	}
}

bool _t_trans::mouse_move(_trans tr, bool final)
{
	if (!final) final = final_fractal(tr);
	master_chain_go.push(this, tr);
	for (int i = (int)link.size() - 1; i >= 0; i--)
	{
		_tetron* a = link[i]->pairr(this);
		if (!link[i]->test_flags(this, flag_sub_go)) continue;
		if (_t_basic_go * aa = *a)
		{
			_trans tr2;
			if (_t_go * tgo = *a)
				tr2 = tr;
			else
				tr2 = tr * a->operator _t_trans * ()->trans;
			if (!aa->calc_area().test(tr2.inverse(mouse_xy))) continue;
			if (aa->mouse_move(tr2, final)) return true;
		}
	}
	_xy r = tr.inverse(mouse_xy);
	for (int i = (int)link.size() - 1; i >= 0; i--)
	{
		_tetron* a = link[i]->pairr(this);
		if (!link[i]->test_flags(this, flag_parent)) continue;
		if (_t_basic_go * aa = *a)
		{
			if (!aa->calc_area().test(r)) continue;
			if (aa->mouse_move(tr, final)) return true;
		}
	}
	master_chain_go.pop();
	return false;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void _t_go::mouse_finish_move()
{
	static _vector_id var; // ??
	var.clear();
	find_all_intermediate(n_finish_mouse_move, flag_information, flag_parent, var);
	for (auto& i : var)
	{
		if (!i) continue;
		i->run(this, i, flag_run);
	}
}

bool _t_go::mouse_move2(_xy r)
{
	static _vector_id var; // ??
	if (test_flags(n_mouse_inactive, flag_information)) return false;

	if (n_go_move != this) // первое перемещение
	{
		var.clear();
		find_all_intermediate(n_start_mouse_move, flag_information, flag_parent, var);
		for (auto& i : var)
		{
			if (!i) continue;
			i->run(this, i, flag_run);
		}
	}

	return true;
}

uint _t_go::get_c()
{
	_tetron* a = find_intermediate<_tetron>(n_color_line, flag_information, flag_parent);
	if (a == nullptr) return c_def;
	if (int64 * c = *a) return (uint)* c;
	return c_def;
}

uint _t_go::get_c2()
{
	_tetron* a = find_intermediate<_tetron>(n_color_bg, flag_information, flag_parent);
	if (a == nullptr) return c2_default;
	if (int64 * c = *a) return (uint)* c;
	return c2_default;
}

void _t_go::set_c(uint c)
{
	_tetron* a = find_intermediate<_tetron>(n_color_line, flag_information, flag_parent);
	if (a != nullptr)
	{
		if (int64 * cc = *a)* cc = c;
		return;
	}
	if (c == c_def) return;
	_t_int* ti = new _t_int;
	ti->a = c;
	ti->add_flags(n_color_line, flag_parent);
	add_flags(ti, flag_information | flag_part);
}

void _t_go::set_c2(uint c)
{
	_tetron* a = find_intermediate<_tetron>(n_color_bg, flag_information, flag_parent);
	if (a != nullptr)
	{
		if (int64 * cc = *a)* cc = c;
		return;
	}
	if (c == c2_default) return;
	_t_int* ti = new _t_int;
	ti->a = c;
	ti->add_flags(n_color_bg, flag_parent);
	add_flags(ti, flag_information | flag_part);
}

_t_go::_t_go() : _t_basic_go(), local_area(_tarea::empty), key_fokus(false)
{
}

void _t_go::ris(_trans tr, bool final)
{
	if (!((tr(calc_area())) & master_obl_izm)) return;
	if (!final) final = final_fractal(tr);
	master_chain_go.push(this, tr);
	for (_frozen_link a(this, flag_parent); a; a++)
	{
		if (_t_go * b = *a)
			b->ris(tr, final);
		else
			if (_t_trans * b = *a)
				b->ris(tr / b->trans, final);
	}
	auto ris_all = [&]() {
		ris2(tr, final);
		if (!final)
			for (_layers_go a(this); a; a++) a->ris(tr, final);
	};

	if (!time_ris)
	{
		ris_all();
		master_chain_go.pop();
	}
	else
	{
		auto t0 = std::chrono::high_resolution_clock::now();
		ris_all();
		auto t = std::chrono::high_resolution_clock::now() - t0;
		master_chain_go.pop();
		_area_old oo = tr(local_area);
		if (oo.x.min < 0) oo.x.min = 0;
		if (oo.y.min < 0) oo.y.min = 0;
		std::wstring s = double_to_string(t.count() / 1000000.0, 2);
		master_bm.text((int)(oo.x.min + 1), (int)oo.y.min, s.c_str(), 13, c_max, 0x00000000);
	}
}

_t_trans* _t_go::ttrans()
{
	for (auto i : link)
	{
		_tetron* a = i->pairr(this);
		if (!i->test_flags(a, flag_sub_go)) continue;
		if (_t_trans * ttg = *a) return ttg;
	}
	return nullptr;
}

void _t_go::clear_go_rod()
{
	for (int i = (int)link.size() - 1; i >= 0; i--)
	{
		_t_trans* tr = *link[i]->pairr(this);
		if (!tr) continue;
		if (!link[i]->test_flags(tr, flag_sub_go + flag_part)) continue;
		delete tr;
	}
}

void _t_go::resize()
{
	_g_scrollbar* po = find1<_g_scrollbar>(flag_part);
	if (!po) return;
	po->prilip(this);
}

void _t_go::mouse_up_middle()
{
	static _vector_id var; // ??
	var.clear();
	find_all_intermediate(n_fun_up_middle, flag_information, flag_parent, var);
	for (auto& i : var)	if (i) i->run(this, i, flag_run);
}

bool _t_go::mouse_move(_trans tr, bool final)
{
	if (!final) final = final_fractal(tr);
	master_chain_go.push(this, tr);
	if (!final)
		for (int i = (int)link.size() - 1; i >= 0; i--)
		{
			_tetron* a = link[i]->pairr(this);
			if (!link[i]->test_flags(this, flag_sub_go)) continue;
			if (_t_basic_go * aa = *a)
			{
				_trans tr2;
				if (_t_go * tgo = *a)
					tr2 = tr;
				else
					tr2 = tr * a->operator _t_trans * ()->trans;
				if (!aa->calc_area().test(tr2.inverse(mouse_xy))) continue;
				if (aa->mouse_move(tr2, final)) return true;
			}
		}
	_xy r = tr.inverse(mouse_xy);
	if (test_local_area(r)) // ДЕЙСТВИЕ
	{
		master_trans_go = tr;
		if (mouse_move2(r))
		{
			if ((n_go_move != this) || (master_trans_go_move != master_trans_go))
			{
				if (n_go_move)
				{
					_t_go* xx = *n_go_move;
					if (xx)
					{
						if (n_ramk2 == xx) set_cursorx(_cursor::normal); else xx->mouse_finish_move();
					}
				}
				n_go_move = this;
				master_trans_go_move = master_trans_go;
			}
			return true;
		}
	}
	for (int i = (int)link.size() - 1; i >= 0; i--)
	{
		_tetron* a = link[i]->pairr(this);
		if (!link[i]->test_flags(this, flag_parent)) continue;
		if (_t_basic_go * aa = *a)
		{
			if (!aa->calc_area().test(r)) continue;
			if (aa->mouse_move(tr, final)) return true;
		}
	}
	master_chain_go.pop();
	return false;
}

bool _t_go::mouse_down_left2(_xy r)
{
	_tetron* a = find_intermediate<_tetron>(n_fun_tani0, flag_information, flag_parent);
	if (!a) return false;
	par_koo1 = r;
	a->run(this, a, flag_run);
	return true;
}

void _t_go::mouse_move_left2(_xy r)
{
	_tetron* a = find_intermediate<_tetron>(n_fun_tani, flag_information, flag_parent);
	par_koo1 = r;
	if (a) a->run(this, a, flag_run);
}

void _t_go::mouse_up_left2(_xy r)
{
	_tetron* a = find_intermediate<_tetron>(n_fun_tani1, flag_information, flag_parent);
	par_koo1 = r;
	if (a) a->run(this, a, flag_run);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void _chain_go::push(_t_basic_go* a, _trans& tr)
{
	chain.push_back(a);
	auto n = hash.find(a);
	if (!n)
	{
		*n = { a, { 1, tr } };
		n.life();
	}
	else
		n->a.k++;
}

void _chain_go::pop()
{
	_t_basic_go* a = chain.back();
	chain.pop_back();
	auto n = hash.find(a);
	n->a.k--;
	if (n->a.k == 0) hash.erase(n);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void _layers_go::operator++(int)
{
	for (++i_m; i_m != map_.end(); ++i_m)
	{
		_tetron* t2 = *i_m->second;
		if (!t2) continue;
		if (!tetron->test_flags(t2, flag_sub_go)) continue;
		tetron2 = (_t_basic_go*)t2;
		return;
	}
	tetron2 = nullptr;
}

_layers_go::_layers_go(_tetron* t) : tetron(t)
{
	for (auto j : t->link)
	{
		_tetron* tt = j->pairr(t);
		if (!j->test_flags(t, flag_sub_go)) continue;
		if (tt->operator _t_basic_go *() == nullptr) continue;
		_t_int* ti = tt->find_intermediate<_t_int>(n_go_layer, inverted_flags(flag_information), flag_parent);
		int n = (ti) ? (int)ti->a : 0;
		map_.insert({n, tt});
	}
	i_m = map_.begin();
	if (i_m != map_.end()) tetron2 = *i_m->second;
}
