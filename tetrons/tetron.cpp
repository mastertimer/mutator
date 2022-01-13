#include "tetron.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace
{
	constexpr uint color_set[32] = { // набор разных цветов одной яркости
		0xFF0080FD, 0xFFEF0000, 0xFF9E3BFF, 0xFF938700, 0xFF12AA00, 0xFFD34E0D, 0xFF7470DC, 0xFF4D9682,
		0xFFC80FCE, 0xFFB06381,	0xFFE22653, 0xFF009D93, 0xFFC4439D, 0xFF258ECB, 0xFF659B00, 0xFF3F77FF,
		0xFF887E87, 0xFF9659DD, 0xFFB57000, 0xFF39A337, 0xFF5D84BE, 0xFFAF1FFB, 0xFFDE028A, 0xFF768F51,
		0xFFB400FC,	0xFFE23700, 0xFFCB29B2, 0xFFA3755A, 0xFFC45D37, 0xFFEB171B, 0xFF966DA8, 0xFFAC4ACF };
}


__hash_table<_link> glink;

uint hash_func(const _he_intermediate& a)
{
	return (uint)((((u64)a.tetron_before) >> 4) * 27644437 + (((u64)a.tetron_after) >> 4) * 33391 +
	              a.flags_before * 16769023 + a.flags_after * 17971);
}

uint hash_func(const _pair_tetron& a)
{
	return (uint)((((u64)a.low_tetron) >> 4) * 27644437 + (((u64)a.high_tetron) >> 4) * 33391);
}

void os_pordis(double min, double max, i64 maxN, double& mi, double& step, double min_step)
{
	i64 n;
	double step2;
	if (maxN < 2) maxN = 2;
	step = exp(round(log((max - min) / maxN) / log(10)) * log(10));
	auto fun = [&]()
	{
		mi = (i64(min / step)) * step;
		if (mi < min) mi += step;
		n = (i64((max - mi) / step)) + 1;
	};
	do
	{
		fun();
		if (n < maxN) step = step * 0.1; else break;
	} while (true);
	while (n > maxN)
	{
		step = step * 10;
		fun();
	}
	step2 = step;
	step = step2 * 0.2;
	fun();
	if (n <= maxN) goto end;
	step = step2 * 0.5;
	fun();
	if (n <= maxN) goto end;
	step = step2;
	fun();
end:
	if (step < min_step)
	{
		step = min_step;
		fun();
	}
}

_tetron::_tetron()
{
	all_tetron.insert(id = id_tetron++, this);
}

void _tetron::find_all_intermediate(_tetron* t, u64 flags_before, u64 flags_after, _vector_id& res)
{ // !! может дублироватьс¤
	for (auto i : link)
	{
		_tetron* a = (*i)(this);
		if (!i->test_flags(this, flag_parent)) continue;
		a->find_all_intermediate(t, flags_before, flags_after, res);
	}
	_tetron* tt = find_intermediate<_tetron>(t, flags_before, flags_after);
	if (tt) res.push_back(tt);
}

std::string _tetron::name()
{
	std::string s = typeid(*this).raw_name();
	return s.substr(4, s.size() - 6);
}

namespace super_del_tetron2
{
	_speed<_vector_tetron>          ud(false); // список на удаление
	_speed<_hash_table_tetron>      Sp(false); // полный список
	__hash_table<_pair_t<_tetron*>> Sp2;       // полный список 2 проход
	_speed<_hash_table_tetron>      Ba(false); // плохой список
	_tetron*                        start;     // стартовый особый тетрон

	void metka_del_B12(_tetron* b)
	{
		if (Sp2.find(b)) return;
		if (Ba->find(b)) return;
		Sp2.insert(b);
		ud->push_back(b);
		for (auto i : b->link)
		{
			_tetron* a = (*i)(b);
			if (!i->test_flags(b, flag_part)) continue;
			metka_del_B12(a);
		}
	}

	bool is_my(_tetron* b)
	{
		for (auto i : b->link)
		{
			_tetron* a = (*i)(b);
			if (!i->test_flags(a, flag_part)) continue;
			if (a == start)
				continue; // NEW
			            // !!! могут удалиться нужные родители !!!
			            // —делать 2 прохода јнтиметок, если q2 точно != q
			            // если да, проверить NEW
			            // 2й проход 1й проход
			if ((Ba->find(a)) || (!Sp->find(a))) return false;
		}
		return true;
	}

	void anti_metka_B12(_tetron* b)
	{
		if (Sp2.find(b)) return;
		if (Ba->find(b)) return;
		Sp2.insert(b);
		if (is_my(b))
		{
			for (auto i : b->link)
			{
				_tetron* a = (*i)(b);
				if (!i->test_flags(b, flag_part)) continue;
				anti_metka_B12(a);
			}
		}
		else
			b->traversal(Ba, flag_part);
	}

	void prepare_sp_unik_sv(_tetron* b) // подготовить список уникальный свойств тетрона
	{
		Sp.start();
		Sp2.clear();
		Ba.start();
		ud.start();
		start = b; // NEW
		b->traversal(Sp, flag_part);
		Sp2.insert(b);
		Ba->insert(b); // NEW
		for (auto i : b->link)
		{
			_tetron* a = (*i)(b);
			if (!i->test_flags(b, flag_part)) continue;
			anti_metka_B12(a);
		}
		// подготовить список
		Sp2.clear();
		Sp2.insert(b);
		for (auto i : b->link)
		{
			_tetron* a = (*i)(b);
			if (!i->test_flags(b, flag_part)) continue;
			metka_del_B12(a);
		}
	}

	void free_bank()
	{
		Ba.stop();
		Sp.stop();
		ud.stop();
	}
} // namespace SuperDelTetron2

_tetron* _tetron::copy_plus()
{
	super_del_tetron2::prepare_sp_unik_sv(this);
	super_del_tetron2::ud->push_back(this); // т.к. в списке главного тетрона нет
	                                      // Sp2 = ud ???
	for (auto b : *super_del_tetron2::ud)
	{
		_tetron* a = create_tetron(b->type());
		a->copy(b);
		super_del_tetron2::Sp2.find(b)->a = a;
	}
	for (auto a : *super_del_tetron2::ud)
	{
		_tetron* aa = super_del_tetron2::Sp2.find(a)->a;
		for (auto j : a->link)
		{
			_tetron* b = (*j)(a);
			auto     n = super_del_tetron2::Sp2.find(b);
			if (!n)
			{
				aa->add_flags(b, j->get_flags(a) & 0xFFFFFFFF, false);
				continue;
			}
			if (aa <= n->a) aa->add_flags(n->a, j->get_flags(a), false);
		}
	}
	_tetron* rr = super_del_tetron2::Sp2.find(this)->a;
	super_del_tetron2::free_bank();
	return rr;
}

u64 _tetron::get_flags(_tetron* t)
{
	_link& li = *glink.find(_pair_tetron(this, t));
	return (li.low_tetron) ? li.get_flags(this) : 0;
}

void _tetron::set2_flags(_tetron* t, u64 flags, func_fl trans, bool after)
{
	if (!t) return;
	auto   err = glink.find(_pair_tetron(this, t));
	_link& li  = *err;
	u64 fl  = (li.low_tetron) ? li.get_flags(this) : 0;
	trans(fl, flags);
	if (li.low_tetron)
	{
		li.set_flags(this, fl);
		return;
	}
	if (fl == 0) return;
	err.life();

	if ((u64)this <= (u64)t)
	{
		li.low_tetron  = this;
		li.high_tetron = t;
		li.n_low       = (uint)link.size();
		li.n_high      = (uint)t->link.size();
	}
	else
	{
		li.low_tetron  = t;
		li.high_tetron = this;
		li.n_low       = (uint)t->link.size();
		li.n_high      = (uint)link.size();
	}
	link.push_back(&li);
	if (this != t) t->link.push_back(&li);
	li.set_flags(this, fl);
	if (after)
	{
		after_create_link(&li);
		if (this != t) t->after_create_link(&li);
	}
}

void _tetron::run(_tetron* tt0, _tetron* tt, u64 flags)
{
	for (_frozen i(this, flag_parent); i; i++) i->run(tt0, tt, flags);
	for (_frozen i(this, flags); i; i++) i->run(tt, i, flags);
}

void _tetron::traversal(_hash_table_tetron* ht, u64 flags, _vector_tetron* lt)
{
	if (ht->insert(this) == false) return;
	if (lt) lt->push_back(this);
	for (auto i : link)
	{
		_tetron* a = (*i)(this);
		if (!i->test_flags(this, flags)) continue;
		a->traversal(ht, flags, lt);
	}
}

_tetron::~_tetron()
{
	for (int i = (int)link.size() - 1; i >= 0; i--) link[i]->~_link();
	all_tetron.erase(id);
}

void _tetron::copy(_tetron* a)
{
	static _stack stack;
	stack.clear();
	a->push(&stack);
	pop(&stack);
}

