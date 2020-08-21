#pragma once

#include "t_basic_go.h"

struct _g1list : public _t_go
{
	_area_old oo; // область
	bool rez1; // вывод только 1 элемента

	_g1list();

	uchar type()               override { return 2; }
	int   get_froglif()        override { return 0xF7; }
	void  push(_stack* mem)    override { _t_go::push(mem); *mem << oo; }
	void  pop(_stack* mem)     override { _t_go::pop(mem); *mem >> oo; }
	void  push(_wjson& b)      override { _t_go::push(b); b.add("oo", oo); }
	void  pop(_rjson& b)       override { _t_go::pop(b); oo = b.read_area("oo"); }

	operator _g1list* ()       override { return this; }

	void ris2(_trans tr, bool final) override;
	bool mouse_wheel2(_xy r)         override;
	void mouse_finish_move()         override;
	bool mouse_move2(_xy r)          override;

private:
	void init_sel(); // задать F_sel
	void calc_local_area(); // область только этого объекта
};
