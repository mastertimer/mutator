#pragma once

#include "t_basic_go.h"

struct _g_scrollbar : public _t_go
{
	double position = 0; // положение стрелки [0-1]
	char   vid      = 0; // 0 - горизонтальный 1 - вертикальный 2 - снизу 3 - справа 4 - сверху 5 - слева

	_g_scrollbar()                         { local_area = _area_old(0, 10, 0, 10); }
	~_g_scrollbar()                        { calc_area(); }

	operator double* ()           override { return &position; }
	operator _g_scrollbar* ()     override { return this; }

	uchar type()                  override { return 16; }
	int   get_froglif()           override { return 0xF8; }
	void  push(_stack* mem)       override { _t_go::push(mem); *mem << position << vid; }
	void  pop(_stack* mem)        override { _t_go::pop(mem);	*mem >> position >> vid; }
	bool  mouse_down_left2(_xy r) override { mouse_move_left2(r); return true; }
	void  mouse_up_left2(_xy r)   override {}

	void  push(_wjson& b)              override;
	void  pop(_rjson& b)               override;
	void  mouse_move_left2(_xy r)      override;
	void  ris2(_trans tr, bool final)  override;
	void  after_create_link(_link* li) override;

	void  prilip(_t_go* r); // прилипание к графическому объекту
};
