#include "tetron.h"

__hash_table<_link> link;

uint hash_func(const _he_intermediate& a)
{
	return (uint)((((uint64)a.tetron_before) >> 4) * 27644437 + (((uint64)a.tetron_after) >> 4) * 33391 +
	              a.flags_before * 16769023 + a.flags_after * 17971);
}

uint hash_func(const _pair_tetron& a)
{
	return (uint)((((uint64)a.low_tetron) >> 4) * 27644437 + (((uint64)a.high_tetron) >> 4) * 33391);
}

_tetron::_tetron() { all_tetron.insert(id = id_tetron++, this); }

void _tetron::find_all_intermediate(_tetron* t, uint64 flags_before, uint64 flags_after, _vector_id& res)
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

namespace SuperDelTetron2
{
	_speed<_vector_tetron>          ud(false); // список на удаление
	_speed<_hash_table_tetron>      Sp(false); // полный список
	__hash_table<_pair_t<_tetron*>> Sp2;       // полный список 2 проход
	_speed<_hash_table_tetron>      Ba(false); // плохой список
	_tetron*                        start;     // стартовый особый тетрон

	void MetkaDelB12(_tetron* b)
	{
		if (Sp2.find(b)) return;
		if (Ba->find(b)) return;
		Sp2.insert(b);
		ud->push_back(b);
		for (auto i : b->link)
		{
			_tetron* a = (*i)(b);
			if (!i->test_flags(b, flag_part)) continue;
			MetkaDelB12(a);
		}
	}

	bool moi(_tetron* b)
	{
		for (auto i : b->link)
		{
			_tetron* a = (*i)(b);
			if (!i->test_flags(a, flag_part)) continue;
			if (a == start)
				continue; // NEW
			            // !!! могут удалитьс¤ нужные родители !!!
			            // —делать 2 прохода јнтиметок, если q2 точно != q
			            // если да, проверить NEW
			            // 2й проход 1й проход
			if ((Ba->find(a)) || (!Sp->find(a))) return false;
		}
		return true;
	}

	void AntiMetkaB12(_tetron* b)
	{
		if (Sp2.find(b)) return;
		if (Ba->find(b)) return;
		Sp2.insert(b);
		if (moi(b))
		{
			for (auto i : b->link)
			{
				_tetron* a = (*i)(b);
				if (!i->test_flags(b, flag_part)) continue;
				AntiMetkaB12(a);
			}
		}
		else
			b->traversal(Ba, flag_part);
	}

	void PodgSpUnikSv(_tetron* b) // подготовить список уникальный свойств тетрона
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
			AntiMetkaB12(a);
		}
		// подготовить список
		Sp2.clear();
		Sp2.insert(b);
		for (auto i : b->link)
		{
			_tetron* a = (*i)(b);
			if (!i->test_flags(b, flag_part)) continue;
			MetkaDelB12(a);
		}
	}

	void FreeBank()
	{
		Ba.stop();
		Sp.stop();
		ud.stop();
	}
} // namespace SuperDelTetron2

_tetron* _tetron::copy_plus()
{
	SuperDelTetron2::PodgSpUnikSv(this);
	SuperDelTetron2::ud->push_back(this); // т.к. в списке главного тетрона нет
	                                      // Sp2 = ud ???
	for (auto b : *SuperDelTetron2::ud)
	{
		_tetron* a = create_tetron(b->type());
		a->copy(b);
		SuperDelTetron2::Sp2.find(b)->a = a;
	}
	for (auto a : *SuperDelTetron2::ud)
	{
		_tetron* aa = SuperDelTetron2::Sp2.find(a)->a;
		for (auto j : a->link)
		{
			_tetron* b = (*j)(a);
			auto     n = SuperDelTetron2::Sp2.find(b);
			if (!n)
			{
				aa->add_flags(b, j->get_flags(a) & 0xFFFFFFFF, false);
				continue;
			}
			if (aa <= n->a) aa->add_flags(n->a, j->get_flags(a), false);
		}
	}
	_tetron* rr = SuperDelTetron2::Sp2.find(this)->a;
	SuperDelTetron2::FreeBank();
	return rr;
}

uint64 _tetron::get_flags(_tetron* t)
{
	_link& li = *::link.find(_pair_tetron(this, t));
	return (li.low_tetron) ? li.get_flags(this) : 0;
}

void _tetron::set2_flags(_tetron* t, uint64 flags, func_fl trans, bool after)
{
	if (!t) return;
	auto   err = ::link.find(_pair_tetron(this, t));
	_link& li  = *err;
	uint64 fl  = (li.low_tetron) ? li.get_flags(this) : 0;
	trans(fl, flags);
	if (li.low_tetron)
	{
		li.set_flags(this, fl);
		return;
	}
	if (fl == 0) return;
	err.life();

	if ((uint64)this <= (uint64)t)
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

void _tetron::run(_tetron* tt0, _tetron* tt, uint64 flags)
{
	for (_frozen i(this, flag_parent); i; i++) i->run(tt0, tt, flags);
	for (_frozen i(this, flags); i; i++) i->run(tt, i, flags);
}

void _tetron::traversal(_hash_table_tetron* ht, uint64 flags, _vector_tetron* lt)
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

void _tetron::add_unique_flags(_tetron* t, uint64 flags, bool after)
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
	uint          size_old   = hash_intermediate.size;
	static uint64 old_number = 0;
	static double k          = 0.5;
	uint64        n_gr       = (uint64)(old_number + (number_intermediate - old_number) * k);
	for (auto& i : hash_intermediate)
	{
		if ((i.tetron_before->get_flags(i.tetron_intermediate) == 0) ||
		    (i.tetron_intermediate->get_flags(i.tetron_after) == 0) || (i.number <= n_gr))
			hash_intermediate.erase(i);
	}
	old_number = n_gr;
	double k2  = k / ((size_old * 0.5) / hash_intermediate.size);
	k          = k * 0.9 + k2 * 0.1;
}

void delete_hvost(_tetron* t, bool del_t, bool run_func)
{
	bool pprr           = run_before_del_link;
	run_before_del_link = run_func;
	SuperDelTetron2::PodgSpUnikSv(t);
	for (auto i : *SuperDelTetron2::ud) delete i;
	SuperDelTetron2::FreeBank();
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
		_link* li = low_tetron->link[n_low]                       = low_tetron->link.back();
		((li->low_tetron == low_tetron) ? li->n_low : li->n_high) = n_low;
	}
	low_tetron->link.pop_back();
	if (low_tetron != high_tetron)
	{
		if (n_high + 1ULL < high_tetron->link.size())
		{
			_link* li = high_tetron->link[n_high]                      = high_tetron->link.back();
			((li->low_tetron == high_tetron) ? li->n_low : li->n_high) = n_high;
		}
		high_tetron->link.pop_back();
	}
	low_tetron = nullptr;
	link.erase(this);
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

_frozen::_frozen(_tetron* t, uint64 flags_) : tetron(t), i(0), tetron2(nullptr), flags(flags_)
{
	for (auto j : t->link)
	{
		if (!j->test_flags(t, flags)) continue;
		if (!tetron2) tetron2 = (*j)(t);
		lt->push_back((*j)(t)); // проверить на move
	}
}

void add_obl_izm(_area2 a)
{
	if (a.empty()) return;
	if ((a.x.max < 0) || (a.x.min > master_bm.size.x) || (a.y.max < 0) || (a.y.min > master_bm.size.y)) return;
	master_obl_izm += a;
}
