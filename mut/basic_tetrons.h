#pragma once

#include "tetron.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct _t_int : public _tetron
{
	int64 a = 0;

	operator int64* ()         override { return &a; }
	operator _t_int* ()        override { return this; }

	uchar type()               override { return 26; }
	int   get_froglif()        override { return 0x40; }
	void  push(_stack* mem)    override { _tetron::push(mem); *mem << a; }
	void  pop(_stack* mem)     override { _tetron::pop(mem); *mem >> a; }
	void  push(_wjson& b)      override { _tetron::push(b); b.add("a", a); }
	void  pop(_rjson& b)       override { _tetron::pop(b); b.read("a", a); }
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct _t_double : public _tetron
{
	double a = 0.0;

	operator _t_double* ()  override { return this; }

	uchar type()            override { return 7; }
	int   get_froglif()     override { return 0x50; }
	void  push(_stack* mem) override { _tetron::push(mem); *mem << a; }
	void  pop(_stack* mem)  override { _tetron::pop(mem); *mem >> a; }
	void  push(_wjson& b)   override { _tetron::push(b); b.add("a", a); }
	void  pop(_rjson& b)    override { _tetron::pop(b); b.read("a", a); }

	operator double* ()     override { return &a; }
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct _t_string : public _tetron
{
	std::wstring s;

	uchar type()              override { return 24; }
	int   get_froglif()       override { return 0x10; }
	void  push(_stack* mem)   override { _tetron::push(mem); *mem << s; }
	void  pop(_stack* mem)    override { _tetron::pop(mem); *mem >> s; }
	void  push(_wjson& b)     override { _tetron::push(b); b.add("s", s); }
	void  pop(_rjson& b)      override { _tetron::pop(b); b.read("s", s); }

	operator std::wstring* () override { return &s; }
	operator _t_string* ()    override { return this; }
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct _t_multi_string : public _tetron
{
	_multi_string s;

	uchar type()               override { return 3; }
	int   get_froglif()        override { return 0x90; }
	void  push(_stack* mem)    override { _tetron::push(mem); s.push(mem); }
	void  pop(_stack* mem)     override { _tetron::pop(mem); s.pop(mem); }
	void  push(_wjson& b)      override { _tetron::push(b); b.add("s", s); }
	void  pop(_rjson& b)       override { _tetron::pop(b); b.read("s", s); }

	operator _multi_string* () override { return &s; }
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct _one_tetron : public _tetron
{
	union
	{
		int64 i[16]{};
		uint64 ui[16];
		double d[16];
		wchar_t s[64];
		uchar c[128];
	};

	_one_tetron() = default;
	_one_tetron(double p1) : d{ p1 } {}
	_one_tetron(double p1, double p2) : d{ p1, p2 } {}

	uchar type()               override { return 13; }
	int   get_froglif()        override { return 0xAF; }
	void  push(_stack* mem)    override { _tetron::push(mem); mem->push_data(c, sizeof(c)); }
	void  pop(_stack* mem)     override { _tetron::pop(mem); mem->pop_data(c, sizeof(c)); }

	void  push(_wjson& b)      override;
	void  pop(_rjson& b)       override;

	operator _one_tetron* ()   override { return this; }
};
