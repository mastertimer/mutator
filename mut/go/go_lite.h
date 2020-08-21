#pragma once

#include "t_basic_go.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void del_hint(); // удалить подсказку
void add_hint(std::wstring_view hint, _t_go* g); // создать подсказку
void change_hint(std::wstring_view hint); // изменить подсказку

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct _g_circle : public _t_go
{
	_g_circle() { calc_local_area(); }

	uchar type()            override { return 10; }
	int   get_froglif()     override { return 0x74; }
	void  push(_stack* mem) override { _t_go::push(mem); *mem << center << radius << width; }
	void  pop(_stack* mem)  override { _t_go::pop(mem); *mem >> center >> radius >> width; }

	operator _g_circle* ()  override { return this; }

	void push(_wjson& b)                              override;
	void pop(_rjson& b)                               override;
	void ris2(_trans tr, bool final)                  override;
	void run(_tetron* tt0, _tetron* tt, uint64 flags) override;
	bool test_local_area(_xy b)                       override; // лежит ли точка внутри

	void calc_local_area();

private:
	_xy center{ 0,0 };
	double radius{ 20 };
	double width{ 2 };
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct _g_froglif : public _t_go
{
	static constexpr int size_f = 43;
	static constexpr int r_f = 4;
	union
	{
		uchar f[size_f]{};
		int f_int;
		int64 f_int64;
	};

	_g_froglif() { local_area = _area_old(0, 24, 0, 24); }

	uchar type()               override { return 14; }
	int   get_froglif()        override { return 0x7F; }
	void  push(_stack* mem)    override { _t_go::push(mem); mem->push_data(f, size_f); }
	void  pop(_stack* mem)     override { _t_go::pop(mem); mem->pop_data(f, size_f); }
	void  push(_wjson& b)      override { _t_go::push(b); b.add_mem("f", f, size_f); }
	void  pop(_rjson& b)       override { _t_go::pop(b); b.read_mem("f", f, size_f); }

	operator int64* ()         override { return &f_int64; }

	void ris2(_trans tr, bool final) override;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct _g_line : public _t_go
{
	_g_line() { calc_local_area(); }

	uchar type()            override { return 11; }
	int   get_froglif()     override { return 0x70; }
	void  push(_stack* mem) override { _t_go::push(mem); *mem << p1 << p2 << width; }
	void  pop(_stack* mem)  override { _t_go::pop(mem); *mem >> p1 >> p2 >> width; }

	void push(_wjson& b)                              override;
	void pop(_rjson& b)                               override;
	void ris2(_trans tr, bool final)                  override;
	void run(_tetron* tt0, _tetron* tt, uint64 flags) override;
	bool test_local_area(_xy b)                       override; // лежит ли точка внутри

	void calc_local_area();

private:
	_xy p1{ 0, 0 }, p2{ 100, 100 }; // начало и конец
	double width{ 1 };
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct _g_picture : public _t_go
{
	_picture pic;

	_g_picture() { local_area = _area_old(0, 100, 0, 100); }

	uchar type()            override { return 31; }
	int   get_froglif()     override { return 0xF6; }
	void  push(_stack* mem) override { _t_go::push(mem); *mem << pic; }
	void  pop(_stack* mem)  override { _t_go::pop(mem); *mem >> pic; }
	void  push(_wjson& b)   override { _t_go::push(b); b.add("pic", pic); }
	void  pop(_rjson& b)    override { _t_go::pop(b); b.read("pic", pic); }

	operator _g_picture* () override { return this; }
	operator _picture* ()   override { return &pic; }

	void ris2(_trans tr, bool final) override;

	void new_size(int rx3, int ry3);
	bool load_from_file(_path fn);
	bool save_to_file(_path fn) const;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct _g_text : public _t_go
{
	std::wstring s = L"";

	_g_text() { local_area = _area_old(0, 13, 0, 13); }

	uchar type()              override { return 29; }
	int   get_froglif()       override { return 0xF1; }
	bool  mouse_move2(_xy r)  override { return false; }
	void  push(_stack* mem)   override { _t_go::push(mem); *mem << s; }
	void  pop(_stack* mem)    override { _t_go::pop(mem); *mem >> s; }
	void  push(_wjson& b)     override { _t_go::push(b); b.add("s", s); }
	void  pop(_rjson& b)      override { _t_go::pop(b); b.read("s", s); }

	operator std::wstring* () override { return &s; }
	operator _g_text* ()      override { return this; }

	void ris2(_trans tr, bool final) override;

	void set_text(std::wstring_view s2);
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct _g_rect : public _t_go
{
	_g_rect() { local_area = _area_old(0, 100, 0, 100); }

	uchar type()         override { return 12; }
	int   get_froglif()  override { return 0xF4; }

	operator _g_rect* () override { return this; }

	void ris2(_trans tr, bool final) override;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct _g_test_graph : public _t_go // !! Ќ≈ ”ƒјЋя“№, ѕ–»√ќƒ»“—я !!
{
	_picture a;

	_g_test_graph();
	uchar type() { return 6; }
	int get_froglif() { return 0xF2; }
	void ris2(_trans tr, bool final) override;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
