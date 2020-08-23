#pragma once

#include "basic_go.h"

struct _g_color_ring : public _t_go
{
	_hsva color = c_def;

	_g_color_ring() { local_area = _area_old(0, 300, 0, 300); }
	uchar type() { return 17; }
	int get_froglif() { return 0x72; }
	void ris2(_trans tr, bool final) override;
	bool mouse_down_left2(_xy r)  override { change(r, true); return true; }
	void mouse_move_left2(_xy r) override { change(r, false); }
	void mouse_up_left2(_xy r)  override {}

private:
	static constexpr double li_r_1 = 0.35;
	static constexpr double li_r_2 = 0.55;
	static constexpr double zazor = (2 * 3.14159265358979323846) * 0.02;
	int iii = 0; // номер четверти при перетаскивании
	void change(_xy r, bool start);
};
