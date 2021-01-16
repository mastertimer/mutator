#pragma once

#include "tetron.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct _molecule
{
	_id tetron; // тетрон
	_id tr_t;
};

struct _view_tetron : public _t_go
{
	std::wstring hint;
	std::vector<_molecule> mo;

	_view_tetron() { local_area = { {0, 24}, {0, 24} }; }
	uchar type() { return 18; }
	int get_froglif() { return 0xFF; }
	void ris2(_trans tr, bool final) override;
	double final_radius()  override { return 21.0; } // минимальный полуразмер, после которого не рисуется структура
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
