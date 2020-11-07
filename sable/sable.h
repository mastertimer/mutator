#pragma once

#include "tetron.h"
#include "sable_stat.h"
#include "basic_oracle.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct _sable_graph : public _t_go
{
	std::vector<_basic_curve*> curve; // кривая
	i64 size_el = 6; // размер элемента

	_sable_graph();

	uchar type()      override { return 9; }
	int get_froglif() override { return 0x71; }

	void ris2(_trans tr, bool final) override;
	void run(_tetron* tt0, _tetron* tt, u64 flags) override;
	bool mouse_down_left2(_xy r) override; // начало перетаскивания
	void mouse_move_left2(_xy r) override; // процесс перетаскивания

private:
	i64 x_tani = 0; // предыдущая координата x при перетаскивании
	int v_vib  = 0; // диапазон полосы прокрутки
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

inline _basic_statistics research1; // для исследования статистики
inline _basic_statistics research2; // для исследования статистики
inline _basic_statistics research3; // для исследования статистики

void calc_all_prediction(_basic_curve &o, i64& nn, double& kk);
void fun13(_tetron* tt0, _tetron* tt, u64 flags);
void fun15(_tetron* tt0, _tetron* tt, u64 flags);
void fun16(_tetron* tt0, _tetron* tt, u64 flags);

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
