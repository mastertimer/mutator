#pragma once

#include "basic_go.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct _g_edit_double : public _g_rect
{
	int cursor;
	double a;

	_g_edit_double();

	operator double* ()     override { return &a; }

	uchar type()            override { return 33; }
	int   get_froglif()     override { return 0x79; }
	void  push(_stack* mem) override { _g_rect::push(mem); *mem << a; }
	void  pop(_stack* mem)  override { _g_rect::pop(mem); *mem >> a; }
	void  push(_wjson& b)   override { _g_rect::push(b); b.add("a", a); }
	void  pop(_rjson& b)    override { _g_rect::pop(b); b.read("a", a); }

	void ris2(_trans tr, bool final) override;
	void key_down(ushort key)        override;
	void key_press(ushort key)       override;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct _g_edit_int : public _t_go
{
	int cursor = 0;

	_g_edit_int() { key_fokus = true; local_area = { {0, 100}, {0, 16} }; }

	uchar type()                      override { return 4; }
	int   get_froglif()               override { return 0xF9; }

	void  ris2(_trans tr, bool final) override;
	bool  mouse_wheel2(_coo2 r)       override;
	void  key_down(ushort key)        override;
	void  key_press(ushort key)       override;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct _g_edit_string : public _t_go
{
	int cursor = 0; // абсолютное положение курсора
	int first = 0; // номер первого символа
	int len2 = 0; // длина куска

	_g_edit_string();
	uchar type() { return 25; }
	int get_froglif() { return 0xFB; }
	void ris2(_trans tr, bool final) override;
	void key_down(ushort key);
	void key_press(ushort key);
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct _g_edit_multi_string : public _t_go
{
	_multi_string strings;
	_num2 cursor;
	int64 first = 0; // номер первого элемента в списке

	_g_edit_multi_string();

	uchar type()            override { return 30; }
	int   get_froglif()     override { return 0xFD; }
	void  push(_stack* mem) override { _t_go::push(mem); strings.push(mem); }
	void  pop(_stack* mem)  override { _t_go::pop(mem); strings.pop(mem); }
	void  push(_wjson& b)   override { _t_go::push(b); b.add("strings", strings); }
	void  pop(_rjson& b)    override { _t_go::pop(b); b.read("strings", strings); }

	void ris2(_trans tr, bool final)                  override;
	void key_press(ushort key)                        override;
	void key_down(ushort key)                         override;
	bool mouse_down_left2(_coo2 r)                    override;
	void run(_tetron* tt0, _tetron* tt, uint64 flags) override;

private:
	int max_i = 0, len2 = 0; // вспомогательные
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct _g_edit64bit : public _t_go
{
	int64 a;

	_g_edit64bit();
	uchar type() { return 1; }
	int get_froglif() { return 0xFE; }
	void ris2(_trans tr, bool final) override;
	bool mouse_move2(_coo2 r) override;
	void mouse_finish_move();
	bool mouse_down_left2(_coo2 r) override;
	operator _g_edit64bit* () { return this; }
	void run(_tetron* tt0, _tetron* tt, uint64 flags);

private:
	int act;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct _g_edit_one : public _g_rect
{
	int n = 0; // идекс массива 
	size_t cursor = 0;
	int zn = 5;
	double a = 0.0;
	int64 a_i = 0;
	std::wstring s = L"0";
	int mode = 0; // 0 - int64, 1 - double

	_g_edit_one();
	uchar type() { return 15; }
	int get_froglif() { return 0x79; }
	void ris2(_trans tr, bool final) override;
	bool mouse_down_left2(_coo2 r) override;
	void key_down(ushort key);
	void key_press(ushort key);
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct _g_button : public _t_go
{
	int64 checked;
	std::wstring hint;
	_picture picture;

	_g_button();

	operator std::wstring* ()        override { return &hint; }
	operator _picture* ()            override { return &picture; }
	operator int64* ()               override { return &checked; }
	operator _g_button* ()           override { return this; }

	uchar type()                     override { return 34; }
	int   get_froglif()              override { return 0xF3; }
	void  mouse_up_left2(_coo2 r)    override { cha_area(); }
	void  mouse_move_left2(_coo2 r)  override {}

	void ris2(_trans tr, bool final) override;
	bool mouse_move2(_coo2 r)        override;
	void mouse_finish_move()         override;
	bool mouse_down_left2(_coo2 r)   override;
	void push(_stack* mem)           override;
	void pop(_stack* mem)            override;
	void push(_wjson& b)             override;
	void pop(_rjson& b)              override;

	void RisIco(astr kod, const char* s);
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct _g_color_ring : public _t_go
{
	_hsva color = c_def;

	_g_color_ring() { local_area = { {0, 300}, {0, 300} }; }
	uchar type() { return 17; }
	int get_froglif() { return 0x72; }
	void ris2(_trans tr, bool final) override;
	bool mouse_down_left2(_coo2 r)   override { change(r, true); return true; }
	void mouse_move_left2(_coo2 r)   override { change(r, false); }
	void mouse_up_left2(_coo2 r)     override {}

private:
	static constexpr double li_r_1 = 0.35;
	static constexpr double li_r_2 = 0.55;
	static constexpr double zazor = (2 * 3.14159265358979323846) * 0.02;
	int iii = 0; // номер четверти при перетаскивании
	void change(_coo2 r, bool start);
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct _g_list_link : public _g_rect // графический объект - список связей
{
	int cursor = 0; // абсолютный номер элемента
	int first = 0; // номер первого элемента в списке

	uchar type() { return 28; }
	int get_froglif() { return 0xF5; }
	void ris2(_trans tr, bool final) override;
	bool mouse_down_left2(_coo2 r) override;
	void run(_tetron* tt0, _tetron* tt, uint64 flags) override;
	operator _g_list_link* () { return this; }

private:
	_tetron* tf = nullptr; // для какого тетрона отображены флаги
	int curf = 0; // курсор у которого отображены флаги
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct _g1list : public _t_go
{
	_area2 oo; // область
	bool rez1; // вывод только 1 элемента

	_g1list();

	uchar type()               override { return 2; }
	int   get_froglif()        override { return 0xF7; }
	void  push(_stack* mem)    override { _t_go::push(mem); *mem << oo; }
	void  pop(_stack* mem)     override { _t_go::pop(mem); *mem >> oo; }
	void  push(_wjson& b)      override { _t_go::push(b); b.add("oo", oo); }
	void  pop(_rjson& b)       override { _t_go::pop(b); oo = b.read_area2("oo"); }

	operator _g1list* ()       override { return this; }

	void ris2(_trans tr, bool final) override;
	bool mouse_wheel2(_coo2 r)       override;
	void mouse_finish_move()         override;
	bool mouse_move2(_coo2 r)        override;

private:
	void init_sel(); // задать F_sel
	void calc_local_area(); // область только этого объекта
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct _g_tetron : public _t_go
{
	std::wstring hint;
	_id star;

	_g_tetron() { local_area = { {0, 24}, {0, 24} }; }

	uchar  type()            override { return 38; }
	int    get_froglif()     override { return 0xFA; }
	void   push(_stack* mem) override { _t_go::push(mem); *mem << hint; }
	void   pop(_stack* mem)  override { _t_go::pop(mem); *mem >> hint; }
	void   push(_wjson& b)   override { _t_go::push(b); b.add("hint", hint); }
	void   pop(_rjson& b)    override { _t_go::pop(b); b.read("hint", hint); }
	double final_radius()    override { return 21.0; } // минимальный полуразмер, после которого не рисуется

	operator _g_tetron* () { return this; }
	operator std::wstring* () { return &hint; }

	void ris2(_trans tr, bool final) override;
	bool mouse_move2(_coo2 r)        override;
	void mouse_finish_move()         override;
	bool test_local_area(_coo2 b)    override; // лежит ли точка внутри
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

	_g_tetron2() { local_area = { {0, 24}, {0, 24} }; }
	uchar type() { return 18; }
	int get_froglif() { return 0xFF; }
	void ris2(_trans tr, bool final) override;
	double final_radius()  override { return 21.0; } // минимальный полуразмер, после которого не рисуется структура
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template <typename _t>
_t* find1_plus_gtetron(_tetron* tet, uint64 flags)
{
	for (auto i : tet->link)
	{
		_tetron* a = i->pairr(tet);
		if (!i->test_flags(tet, flags)) continue;
		if (_g_tetron * g = *a) // как редактировать hint _g_tetron?
		{
			_t* t2 = g->find1<_t>(flag_specialty);
			if (t2 != nullptr) return t2;
		}
		if (_t * x = *a) return x;
	}
	return nullptr;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct _g_link : public _t_go
{
	static constexpr double dalpha = 0.4;
	static constexpr int64 v_link = 9;

	_coo2 p11 = { 0, 0 };
	_coo2 p12 = { 0, 0 };
	_coo2 p21 = { 0, 0 };
	_coo2 p22 = { 0, 0 };
	double k = 1.0;
	int act_li = -1;

	_g_link() = default;
	uchar type() { return 8; }
	int get_froglif() { return 0x7A; }
	void calc_local_area();
	void ris2(_trans tr, bool final) override;
	bool test_local_area(_coo2 b) override; // лежит ли точка внутри
	bool mouse_move2(_coo2 r) override;
	void mouse_finish_move();
	bool mouse_down_left2(_coo2 r) override;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
