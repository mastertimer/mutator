#pragma once

#include "basic_go.h"

struct _g_graph : public _t_go
{
	_g_graph() { local_area = { {0, 100}, {0, 100} }; }

	uchar type()      override { return 9; }
	int get_froglif() override { return 0xF6; } // !!!!!

	void ris2(_trans2 tr, bool final) override;

};