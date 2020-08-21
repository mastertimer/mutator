#pragma once

#include "t_basic_go.h"

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
	void  mouse_up_left2(_xy r)      override { cha_area(); }
	void  mouse_move_left2(_xy r)    override {}

	void ris2(_trans tr, bool final) override;
	bool mouse_move2(_xy r)          override;
	void mouse_finish_move()         override;
	bool mouse_down_left2(_xy r)     override;
	void push(_stack* mem)           override;
	void pop(_stack* mem)            override;
	void push(_wjson& b)             override;
	void pop(_rjson& b)              override;

	void RisIco(const char* kod, const char* s);
};

