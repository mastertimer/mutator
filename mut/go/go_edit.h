#pragma once

#include "t_basic_go.h"

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

	_g_edit_int() { key_fokus = true; local_area = _area_old(0, 100, 0, 16); }

	uchar type()                      override { return 4; }
	int   get_froglif()               override { return 0xF9; }

	void  ris2(_trans tr, bool final) override;
	bool  mouse_wheel2(_xy r)         override;
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
	_ixy cursor;
	int first = 0; // номер первого элемента в списке

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
	bool mouse_down_left2(_xy r)                      override;
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
	bool mouse_move2(_xy r);
	void mouse_finish_move();
	bool mouse_down_left2(_xy r);
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
	bool mouse_down_left2(_xy r);
	void key_down(ushort key);
	void key_press(ushort key);
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
