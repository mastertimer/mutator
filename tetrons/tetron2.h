#pragma once

#include "tetron.h"
#include "mathematics.h"

struct _curve
{
	_matrix     a;             // данные - 1 или 2 стобца
	std::string caption;       // подпись линии
	bool        bar   = false; // гистограмма
	double      width = 1.5;   // толщина линии
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct _g_graph : public _t_go
{
	std::vector<_curve> curve;

	_g_graph() { local_area = { {0, 100}, {0, 100} }; }

	uchar type() { return 13; }
	int get_froglif() { return 0xFC; }
	void ris2(_trans tr, bool final) override;

	void add(const _matrix& b, std::string_view s = "", bool bar = false);
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