void _tetron::add_unique_flags(_tetron* t, u64 flags, bool after)
{
	for (int i = (int)link.size() - 1; i >= 0; i--) // должна быть только одна!
	{
		if (!link[i]->test_flags(this, flags)) continue;
		link[i]->set_flags(this, link[i]->get_flags(this) & (~flags));
	}
	if (t != nullptr) add_flags(t, flags);
}

void optimize_hash_intermediate()
{ // проверить подбор k, чтобы удалялось ~ 50%
	if (hash_intermediate.size < 100000) return; // примерно 10МБ
	uint size_old = hash_intermediate.size;
	static u64 old_number = 0;
	static double k = 0.5;
	u64 n_gr = (u64)(old_number + (number_intermediate - old_number) * k);
	for (auto& i : hash_intermediate)
	{
		if ((i.tetron_before->get_flags(i.tetron_intermediate) == 0) ||
		    (i.tetron_intermediate->get_flags(i.tetron_after) == 0) || (i.number <= n_gr))
			hash_intermediate.erase(i);
	}
	old_number = n_gr;
	double k2 = k / ((size_old * 0.5) / hash_intermediate.size);
	k = k * 0.9 + k2 * 0.1;
}

void delete_hvost(_tetron* t, bool del_t, bool run_func)
{
	bool pprr           = run_before_del_link;
	run_before_del_link = run_func;
	super_del_tetron2::prepare_sp_unik_sv(t);
	for (auto i : *super_del_tetron2::ud) delete i;
	super_del_tetron2::free_bank();
	if (del_t) delete t;
	run_before_del_link = pprr;
}

void _link::operator=(_link&& a) noexcept
{
	if (low_tetron) this->~_link();
	low_tetron                = a.low_tetron;
	high_tetron               = a.high_tetron;
	n_low                     = a.n_low;
	n_high                    = a.n_high;
	flags                     = a.flags;
	low_tetron->link[n_low]   = this;
	high_tetron->link[n_high] = this;
	a.low_tetron              = nullptr;
}

_link::~_link()
{
	if (!low_tetron) return;
	if (run_before_del_link)
	{
		low_tetron->before_delete_link(this);
		if (low_tetron != high_tetron) high_tetron->before_delete_link(this);
	}
	if (n_low + 1ULL < low_tetron->link.size())
	{
		_link* li = low_tetron->link[n_low] = low_tetron->link.back();
		((li->low_tetron == low_tetron) ? li->n_low : li->n_high) = n_low;
	}
	low_tetron->link.pop_back();
	if (low_tetron != high_tetron)
	{
		if (n_high + 1ULL < high_tetron->link.size())
		{
			_link* li = high_tetron->link[n_high] = high_tetron->link.back();
			((li->low_tetron == high_tetron) ? li->n_low : li->n_high) = n_high;
		}
		high_tetron->link.pop_back();
	}
	low_tetron = nullptr;
	glink.erase(this);
}

void _frozen::operator++(int)
{
	for (i++; i < lt->size(); i++)
	{
		_tetron* t2 = (*lt)[i];
		if (!t2) continue;
		if (!tetron->test_flags(t2, flags)) continue;
		tetron2 = t2;
		return;
	}
	tetron2 = nullptr;
}

_frozen::_frozen(_tetron* t, u64 flags_) : tetron(t), i(0), tetron2(nullptr), flags(flags_)
{
	for (auto j : t->link)
	{
		if (!j->test_flags(t, flags)) continue;
		if (!tetron2) tetron2 = (*j)(t);
		lt->push_back((*j)(t)); // проверить на move
	}
}

