#pragma once

#include "tetron.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct _t_function : public _tetron
{
	i64 a = 0;

	_t_function() = default;
	_t_function(i64 b) : a(b) {}

	operator i64* ()         override { return &a; }
	operator _t_function* () override { return this; }

	uchar type()             override { return 32; }
	int get_froglif()        override { return 0xA0; }
	void  push(_stack* mem)  override { _tetron::push(mem); *mem << a; }
	void  pop(_stack* mem)   override { _tetron::pop(mem);  *mem >> a; }
	void  push(_wjson& b)    override { _tetron::push(b);   b.add("a", a); }
	void  pop(_rjson& b)     override { _tetron::pop(b);    b.read("a", a); }

	void run(_tetron* tt0, _tetron* tt, u64 flags) override;
};
