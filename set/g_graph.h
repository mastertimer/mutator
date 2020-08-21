#pragma once

#include "t_basic_go.h"

struct _g_graph : public _t_go
{
	_g_graph() { local_area = _area_old(0, 100, 0, 100); }

	uchar type()      override { return 9; }
	int get_froglif() override { return 0xF6; } // !!!!!

	void ris2(_trans tr, bool final) override;

};