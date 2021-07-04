#pragma once

#include "tetron.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct _basic_curve // база для кривых
{
	virtual void draw(i64 n, _area area) = 0; // нарисовать 1 элемент
	virtual _interval get_y(i64 n); // дипазон рисования по y
	virtual ~_basic_curve() {}
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct _sable_graph : public _t_go
{
	std::vector<_basic_curve*> curve2; // кривая, 2я версия
	i64 size_el = 6; // размер элемента

	_sable_graph();
	~_sable_graph();

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

void calc_all_prediction(std::function<i64(i64)> o, i64& vv, double& k);
void calc_delta_price(i64 delta_minute, _basic_statistics& bs);

void start_stock();
void scan_supply_and_demand();
void save_stock_statistics();
void change_can_trade(bool);
void buy_shares();
void sell_shares();
void expand_elements_graph();
void narrow_graph_elements();

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
