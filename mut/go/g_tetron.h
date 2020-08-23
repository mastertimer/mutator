#pragma once

#include "basic_go.h"

struct _g_tetron : public _t_go
{
	std::wstring hint;
	_id star;

	_g_tetron()                       { local_area = _area_old(0, 24, 0, 24); }

	uchar  type()            override { return 38; }
	int    get_froglif()     override { return 0xFA; }
	void   push(_stack* mem) override { _t_go::push(mem); *mem << hint; }
	void   pop(_stack* mem)  override { _t_go::pop(mem); *mem >> hint; }
	void   push(_wjson& b)   override { _t_go::push(b); b.add("hint", hint); }
	void   pop(_rjson& b)    override { _t_go::pop(b); b.read("hint", hint); }
	double final_radius()    override { return 21.0; } // минимальный полуразмер, после которого не рисуется

	operator _g_tetron* ()            { return this; }
	operator std::wstring* ()         { return &hint; }

	void ris2(_trans tr, bool final) override;
	bool mouse_move2(_xy r)          override;
	void mouse_finish_move()         override;
	bool test_local_area(_xy b)      override; // лежит ли точка внутри
	void add_unique_flags(_tetron* t, uint64 flags, bool after = true) override; // создать уникальную связь

private:
	std::vector<_t_basic_go*> drawn_create_star; // стек отрисованных в момент создания звезды

	bool cmp_drawn(); // сравнение стеков
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct _molecule
{
	_id tetron; // тетрон
	_id tr_t;
};

struct _g_tetron2 : public _t_go
{
	std::wstring hint;
	std::vector<_molecule> mo;

	_g_tetron2() { local_area = _area_old(0, 24, 0, 24); }
	uchar type() { return 18; }
	int get_froglif() { return 0xFF; }
	void ris2(_trans tr, bool final) override;
	double final_radius()  override { return 21.0; } // минимальный полуразмер, после которого не рисуется структура
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template <typename _t>
_t* find1_plus_gtetron(_tetron *tet, uint64 flags)
{
	for (auto i : tet->link)
	{
		_tetron* a = i->pairr(tet);
		if (!i->test_flags(tet, flags)) continue;
		if (_g_tetron* g = *a) // как редактировать hint _g_tetron?
		{
			_t* t2 = g->find1<_t>(flag_specialty);
			if (t2 != nullptr) return t2;
		}
		if (_t* x = *a) return x;
	}
	return nullptr;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct _g_link : public _t_go
{
	static constexpr double dalpha = 0.4;
	static constexpr int64 v_link = 9;

	_xy p11 = { 0, 0 };
	_xy p12 = { 0, 0 };
	_xy p21 = { 0, 0 };
	_xy p22 = { 0, 0 };
	double k = 1.0;
	int act_li = -1;

	_g_link() = default;
	uchar type() { return 8; }
	int get_froglif() { return 0x7A; }
	void calc_local_area();
	void ris2(_trans tr, bool final) override;
	bool test_local_area(_xy b) override; // лежит ли точка внутри
	bool mouse_move2(_xy r);
	void mouse_finish_move();
	bool mouse_down_left2(_xy r);
};