void add_obl_izm(_area a)
{
	if (a.empty()) return;
	if ((a.x.max < 0) || (a.x.min > master_bm.size.x) || (a.y.max < 0) || (a.y.min > master_bm.size.y)) return;
	master_obl_izm |= a;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

_trans    master_trans_go;      // трансформация тяни-толкай объекта, или объекта под мышкой
_chain_go master_chain_go;      // активная цепочка графических объектов
bool      time_ris = false;     // отображать время рисования
_trans    master_trans_go_move; // трансформация n_go_move
_xy       par_koo1(0, 0);       // .....вспомогательная переменная  !!!ИЗБАВИТЬСЯ!!!

void smena_avt();

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

_area _t_basic_go::calc_area()
{ // рекурсия невозможна
	if (area_definite) return area;
	area_definite = true;
	if (_t_go* tgo = *this)
		area = tgo->local_area;
	else
		area = _area();
	for (auto i : link)
	{
		_tetron* a = (*i)(this);
		if (!(i->get_flags(this) & (flag_parent + flag_sub_go))) continue;
		_t_basic_go* b = *a; if (b == nullptr) continue;
		b->calc_area();
		if (i->test_flags(this, flag_parent))
			area |= b->area;
		else
		{
			if (_t_trans* ttr = *b)
				area |= ttr->trans(b->area);
			else
				area |= b->area;
		}
	}
	return area;
}

void _t_basic_go::add_area(_area a, bool first)
{
	static _speed<_hash_table_tetron> hash(false);
	bool start = (hash.a == nullptr);
	if (start) hash.start();
	_t_go* tgo = *this;
	_t_trans* ttr = *this;
	if (first)
		a = calc_area();
	else
	{
		if (!(a <= area)) area_definite = false;
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
		_tetron* t = (*i)(this);
		_t_basic_go* b = *t; if (b == nullptr) continue;
		if (hash->find(t)) continue; // рекурсия не допускается
		if (i->test_flags(t, flag_sub_go)) b->add_area((tgo) ? a : ttr->trans(a), false); // можно ли
		if (i->test_flags(t, flag_parent)) b->add_area(a, false); // И то И то?
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

void _t_basic_go::cha_area(_area a, bool first)
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
	if (first)
		if (tgo) a = tgo->local_area;
	hash->insert(this);
	for (auto i : link)
	{
		_tetron* t = (*i)(this);
		_t_basic_go* b = *t; if (b == nullptr) continue;
		if (hash->find(t)) continue; // рекурсия не допускается
		if (i->test_flags(t, flag_sub_go)) b->cha_area((tgo) ? a : ttr->trans(a), false); // можно ли
		if (i->test_flags(t, flag_parent)) b->cha_area(a, false); // И то И то?
	}
	hash->erase(this);
	if (start) hash.stop();
}

void _t_basic_go::del_area(_area a, bool first)
{
	static _speed<_hash_table_tetron> hash(false);
	bool start = (hash.a == nullptr);
	if (start) hash.start();
	_t_go* tgo = *this;
	_t_trans* ttr = *this;
	if (first)
		a = calc_area();
	else
	{
		if (!(a < area)) area_definite = false;
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
		_tetron* t = (*i)(this);
		_t_basic_go* b = *t; if (b == nullptr) continue;
		if (hash->find(t)) continue; // рекурсия не допускается
		if (i->test_flags(t, flag_sub_go)) b->del_area((tgo) ? a : ttr->trans(a), false); // можно ли
		if (i->test_flags(t, flag_parent)) b->del_area(a, false); // И то И то?
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
			_tetron* aa = (*i)(b);
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
		if ((*li)(this)->operator _t_basic_go * ()) add_area();
}

void _t_basic_go::before_delete_link(_link* li)
{
	_tetron::before_delete_link(li);
	if (li->test_flags(this, inverted_flags(flag_sub_go)))
		if ((*li)(this)->operator _t_basic_go * ()) del_area();
}

void _t_basic_go::set_layer(double n)
{
	_id tt = master_layers[n];
	_t_double* t;
	if (!tt)
	{
		t = new _t_double;
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
		_tetron* a = (*link[i])(this);
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
			master_trans_go = tr; // было в if
			if (tgo->mouse_down_left2(r))
			{
				n_tani = this;
//				master_trans_go = tr;
				return true;
			}
			if (tgo->key_fokus) return true;
		}
	for (int i = (int)link.size() - 1; i >= 0; i--)
	{
		_tetron* a = (*link[i])(this);
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
		_tetron* a = (*link[i])(this);
		if (!link[i]->test_flags(this, flag_sub_go)) continue;
		if (_t_basic_go* aa = *a)
		{
			if (n_ramk == aa) continue;//?только здесь
			if (n_ramk2 == aa) continue;
			_xy r2(0, 0);
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
		_tetron* a = (*link[i])(this);
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
		_tetron* a = (*link[i])(this);
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
	if (tr(calc_area()).min_length() * 0.5 < final_radius()) return true;
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
	trr->area_definite = false;
	trr->add_area();
}

_t_trans* _t_basic_go::set_t_trans(_tetron* go, u64 flags)
{
	_t_trans* ttr = nullptr;
	if (link.size() <= go->link.size())
	{
		for (auto i : link)
			if (_t_trans* b = *(*i)(this))
				if (b->get_flags(go))
				{
					ttr = b;
					break;
				}
	}
	else
	{
		for (auto i : go->link)
			if (_t_trans* b = *(*i)(go))
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
	if (((tr(calc_area())) & master_obl_izm).empty()) return;
	if (!final) final = final_fractal(tr);
	master_chain_go.push(this, tr);
	for (_frozen a(this, flag_parent); a; a++)
	{
		if (_t_go* b = *a)
			b->ris(tr, final);
		else
			if (_t_trans* bb = *a)
				bb->ris(tr / bb->trans, final);
	}
	for (_layers_go a(this); a; a++) a->ris(tr, final);
	master_chain_go.pop();
}

void _t_trans::clear_go_rod()
{
	for (int i = (int)link.size() - 1; i >= 0; i--)
	{
		_tetron* a = (*link[i])(this);
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
		_tetron* a = (*link[i])(this);
		if (!link[i]->test_flags(this, flag_sub_go)) continue;
		if (_t_basic_go* aa = *a)
		{
			_trans tr2;
			if (_t_go* tgo = *a)
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
		_tetron* a = (*link[i])(this);
		if (!link[i]->test_flags(this, flag_parent)) continue;
		if (_t_basic_go* aa = *a)
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
	if (a == nullptr) return cc1;
	if (i64* c = *a) return (uint)*c;
	return cc1;
}

uint _t_go::get_c2()
{
	_tetron* a = find_intermediate<_tetron>(n_color_bg, flag_information, flag_parent);
	if (a == nullptr) return c2_default;
	if (i64* c = *a) return (uint)*c;
	return c2_default;
}

void _t_go::set_c(uint c)
{
	_tetron* a = find_intermediate<_tetron>(n_color_line, flag_information, flag_parent);
	if (a != nullptr)
	{
		if (i64* cc = *a)*cc = c;
		return;
	}
	if (c == cc1) return;
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
		if (i64* cc = *a)*cc = c;
		return;
	}
	if (c == c2_default) return;
	_t_int* ti = new _t_int;
	ti->a = c;
	ti->add_flags(n_color_bg, flag_parent);
	add_flags(ti, flag_information | flag_part);
}

_t_go::_t_go() : _t_basic_go(), key_fokus(false)
{
}

void _t_go::ris(_trans tr, bool final)
{
	if (((tr(calc_area())) & master_obl_izm).empty()) return;
	if (!final) final = final_fractal(tr);
	master_chain_go.push(this, tr);
	for (_frozen a(this, flag_parent); a; a++)
	{
		if (_t_go* b = *a)
			b->ris(tr, final);
		else
			if (_t_trans* bb = *a)
				bb->ris(tr / bb->trans, final);
	}
	auto ris_all = [&]() {
		master_bm.set_drawing_area(tr(local_area) & master_obl_izm);
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
		_xy oo = tr(local_area.top_left());
		if (oo.x < 0) oo.x = 0;
		if (oo.y < 0) oo.y = 0;
		std::wstring s = double_to_wstring(t.count() / 1000000.0, 2);
		master_bm.text({ (i64)(oo.x + 1), (i64)oo.y }, s.c_str(), 13, cc2, 0x00000000);
	}
}

_t_trans* _t_go::ttrans()
{
	for (auto i : link)
	{
		_tetron* a = (*i)(this);
		if (!i->test_flags(a, flag_sub_go)) continue;
		if (_t_trans* ttg = *a) return ttg;
	}
	return nullptr;
}

void _t_go::clear_go_rod()
{
	for (int i = (int)link.size() - 1; i >= 0; i--)
	{
		_t_trans* tr = *(*link[i])(this);
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
			_tetron* a = (*link[i])(this);
			if (!link[i]->test_flags(this, flag_sub_go)) continue;
			if (_t_basic_go* aa = *a)
			{
				_trans tr2;
				if (_t_go* tgo = *a)
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
						if (n_ramk2 == xx) set_cursor(_cursor::normal); else xx->mouse_finish_move();
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
		_tetron* a = (*link[i])(this);
		if (!link[i]->test_flags(this, flag_parent)) continue;
		if (_t_basic_go* aa = *a)
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
		_tetron* tt = (*j)(t);
		if (!j->test_flags(t, flag_sub_go)) continue;
		if (tt->operator _t_basic_go * () == nullptr) continue;
		_t_double* ti = tt->find_intermediate<_t_double>(n_go_layer, inverted_flags(flag_information), flag_parent);
		double n = (ti) ? ti->a : 0;
		map_.insert({ n, tt });
	}
	i_m = map_.begin();
	if (i_m != map_.end()) tetron2 = *i_m->second;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void _g_circle::push(_wjson& b)
{
	_t_go::push(b);
	b.add("center", center);
	b.add("radius", radius);
	b.add("width", width);
}

void _g_circle::pop(_rjson& b)
{
	_t_go::pop(b);
	b.read("center", center);
	b.read("radius", radius);
	b.read("width", width);
}

void _g_circle::run(_tetron* tt0, _tetron* tt, u64 flags)
{
	_t_xy* a = find_intermediate<_t_xy>(n_center, flag_information, flag_parent);
	if (a) center = a->a;
	_t_double* aa = find_intermediate<_t_double>(n_radius, flag_information, flag_parent);
	if (aa) radius = aa->a;
	aa = find_intermediate<_t_double>(n_width, flag_information, flag_parent);
	if (aa) width = aa->a;
	del_area();
	calc_local_area();
	area_definite = false;
	add_area();
	_t_go::run(tt0, tt, flags);
}

void _g_circle::calc_local_area()
{
	local_area = { {center.x - radius, center.x + radius}, {center.y - radius, center.y + radius} };
}

void _g_circle::ris2(_trans tr, bool final)
{
	master_bm.fill_ring(tr(center), tr(radius), tr(width), get_c(), get_c2());
}

bool _g_circle::test_local_area(_xy b)
{
	if (!local_area.test(b)) return false;
	return ((b - center).len2() <= radius * radius);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void _g_froglif::ris2(_trans tr, bool final)
{
	_area a = tr(local_area);
	master_bm.froglif(a.top_left(), a.min_length(), f, r_f, get_c(), get_c2());
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void _g_line::run(_tetron* tt0, _tetron* tt, u64 flags)
{
	_t_xy* a = find_intermediate<_t_xy>(n_begin, flag_information, flag_parent);
	if (a) p1 = a->a;
	a = find_intermediate<_t_xy>(n_end, flag_information, flag_parent);
	if (a) p2 = a->a;
	_t_double* aa = find_intermediate<_t_double>(n_width, flag_information, flag_parent);
	if (aa) width = aa->a;
	del_area();
	calc_local_area();
	area_definite = false;
	add_area();
	_t_go::run(tt0, tt, flags);
}

void _g_line::calc_local_area()
{
	local_area = (_area(p1) | p2).expansion(width * 0.5);
}

void _g_line::ris2(_trans tr, bool final)
{
	master_bm.lines(tr(p1), tr(p2), tr(width), get_c());
}

bool _g_line::test_local_area(_xy b)
{
	if (!local_area.test(b)) return false;
	_xy n = { p1.y - p2.y,p2.x - p1.x };
	n *= 1.0 / n.len();
	_xy p11 = p1 + n * (width * 0.75);
	_xy p12 = p1 - n * (width * 0.75);
	_xy p21 = p2 + n * (width * 0.75);
	_xy p22 = p2 - n * (width * 0.75);
	if (((test_line(p11, p12, b) + test_line(p21, p22, b) + test_line(p11, p21, b) + test_line(p12, p22, b)) & 1) == 0) return false;
	return true;
}

void _g_line::push(_wjson& b)
{
	_t_go::push(b);
	b.add("p1", p1);
	b.add("p2", p2);
	b.add("width", width);
}

void _g_line::pop(_rjson& b)
{
	_t_go::pop(b);
	b.read("p1", p1);
	b.read("p2", p2);
	b.read("width", width);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void _g_picture::ris2(_trans tr, bool final)
{
	if (pic.size.x * pic.size.y == 0)
	{
		int rr = 100;
		master_bm.line({ tr.offset.x, tr.offset.y }, { tr.offset.x + rr * tr.scale, tr.offset.y }, cc1);
		master_bm.line({ tr.offset.x + rr * tr.scale, tr.offset.y },
			{ tr.offset.x + rr * tr.scale, tr.offset.y + rr * tr.scale }, cc1);
		master_bm.line({ tr.offset.x + rr * tr.scale, tr.offset.y + rr * tr.scale },
			{ tr.offset.x, tr.offset.y + rr * tr.scale }, cc1);
		master_bm.line({ tr.offset.x, tr.offset.y + rr * tr.scale }, { tr.offset.x, tr.offset.y }, cc1);
		master_bm.line({ tr.offset.x, tr.offset.y },
			{ tr.offset.x + rr * tr.scale, tr.offset.y + rr * tr.scale }, cc1);
		master_bm.line({ tr.offset.x + rr * tr.scale, tr.offset.y },
			{ tr.offset.x, tr.offset.y + rr * tr.scale }, cc1);
		return;
	}
	_area oo = tr(local_area);
	master_bm.stretch_draw(&pic, (int)oo.x.min, (int)oo.y.min, tr.scale);
}

void _g_picture::new_size(_isize ns)
{
	if (!pic.resize(ns)) return;
	del_area();
	local_area = pic.size;
	area_definite = false;
	add_area();
}

void _g_picture::set_pic(const _picture& pic2)
{
	(_picture)pic = pic2;
	del_area();
	local_area = pic.size;
	area_definite = false;
	add_area();
}

bool _g_picture::load_from_file(wstr fn)
{
	/*	del_area();
		_stack fs;
		fs.load_from_file(fn);
		_bitmap_file bf;
		fs >> bf;
		if (!bf) return false;
		pic = std::move(bf);
		local_area = _area_old(0, (double)pic.size.x, 0, (double)pic.size.y);
		area_definite = false;
		add_area();
		return true;*/
	return true;
}

bool _g_picture::save_to_file(wstr fn) const
{
	/*	_stack fs((size_t)pic.size.x * pic.size.y * sizeof(_color));
		fs << _bitmap_file(pic);
		return fs.save_to_file(fn);*/
	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void _g_rect::ris2(_trans tr, bool final)
{
	_area oo = tr(local_area);
	uint c2 = get_c2();
	master_bm.fill_rectangle({ {(i64)oo.x.min, (i64)oo.x.max + 1}, {(i64)oo.y.min, (i64)oo.y.max + 1} }, c2);
	uint c0 = get_c();
	if (((c0 >> 24) != 0x00) && (c0 != c2)) master_bm.rectangle(oo, c0);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void _g_text::value_changed()
{
	del_area();
	_isize size = master_bm.size_text(s, 13);
	local_area = { {-1, std::max((double)size.x, 13.0)}, {0, std::max((double)size.y, 13.0)} };
	area_definite = false;
	add_area();
}

void _g_text::ris2(_trans tr, bool final)
{
	int sf = (int)(13 * tr.scale + 0.5);
	if (sf < 5) return;
	master_bm.text(tr.offset, s.c_str(), sf, get_c(), get_c2());
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void del_hint()
{
	if (!n_hint) return;
	delete_hvost(n_hint->operator _t_basic_go * ()->ttrans(), true);
}

void add_hint(std::wstring_view hint, _t_go* g)
{
	del_hint();
	if (hint.empty()) return;
	_t_trans* ko = *n_ko;
	_trans tr = master_trans_go;
	_isize siz = master_bm.size_text(hint, 13);
	tr.offset += _xy{ -siz.x * 0.5, -15.0 } + _xy{ 0.5 * g->local_area.x, g->local_area.y.min } *tr.scale;
	tr.scale = 1;
	_g_text* go = new _g_text;
	go->set_c(cc3);
	go->set_c2(cc0);
	_t_trans* ttr = new _t_trans;
	ttr->trans = ko->trans.inverse() * tr;
	ttr->add_flags(go, flag_part + flag_sub_go);
	go->s = hint;
	go->value_changed();
	ko->add_flags(ttr, flag_part + flag_sub_go);
	n_hint = go;
}

void change_hint(std::wstring_view hint)
{
	if (!n_hint) return;
	_g_text* go = *n_hint;
	go->s = hint;
	go->value_changed();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void _g_test_graph::ris2(_trans tr, bool final)
{
	static bool first = true;
	if (first)
	{
		_picture te({ 120, 90 });
		te.clear();
		for (int i = 0; i < 100; i++)
			te.line({ rnd(120), rnd(90) }, { rnd(120), rnd(90) }, 0xFF2080FF);
		first = false;
		a.clear(0xFF0000FF);
		_iarea ogr({ 30LL, 225LL }, { 25LL, 125LL });
		a.set_drawing_area(ogr);
		a.stretch_draw(&te, 0, 0, 3.3);
		a.rectangle(ogr, 0x80FF0000);
	}
	_area oo = tr(local_area);
	master_bm.stretch_draw(&a, (int)(oo.x.min + 20), (int)(oo.y.min + 20), 1);
}

_g_test_graph::_g_test_graph() : a({ 250, 150 })
{
	local_area = { {0, 300}, {0, 200} };
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void _g_scrollbar::push(_wjson& b)
{
	_t_go::push(b);
	b.add("position", position);
	b.add("vid", vid);
}

void _g_scrollbar::pop(_rjson& b)
{
	_t_go::pop(b);
	b.read("position", position);
	b.read("vid", vid);
}

void _g_scrollbar::prilip(_t_go* r)
{
	if (!r) return;
	del_area();
	double l = ((vid & 1) == 1) ? local_area.x.length() : local_area.y.length();
	_area& o = r->local_area;
	if (vid == 2) local_area = { {o.x.min, o.x.max}, {o.y.max, o.y.max + l} };
	if (vid == 3) local_area = { {o.x.max, o.x.max + l}, {o.y.min, o.y.max} };
	if (vid == 4) local_area = { {o.x.min, o.x.max}, {o.y.min - l, o.y.min} };
	if (vid == 5) local_area = { {o.x.min - l, o.x.min}, {o.y.min, o.y.max} };
	//	if (vid > 1) trans = _trans(); //глобальная замена trans
	area_definite = false;
	add_area();
}

void _g_scrollbar::after_create_link(_link* li)
{
	if (vid > 1)
		if (li->test_flags(this, inverted_flags(flag_sub_go)))
		{
			_t_go* r = *(*li)(this);
			prilip(r);
		}
	_t_go::after_create_link(li);
}

void _g_scrollbar::ris2(_trans tr, bool final)
{
	_area a = tr(local_area);
	uint c = cc1;
	if ((vid & 1) == 0)
	{
		master_bm.line({ a.x.min, 0.5 * a.y }, { a.x.max, 0.5 * a.y }, c);
		master_bm.line({ position * a.x, a.y.min }, { position * a.x, a.y.max }, c);
	}
	else
	{
		master_bm.line({ 0.5 * a.x, a.y.min }, { 0.5 * a.x, a.y.max }, c);
		master_bm.line({ a.x.min, position * a.y }, { a.x.max, position * a.y }, c);
	}
}

void _g_scrollbar::mouse_move_left2(_xy r)
{
	double ii;
	if ((vid & 1) == 0)
		ii = (r.x - local_area.x.min) / local_area.x.length();
	else
		ii = (r.y - local_area.y.min) / local_area.y.length();
	if (ii < 0) ii = 0;
	if (ii > 1) ii = 1;
	if (ii != position)
	{
		position = ii;
		cha_area();
		run(0, this, flag_run);
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void _g_edit_double::ris2(_trans tr, bool final)
{
	_g_rect::ris2(tr, final);
	_area oo = tr(local_area);
	uint c0 = get_c();
	int sf2 = (int)(13 * tr.scale + 0.5);
	if (sf2 < 5) return;
	std::wstring s = double_to_wstring(a, 2);

	master_bm.text({ (i64)(oo.x.min + 5), (i64)(oo.y.min + 1) }, s.c_str(), sf2, c0, cc0);
	if (n_act_key == this)
	{
		_isize size = master_bm.size_text(s.substr(/*first_+*/ 0, cursor /*-first_*/).c_str(), sf2);
		master_bm.line({ oo.x.min + 4 + size.x, oo.y.min + 1 }, { oo.x.min + 4 + size.x, oo.y.min + sf2 }, c0);
	}
}

void _g_edit_double::key_down(ushort key)
{
	if (key == 37) // left
	{
		if (cursor > 0)
		{
			cursor--;
			cha_area();
		}
		return;
	}
	std::wstring s = double_to_wstring(a, 2);
	if (key == 8) // backspace
	{
		if ((cursor > 0) && (cursor < (int)s.size() - 2))
		{
			i64 b = (i64)a;
			i64 c = 1;
			for (int j = int(s.size() - 3); j > cursor; j--) c *= 10;
			a = (b / (c * 10)) * c + (b % c) + 0.1 * (s[s.size() - 2] - L'0') + 0.01 * (s[s.size() - 1] - L'0');
			cursor--;
			cha_area();
		}
		return;
	}
	if (key == 39) // right
	{
		if (cursor < (int)s.size())
		{
			cursor++;
			cha_area();
		}
		return;
	}
	if (key == 46) // delete
	{
		if (cursor < (int)s.size() - 3)
		{
			i64 b = (i64)a;
			i64 c = 1;
			for (int j = int(s.size() - 4); j > cursor; j--) c *= 10;
			a = (b / (c * 10)) * c + (b % c) + 0.1 * (s[s.size() - 2] - L'0') + 0.01 * (s[s.size() - 1] - L'0');
			cha_area();
		}
		return;
	}
}

void _g_edit_double::key_press(ushort key)
{
	if ((key < L'0') || (key > L'9')) return;
	cha_area();
	std::wstring s = double_to_wstring(a, 2);
	if (cursor == s.size()) return;
	i64 b = (i64)a;
	if (cursor == s.size() - 1)
		a = b + 0.1 * (s[s.size() - 2] - L'0') + 0.01 * (key - L'0');
	else
		if (cursor == s.size() - 2)
			a = b + 0.01 * (s[s.size() - 1] - L'0') + 0.1 * (key - L'0');
		else
		{
			if (b == 0)
			{
				a = (key - L'0') + 0.1 * (s[s.size() - 2] - L'0') + 0.01 * (s[s.size() - 1] - L'0');
				cursor = 0;
			}
			else
			{
				i64 c = 1;
				for (int j = int(s.size() - 3); j > cursor; j--) c *= 10;
				a = (b / c) * (c * 10) + (b % c) + (key - L'0') * c + 0.1 * (s[s.size() - 2] - L'0') + 0.01 *
					(s[s.size() - 1] - L'0');
			}
		}
	cursor++;
}

_g_edit_double::_g_edit_double() : cursor(0), a(0.0)
{
	local_area = { {0, 100}, {0, 16} };
	key_fokus = true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void _g_edit_int::ris2(_trans tr, bool final)
{
	_area oo = tr(local_area);
	uint c0 = get_c();
	master_bm.rectangle(oo, c0);
	int sf2 = (int)(13 * tr.scale + 0.5);
	if (sf2 < 5) return;
	i64* a = find1_plus_gtetron<i64>(this, flag_specialty);
	if (!a)
	{
		master_bm.line(oo.top_left(), oo.bottom_right(), c0);
		master_bm.line(oo.bottom_left(), oo.top_right(), c0);
		return;
	}
	bool hex = test_flags(n_hex, flag_information);
	std::wstring s = (hex) ? std::wstring(uint64_to_wstr_hex(*a)) : std::to_wstring(*a);
	master_bm.text({ (i64)(oo.x.min + 5), (i64)(oo.y.min + 1) }, s.c_str(), sf2, c0, cc0);
	if (n_act_key == this)
	{
		_isize size = master_bm.size_text(s.substr(/*first_+*/ 0, cursor /*-first_*/).c_str(), sf2);
		master_bm.line({ oo.x.min + 4 + size.x, oo.y.min + 1 }, { oo.x.min + 4 + size.x, oo.y.min + sf2 }, c0);
	}
}

void _g_edit_int::key_down(ushort key)
{
	i64* a = find1_plus_gtetron<i64>(this, flag_specialty);
	if (!a) return;
	bool hex = test_flags(n_hex, flag_information);

	if (key == 8) // backspace
	{
		if (cursor > 0)
		{
			if (hex)
			{
				*a /= 16;
			}
			else
			{
				int l = (int)std::to_wstring(*a).size();
				if (cursor > l) cursor = l;
				i64 mn = 1;
				for (int i = cursor; i < l; i++) mn *= 10;
				*a = (*a / (mn * 10)) * mn + (*a % mn);
			}
			cursor--;
			cha_area();
		}
		return;
	}
	if (key == 37) // left
	{
		if (cursor > 0)
		{
			cursor--;
			cha_area();
		}
		return;
	}
	int l = (int)((hex) ? std::wstring(uint64_to_wstr_hex(*a)) : std::to_wstring(*a)).size();
	if (key == 39) // right
	{
		if (cursor < l)
		{
			cursor++;
			cha_area();
		}
		return;
	}
	if (key == 46) // delete
	{
		if (cursor < l)
		{
			if (hex)
			{
				*a /= 16;
			}
			else
			{
				if (cursor > l) cursor = l;
				i64 mn = 1;
				for (int i = cursor; i < l; i++) mn *= 10;
				*a = (*a / mn) * (mn / 10) + (*a % (mn / 10));
			}
			cha_area();
		}
		return;
	}
}

bool _g_edit_int::mouse_wheel2(_xy r)
{
	i64* a = find1_plus_gtetron<i64>(this, flag_specialty);
	if (!a) return true;
	*a += *n_wheel->operator i64 * ();
	cha_area();
	return true;
}

void _g_edit_int::key_press(ushort key)
{
	i64* a = find1_plus_gtetron<i64>(this, flag_specialty);
	if (!a) return;
	if (test_flags(n_hex, flag_information))
	{
		if ((key >= L'0') && (key <= L'9'))*a = *a * 16 + (key - L'0');
		if ((key >= L'a') && (key <= L'f'))*a = *a * 16 + (key - L'a') + 10;
		if ((key >= L'A') && (key <= L'F'))*a = *a * 16 + (key - L'A') + 10;
	}
	else
		if ((key >= L'0') && (key <= L'9'))
		{
			if (*a == 0) cursor = 1; // особый случай
			int l = (int)std::to_wstring(*a).size();
			if (cursor > l) cursor = l;
			i64 mn = 1;
			for (int i = cursor; i < l; i++) mn *= 10;
			*a = (*a / mn) * (mn * 10) + (key - L'0') * mn + (*a % mn);
			l = (int)std::to_wstring(*a).size();
			if (cursor < l) cursor++;
		}
	cha_area();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void _g_edit_string::ris2(_trans tr, bool final)
{
	_area oo = tr(local_area);
	uint c0 = get_c();
	master_bm.rectangle(oo, c0);
	std::wstring* s = find1_plus_gtetron<std::wstring>(this, flag_specialty);
	if (!s)
	{
		master_bm.line(oo.top_left(), oo.bottom_right(), c0);
		master_bm.line(oo.bottom_left(), oo.top_right(), c0);
		return;
	}
	int sf = (int)(13 * tr.scale + 0.5);
	if (sf < 1) return;
	int l = (int)s->size();
	if (cursor < 0) cursor = 0;
	if (cursor > l) cursor = l;
	if (first > cursor) first = cursor;
	if (first + len2 > l) len2 = l - first;
	if (first + len2 < cursor) first = cursor - len2;
	int rx_text = (int)(oo.x.length() - 8);
	if ((l > 0) && (rx_text > 3))
	{
		_isize size = master_bm.size_text(s->substr(first, len2).c_str(), sf);
		if (size.x > rx_text)
		{
			while (first + len2 > cursor)
			{
				len2--;
				size = master_bm.size_text(s->substr(first, len2).c_str(), sf);
				if (size.x <= rx_text) break;
			}
			if (size.x > rx_text)
				while (first < cursor)
				{
					len2--;
					first++;
					size = master_bm.size_text(s->substr(first, len2).c_str(), sf);
					if (size.x <= rx_text) break;
				}
		}
		else if (size.x < rx_text)
		{
			while (first + len2 < l)
			{
				size = master_bm.size_text(s->substr(first, len2 + 1LL).c_str(), sf);
				if (size.x > rx_text) break;
				len2++;
			}
			if (size.x < rx_text)
				while (first > 0)
				{
					size = master_bm.size_text(s->substr(first - 1LL, len2 + 1LL).c_str(), sf);
					if (size.x > rx_text) break;
					len2++;
					first--;
				}
		}
		master_bm.text({ (i64)(oo.x.min + 5), (i64)oo.y.min }, s->substr(first, len2).c_str(), sf, c0, 0);
		if (first > 0)        master_bm.line({ oo.x.min + 2, oo.y.min }, { oo.x.min + 2, oo.y.max }, 0xFF30C0F0);
		if (len2 < l - first) master_bm.line({ oo.x.max - 2, oo.y.min }, { oo.x.max - 2, oo.y.max }, 0xFF30C0F0);
	}
	if (n_act_key == this)
	{
		_isize size = master_bm.size_text(s->substr(first, (i64)cursor - first).c_str(), sf);
		master_bm.line({ oo.x.min + 4 + size.x, oo.y.min + 1 }, { oo.x.min + 4 + size.x, oo.y.min + sf }, c0);
	}
}

void _g_edit_string::key_down(ushort key)
{
	_tetron* s_t;
	std::wstring* s = find1_plus_gtetron<std::wstring>(this, flag_specialty, &s_t);
	if (!s) return;
	if (key == 8) // backspace
	{
		if (cursor > 0)
		{
			s->erase(cursor - 1LL, 1);
			cursor--;
			cha_area();
			s_t->value_changed();
		}
		return;
	}
	if (key == 37) // left
	{
		if (cursor > 0)
		{
			cursor--;
			cha_area();
		}
		return;
	}
	int l = (int)s->size();
	if (key == 39) // right
	{
		if (cursor < l)
		{
			cursor++;
			cha_area();
		}
		return;
	}
	if (key == 46) // delete
	{
		if (cursor < l)
		{
			s->erase(cursor, 1);
			cha_area();
			s_t->value_changed();
		}
		return;
	}
}

void _g_edit_string::key_press(ushort key)
{
	if (key < 32) return;
	_tetron* s_t;
	std::wstring* s = find1_plus_gtetron<std::wstring>(this, flag_specialty, &s_t);
	if (!s) return;
	s->insert(cursor, 1, key);
	cursor++;
	cha_area();
	s_t->value_changed();
}

_g_edit_string::_g_edit_string()
{
	key_fokus = true;
	local_area = { {0, 100}, {0, 16} };
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void _g_edit_multi_string::ris2(_trans tr, bool final)
{
	_area oo = tr(local_area);
	if ((oo.x.length() <= 3) || (oo.y.length() <= 3)) return;
	_multi_string* str = find1_plus_gtetron<_multi_string>(this, flag_specialty);
	if (!str) str = &strings;
	master_bm.rectangle((_area)oo, cc1);
	if (str != &strings)
	{
		_area ooo = oo;
		ooo = ooo.expansion(-1.0);
		master_bm.rectangle((_area)ooo, cc1);
	}
	len2 = (int)((local_area.y.length() - 8) / 16);
	if (len2 == 0) return;
	int sf = (int)(13 * tr.scale + 0.5);
	if (sf < 5) return;
	if (_g_scrollbar* polz = find1<_g_scrollbar>(flag_part))
	{
		int k = (int)str->line.size() - len2;
		if (k < 0) k = 0;
		max_i = k;
		if (k >= 1)
		{
			first = (int)(polz->position * k + 0.5);
			if (cursor.y < first) cursor.y = first;
			if (cursor.y >= first + len2) cursor.y = first + len2 - 1;
		}
	}
	if (cursor.y >= (int)str->line.size()) cursor.y = (int)str->line.size() - 1;
	if (cursor.y < 0) cursor.y = 0;
	if (first > cursor.y) first = cursor.y;
	if (first + len2 > (int)str->line.size()) first = (i64)str->line.size() - len2;
	if (first < 0) first = 0;
	i64 k = len2;
	if ((int)str->line.size() < first + k) k = (i64)str->line.size() - first;
	for (i64 i = 0; i < k; i++)
	{
		i64 ii = first + i;
		master_bm.text({ (i64)(oo.x.min + 4 * tr.scale), (i64)(oo.y.min + (i * 16LL + 4) * tr.scale) },
			str->line[ii].c_str(), sf, cc1, 0);
	}
	if (first) master_bm.line({ oo.x.min + 2 * tr.scale, oo.y.min + 2 * tr.scale },
		{ oo.x.max - 2 * tr.scale, oo.y.min + 2 * tr.scale }, 0xFF30C0F0);
	if (first + k < (int)str->line.size()) master_bm.line({ oo.x.min + 2 * tr.scale, oo.y.max - 2 * tr.scale },
		{ oo.x.max - 2 * tr.scale, oo.y.max - 2 * tr.scale }, 0xFF30C0F0);

	if (n_act_key == this)
	{
		_isize size = master_bm.size_text(str->line[cursor.y].substr(0, cursor.x).c_str(), sf);
		master_bm.line({ oo.x.min + 4 * tr.scale + size.x - 1,
			oo.y.min + (((i64)cursor.y - first) * 16 + 4) * tr.scale },
			{ oo.x.min + 4 * tr.scale + size.x - 1,
			oo.y.min + ((cursor.y + 1LL - first) * 16 + 4) * tr.scale }, cc1);
	}
}

void _g_edit_multi_string::key_down(ushort key)
{
	_multi_string* str = find1_plus_gtetron<_multi_string>(this, flag_specialty);
	if (!str) str = &strings;
	if (str->line.size() == 0) return;
	if (cursor.y >= (int)str->line.size()) cursor.y = (int)str->line.size() - 1;

	if (key == 8) // backspace
	{
		int ll = 0;
		if (cursor.y) ll = (int)str->line[cursor.y - 1LL].size();
		if (str->delete_char(cursor.y, cursor.x - 1))
		{
			cursor.x--;
			if (cursor.x < 0)
			{
				cursor.x = ll;
				cursor.y--;
			}
			cha_area();
		}
		return;
	}
	if (key == 13) // enter
	{
		str->div2line(cursor.y, cursor.x);
		cursor.y++;
		cursor.x = 0;
		cha_area();
	}
	if (key == 37) // left
	{
		if (cursor.x > 0)
		{
			cursor.x--;
			cha_area();
		}
		else
			if (cursor.y > 0)
			{
				cursor.y--;
				cursor.x = (int)str->line[cursor.y].size();
				cha_area();
			}

		return;
	}
	if (key == 38) // up
	{
		if (cursor.y > 0)
		{
			cursor.y--;
			int l = (int)str->line[cursor.y].size();
			if (cursor.x > l) cursor.x = l;
			if (_g_scrollbar* polz = find1<_g_scrollbar>(flag_part))
			{
				i64 first_ = (i64)(polz->position * max_i + 0.5);
				if (first_ > cursor.y)
				{
					polz->position = (double)cursor.y / max_i;
					polz->cha_area();
				}
			}
		}
		else
			cursor.x = 0;
		cha_area();
		return;
	}
	int l = (int)str->line[cursor.y].size();
	if (key == 39) // right
	{
		if (cursor.x < l)
		{
			cursor.x++;
			cha_area();
		}
		else
			if (cursor.y < (int)str->line.size() - 1)
			{
				cursor.y++;
				cursor.x = 0;
				cha_area();
			}
		return;
	}
	if (key == 40) // down
	{
		if (cursor.y < (int)str->line.size() - 1)
		{
			cursor.y++;
			l = (int)str->line[cursor.y].size();
			if (cursor.x > l) cursor.x = l;
			if (_g_scrollbar* polz = find1<_g_scrollbar>(flag_part))
			{
				i64 first_ = (i64)(polz->position * max_i + 0.5);
				if (cursor.y >= first_ + len2)
				{
					polz->position = (double)(1LL + cursor.y - len2) / max_i;
					polz->cha_area();
				}
			}
		}
		else
			cursor.x = (int)str->line[cursor.y].size();
		cha_area();
		return;
	}
	if (key == 46) // delete
	{
		if (str->delete_char(cursor.y, cursor.x)) cha_area();
		return;
	}
}

void _g_edit_multi_string::key_press(ushort key)
{
	if (key < 32) return;
	_multi_string* str = find1_plus_gtetron<_multi_string>(this, flag_specialty);
	if (!str) str = &strings;
	str->insert_char(cursor.y, cursor.x++, key);
	cha_area();
}

_g_edit_multi_string::_g_edit_multi_string() : cursor{ 0LL, 0LL }
{
	local_area = { {0, 100}, {0, 100} };
	key_fokus = true;
}

void _g_edit_multi_string::run(_tetron* tt0, _tetron* tt, u64 flags)
{
	cha_area();
}

bool _g_edit_multi_string::mouse_down_left2(_xy r)
{
	if (!find1<_g_scrollbar>(flag_information))
	{
		_g_scrollbar* sb = new _g_scrollbar;
		sb->vid = 3;
		add_flags(sb, flag_sub_go + flag_part + flag_information + (flag_run << 32));
	}
	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool _g_edit64bit::mouse_down_left2(_xy r)
{
	if (act < 0) return true;
	a ^= (1LL << act);
	cha_area();
	return true;
}

void _g_edit64bit::mouse_finish_move()
{
	if (act < 0) return;
	act = -1;
	cha_area();
	del_hint();
}

bool _g_edit64bit::mouse_move2(_xy r)
{
	double d1 = local_area.x.length();
	double d2 = local_area.y.length();
	if ((d1 == 0) || (d2 == 0)) return true;
	int act2 = act;
	int x = (int)((r.x - local_area.x.min) * 8 / d1);
	int y = (int)((r.y - local_area.y.min) * 8 / d2);
	if (x < 0) x = 0;
	if (y < 0) y = 0;
	if (x >= 8) x = 7;
	if (y >= 8) y = 7;
	act = y * 8 + x;
	if (act2 != act)
	{
		cha_area();
		_multi_string* s = find_intermediate<_multi_string>(n_zagolovok, flag_information, flag_parent);
		std::wstring   ss;
		if (s)
			if ((int)s->line.size() > act) ss = s->line[act];
		if (ss.empty()) ss = std::to_wstring(act);
		add_hint(ss, this);
	}
	return true;
}

void _g_edit64bit::ris2(_trans tr, bool final)
{
	_area  oo = tr(local_area);
	double d = oo.x.length();
	double d2 = oo.y.length();
	if ((d < 5) || (d2 < 5)) return;
	u64* c = (u64*)&a;
	for (int j = 0; j < 8; j++)
		for (int i = 0; i < 8; i++)
			if (*c & (1ULL << (j * 8 + i)))
				master_bm.fill_rectangle({ {(i64)(oo.x.min + 1 + (d - 1) * 0.125 * i),
					(i64)(oo.x.min + (d - 1) * 0.125 * (i + 1.0)) + 1},
					{(i64)(oo.y.min + 1 + (d2 - 1) * 0.125 * j),
					(i64)(oo.y.min + (d2 - 1) * 0.125 * (j + 1.0)) + 1} }, cc2);
	if ((d < 25) || (d2 < 25)) { master_bm.rectangle((_area)oo, cc1); }
	else
	{
		for (int i = 0; i <= 8; i++)
		{
			master_bm.line({ oo.x.min, oo.y.min + d2 * 0.125 * i }, { oo.x.max, oo.y.min + d2 * 0.125 * i }, cc1);
			master_bm.line({ oo.x.min + d * 0.125 * i, oo.y.min }, { oo.x.min + d * 0.125 * i, oo.y.max }, cc1);
		}
		if (act >= 0)
		{
			int j = act / 8;
			int i = act % 8;
			//			master_bm.rectangle((int)(oo.x.min + d * 0.125 * i), (int)(oo.y.min + d2 * 0.125 * j),
			//				(int)(oo.x.min + d * 0.125 * (i + 1.0)), (int)(oo.y.min + d2 * 0.125 * (j + 1.0)),
			//				0xFF0000);
			master_bm.rectangle({ {(i64)(oo.x.min + d * 0.125 * i), (i64)(oo.x.min + d * 0.125 * (i + 1.0)) + 1},
				{(i64)(oo.y.min + d2 * 0.125 * j), (i64)(oo.y.min + d2 * 0.125 * (j + 1.0)) + 1} }, 0xFFFF0000);
		}
	}
}

_g_edit64bit::_g_edit64bit()
{
	local_area = { {0, 58}, {0, 58} };
	a = 0;
	act = -1;
}

void _g_edit64bit::run(_tetron* tt0, _tetron* tt, u64 flags) { cha_area(); }

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void _g_button::RisIco(astr kod, const char* s)
{
	picture.resize({ 24, 24 });
	picture.clear(0xFF000000);
	for (; *kod; kod++)
	{
		if (*kod == 'a')
		{

		}
	}
	if (*s == 0) return;
	int n = 1;
	for (astr i = s; *i; i++)
		if (*i == '\n') n++;
	int y0 = -5;
	int dy = 6;
	if (n == 1)	y0 = 4;
	if (n == 2)
	{
		y0 = -1;
		dy = 10;
	}
	if (n == 3)
	{
		y0 = -3;
		dy = 7;
	}
	astr ss = s;
	i64 y = y0;
	for (astr i = s; *i; i++)
		if (*i == '\n')
		{
			picture.text16({ 0LL, y }, ss, cc1);
			y += dy;
			ss = i + 1;
		}
	picture.text16({ 0LL, y }, ss, cc1);
}

void _g_button::ris2(_trans tr, bool final)
{
	_area oo = tr(local_area);
	uint c = 0;
	if (picture.size.x * picture.size.y > 0)
	{
		i64 rx2 = (i64)(picture.size.x * tr.scale + 0.5);
		i64 ry2 = (i64)(picture.size.y * tr.scale + 0.5);
		_ixy ce = oo.center();
		master_bm.stretch_draw(&picture, ce.x - rx2 / 2, ce.y - ry2 / 2, tr.scale);
	}
	else
		c = cc1;
	if (checked) c = cc1 - 0x40000000;
	if (n_go_move == this) c = cc1 - 0x80000000;
	if (n_tani == this) c = cc2 - 0x80000000;
	master_bm.fill_rectangle({ {(i64)oo.x.min, (i64)oo.x.max + 1}, {(i64)oo.y.min, (i64)oo.y.max + 1} }, c);
}

void _g_button::push(_stack* mem)
{
	_t_go::push(mem);
	*mem << checked << hint << picture;
}

void _g_button::pop(_stack* mem)
{
	_t_go::pop(mem);
	*mem >> checked >> hint >> picture;
}

void _g_button::push(_wjson& b)
{
	_t_go::push(b);
	b.add("checked", checked);
	b.add("hint", hint);
	b.add("picture", picture, c00, cc1);
}

void _g_button::pop(_rjson& b)
{
	_t_go::pop(b);
	b.read("checked", checked);
	b.read("hint", hint);
	b.read("picture", picture, c00, cc1);
}

_g_button::_g_button() : checked(0)
{
	local_area = { {0, 26}, {0, 26} };
}

bool _g_button::mouse_move2(_xy r)
{
	if (n_go_move != this) // первое перемещение
	{
		cha_area();
	}
	if (!n_hint) add_hint(hint, this); // нет подсказки
	return true;
}

void _g_button::mouse_finish_move()
{
	cha_area();
	del_hint();
}

bool _g_button::mouse_down_left2(_xy r)
{
	if (test_flags(n_checkbox, flag_parent)) checked = !checked;
	run(this, this, flag_run);
	cha_area();
	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool _g_list_link::mouse_down_left2(_xy r)
{
	if (!find1<_g_scrollbar>(flag_part))
	{
		_g_scrollbar* sb = new _g_scrollbar;
		sb->vid = 3;
		add_flags(sb, flag_sub_go + flag_part + (flag_run << 32));
	}
	cursor = (int)(first + (r.y - 4) / 16);
	if (*n_s_double->operator i64 * ())
	{
		_g_tetron* g = find1<_g_tetron>(flag_specialty);
		if (g == nullptr) return true;
		_tetron* a = g->find1<_tetron>(flag_specialty);
		if (a == nullptr) return true;
		if ((cursor < 0) || (cursor >= (int)a->link.size())) return true;
		_id t = (*a->link[cursor])(a);
		n_act = t; // заменить на новое!!
		smena_avt(); // заменить на новое!!
		g->add_unique_flags(t, flag_specialty);
		g->run(0, g, flag_run);
	}
	else
		run(0, this, flag_run); // ChaArea();
	return true;
}

void _g_list_link::ris2(_trans tr, bool final)
{
	_g_rect::ris2(tr, final);
	int sf = (int)(13 * tr.scale + 0.5);
	if (sf < 1) return;
	_g_tetron* g = find1<_g_tetron>(flag_specialty);
	if (g == nullptr) return;
	_tetron* a = g->find1<_tetron>(flag_specialty);
	if (a == nullptr) return;
	if (a->link.size() == 0) return;
	_area oo = tr(local_area);
	int len2 = (int)((local_area.y.length() - 8) / 16);
	if (len2 < 1) return;
	_g_scrollbar* polz = find1<_g_scrollbar>(flag_part);
	if (polz)
	{
		int k = (int)a->link.size() - len2;
		if (k >= 1)
		{
			first = (int)(polz->position * k + 0.5);
			if (cursor < first) cursor = first;
			if (cursor >= first + len2) cursor = first + len2 - 1;
		}
	}
	if (cursor >= (int)a->link.size()) cursor = (int)a->link.size() - 1;
	if (cursor < 0) cursor = 0;
	if (first > cursor) first = cursor;
	if (first + len2 > (int)a->link.size()) first = (int)a->link.size() - len2;
	if (first < 0) first = 0;
	int k = len2;
	if ((int)a->link.size() < first + k) k = (int)a->link.size() - first;
	for (int i = 0; i < k; i++)
	{
		int ii = first + i;
		std::wstring s;
		s = string_to_wstring((*a->link[ii])(a)->name());
		//		s = int_to_hex((uint64)a->link[ii].tetron);
		//		int nn = a->link[ii].nom_link();
		//			s += L" " + IntToString(nn);
		s = std::to_wstring(ii) + L". " + s;
		uint color = cc1;
		master_bm.text({ (i64)(oo.x.min + 4 * tr.scale), (i64)(oo.y.min + (i * 16.0 + 4) * tr.scale) }, s.c_str(), sf,
			color, 0);
	}
	if (first) master_bm.line({ oo.x.min + 2 * tr.scale, oo.y.min + 2 * tr.scale },
		{ oo.x.max - 2 * tr.scale, oo.y.min + 2 * tr.scale }, 0xFF30C0F0);
	if (first + k < (int)a->link.size()) master_bm.line({ oo.x.min + 2 * tr.scale, oo.y.max - 2 * tr.scale },
		{ oo.x.max - 2 * tr.scale, oo.y.max - 2 * tr.scale }, 0xFF30C0F0);
	master_bm.rectangle({ {(i64)(oo.x.min + 2 * tr.scale), (i64)(oo.x.max - 2 * tr.scale) + 1},
		{(i64)(oo.y.min + (4 + ((i64)cursor - first) * 16) * tr.scale),
		(i64)(oo.y.min + (19 + ((i64)cursor - first) * 16) * tr.scale) + 1} }, cc1);
	if ((tf != a) || (cursor != curf))
	{
		tf = a;
		curf = cursor;
		i64 a1 = 0;
		if ((cursor >= 0) || (cursor < (int)a->link.size()))
			if ((*a->link[cursor])(a))
				a1 = a->link[cursor]->get_flags(a);
		_g_edit64bit* ee = find1<_g_edit64bit>(flag_information);
		if (ee)
		{
			ee->a = a1;
			//			ee->Recovery(); // должны рисоваться после этого списка или все вычисления вынести из Ris в run
		}
	}
}

void _g_list_link::run(_tetron* tt0, _tetron* tt, u64 flags)
{
	cha_area();
	_g_edit64bit* ee = find1<_g_edit64bit>(flag_information);
	if (ee) ee->cha_area();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void _g1list::init_sel() //вызывается только при отсутствии
{
	for (auto i : link)
	{
		_tetron* t = (*i)(this);
		if (!t->operator _t_string * ()) continue;
		if (!i->test_flags(this, flag_part)) continue;
		i->set_flags(this, i->get_flags(this) | flag_specialty);
		return;
	}
}

void _g1list::ris2(_trans tr, bool final)
{
	_area oo2 = tr(oo);
	uint c = get_c();
	master_bm.rectangle((_area)oo2, c);
	_t_string* s = find1<_t_string>(flag_specialty);
	if (s == nullptr)
	{
		init_sel();
		s = find1<_t_string>(flag_specialty);
		if (s == nullptr) return;
	}
	int sf = (int)(13 * tr.scale + 0.5);
	if (sf < 1) return;
	master_bm.text({ (i64)(oo2.x.min + 4), (i64)(oo2.y.min + 1) }, s->s.c_str(), sf, c, 0);
	if (rez1) return;
	bool start = false;
	int dx = 0;
	for (int i = (int)link.size() - 1; i >= 0; i--)
	{
		_tetron* a = (*link[i])(this);
		if (!a->operator std::wstring * ()) continue;
		if (!link[i]->test_flags(this, flag_part)) continue;
		if (start)
		{
			dx--;
			master_bm.text({ (i64)(oo2.x.min + 4), (i64)(oo2.y.min + dx * 16.0 * tr.scale) },
				a->operator std::wstring * ()->c_str(), sf, c, 0);
		}
		if (link[i]->test_flags(this, flag_specialty)) start = true; // если будет несколько - будут проблемы
	}
	start = false;
	dx = 0;
	for (auto i : link)
	{
		_tetron* a = (*i)(this);
		if (!a->operator std::wstring * ()) continue;
		if (!i->test_flags(this, flag_part)) continue;
		if (start)
		{
			dx++;
			master_bm.text({ (i64)(oo2.x.min + 4), (i64)(oo2.y.min + dx * 16.0 * tr.scale) },
				a->operator std::wstring * ()->c_str(), sf, c, 0);
		}
		if (i->test_flags(this, flag_specialty)) start = true; // если будет несколько - будут проблемы
	}
}

void _g1list::calc_local_area()
{
	local_area = oo;
	if (rez1) return;
	int n = 0;
	int na = 0;
	for (auto i : link)
	{
		_tetron* t = (*i)(this);
		if (!t->operator _t_string * ()) continue;
		if (!i->test_flags(this, flag_part)) continue;
		n++;
		if (i->test_flags(this, flag_specialty)) na = n;
	}
	local_area.y.max += ((i64)n - na) * 16;
	local_area.y.min -= (na - 1LL) * 16;
	area_definite = false;
}

_g1list::_g1list()
{
	oo = { {0, 100}, {0, 16} };
	rez1 = true;
	local_area = oo;
}

void _g1list::mouse_finish_move()
{
	if (!rez1)
	{
		del_area();
		rez1 = true;
		calc_local_area();
		add_area();
	}
}

bool _g1list::mouse_move2(_xy r)
{
	if (!rez1)
	{
		del_area();
		rez1 = true;
		calc_local_area();
		add_area();
	}
	return true;
}

bool _g1list::mouse_wheel2(_xy r)
{
	_t_string* bb = find1<_t_string>(flag_specialty);
	if (bb == nullptr)
	{
		init_sel();
		return true;
	}
	int dx = *n_wheel->operator i64 * ();
	if (dx == 0) return true;
	del_area();
	bool start = false;
	_tetron* b = bb;
	if (dx > 0)
	{
		for (int i = (int)link.size() - 1; i >= 0; i--)
		{
			_tetron* a = (*link[i])(this);
			if (!a->operator _t_string * ()) continue;
			if (!link[i]->test_flags(this, flag_part)) continue;
			if (start)
			{
				b = a;
				dx--;
				if (dx == 0) break;
			}
			if (bb == a) start = true;
		}
	}
	else
	{
		for (auto i : link)
		{
			_tetron* a = (*i)(this);
			if (!a->operator _t_string * ()) continue;
			if (!i->test_flags(this, flag_part)) continue;
			if (start)
			{
				b = a;
				dx++;
				if (dx == 0) break;
			}
			if (bb == a) start = true;
		}
	}
	del_flags(bb, flag_specialty);
	add_flags(b, flag_specialty);
	rez1 = false;
	calc_local_area();
	add_area();
	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool _g_tetron::mouse_move2(_xy r)
{
	if (n_hint) return true; // есть подсказка
	_tetron* a = find1<_tetron>(flag_specialty);
	std::wstring re;
	if (a) re = string_to_wstring(a->name()) + L" " + std::to_wstring(a->id);
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
	_area a = tr(local_area);
	_xy p = a.center();
	double r = a.min_length() * 0.5;
	double d = log(1.001 + r) * 0.5;
	double y = 0.7 * (r - d) * 2;
	uint c = cc4;

	_tetron* t2 = find1<_tetron>(flag_specialty); // связанный тетрон
	bool im_temp = test_flags(n_temp_go, flag_parent); // текущий _g_tetron - временный
	// выделение красным цветом
	static _tetron* root_tetron = nullptr;
	if (!im_temp)
		root_tetron = t2;
	else
		if (t2 == root_tetron) c = 0xFFFF0000;

	master_bm.fill_ring(p, r, d, c, (im_temp) ? 0 : cc0);

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
		master_bm.froglif(p - _xy{ y / 2, y / 2 }, y, (uchar*)&fr, 2, cc2);
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
				ttr3->trans.offset = { 0.5 * local_area.x, 0.5 * local_area.y };
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
		if (!(*i)(t2)->test_flags(n_temp_go, flag_parent)) vl++;
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
		ttr3->trans.offset = { 0.5 * local_area.x, 0.5 * local_area.y };
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
		gg2->local_area = { {-1.0, 1.0}, {-1.0, 1.0} };
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
			_tetron* t = (*t2->link[ilink])(t2);
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
				gg->local_area = { {-1.0, 1.0}, {-1.0, 1.0} };
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
			gl->area_definite = false;
			inn++;
		}
		uze_nar += nn;
	}
	return;
}

void _g_tetron::add_unique_flags(_tetron* t, u64 flags, bool after)
{
	if (star) delete_hvost(star, true, false);
	_tetron::add_unique_flags(t, flags, after);
	//	cha_area();
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
	u64 f = t1->get_flags(t2);

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
		uint c = (f & (1ULL << i)) ? color_set[i] : cclow(color_set[i]);
		double t = 1;
		if (al == i)
		{
			t = 2;
			c = brighten(c);
		}
		if ((al != -1) || (f & (1ULL << i)))
		{
			auto beg = pp11 + (pp12 - pp11) * ((i + 0.5) / (v_link * 2)) + v * dl;
			auto end = pp21 + (pp22 - pp21) * ((i + 0.5) / (v_link * 2)) - v * dl2;
			master_bm.lines(beg, end, t, c);

			auto dist = beg - end;
			if (dist.len() >= 8)
			{
				dist = dist * (1 / dist.len()) * 7;
				master_bm.lines(end, end + dist.rotation(0.26), t, c);
				master_bm.lines(end, end + dist.rotation(-0.26), t, c);
			}
		}
		c = (f & (1ULL << (i + 32))) ? color_set[i] : cclow(color_set[i]);
		t = 1;
		if (al == (v_link * 2 - 1 - i))
		{
			t = 2;
			c = brighten(c);
		}
		if ((al != -1) || (f & (1ULL << (i + 32))))
		{
			auto beg = pp11 + (pp12 - pp11) * ((v_link * 2 - 0.5 - i) / (v_link * 2)) + v * dl;
			auto end = pp21 + (pp22 - pp21) * ((v_link * 2 - 0.5 - i) / (v_link * 2)) - v * dl2;
			master_bm.lines(beg, end, t, c);

			auto dist = end - beg;
			if (dist.len() >= 8)
			{
				dist = dist * (1 / dist.len()) * 7;
				master_bm.lines(beg, beg + dist.rotation(0.26), t, c);
				master_bm.lines(beg, beg + dist.rotation(-0.26), t, c);
			}
		}
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
	local_area = _area();
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
	_area a1 = tr1->trans(g1->local_area);
	_area a2 = tr2->trans(g2->local_area);
	par->trans = _trans();
	_xy p1 = a1.center();
	double r1 = a1.min_length() * 0.5;
	_xy p2 = a2.center();
	double r2 = a2.min_length() * 0.5;
	_xy d = p2 - p1;
	if ((d.y == 0) && (d.x == 0)) return;
	double alpha = atan2(d.y, d.x);
	double dv0 = d.len();
	double dv = (dv0 - (r1 + r2) * cos(dalpha)) * k;
	if (k < 0.9)
		dv = (dv0 - (r1 + r2)) * k + r1 * (1 - cos(dalpha));
	double kk = dv / dv0;
	local_area |= p11 = _xy{ p1.x + r1 * cos(alpha - dalpha), p1.y + r1 * sin(alpha - dalpha) };
	local_area |= p12 = _xy{ p1.x + r1 * cos(alpha + dalpha), p1.y + r1 * sin(alpha + dalpha) };
	p21 = { p11.x + d.x * kk, p11.y + d.y * kk };
	p22 = { p12.x + d.x * kk, p12.y + d.y * kk };
	local_area |= p21;
	local_area |= p22;
}

bool _g_link::mouse_down_left2(_xy r)
{
	if (act_li < 0) return true;

	_g_tetron* g1 = find1<_g_tetron>(flag_specialty);
	_g_tetron* g2 = find1<_g_tetron>(flag_specialty2);
	_tetron* t1 = g1->find1<_tetron>(flag_specialty);
	_tetron* t2 = g2->find1<_tetron>(flag_specialty);
	u64 f;
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
	double kk = a / b; // 0..1
	int ii = act_li;
	act_li = (int)(kk * v_link * 2);
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

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
