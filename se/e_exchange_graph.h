#pragma once

#include "ui.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct _basic_curve2 // база для кривых
{
	virtual void draw(_bitmap& bm, i64 n, _area area) = 0; // нарисовать 1 элемент
	virtual _interval get_y(i64 n) = 0; // дипазон рисования по y
	virtual ~_basic_curve2() {}
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct _e_exchange_graph : public _ui_element
{
	std::vector<_basic_curve2*> curve; // кривая, 2я версия
	i64 size_el = 6; // размер элемента

	_e_exchange_graph(_ui* ui_);
	~_e_exchange_graph();
	void ris2(_trans tr) override;
	void update() override;
	bool mouse_down_left2(_xy r) override; // начало перетаскивания
	void mouse_move_left2(_xy r) override; // процесс перетаскивания

private:
	i64 x_tani = 0; // предыдущая координата x при перетаскивании
	int v_vib = 0; // диапазон полосы прокрутки
};

inline _e_exchange_graph* graph2 = nullptr; // график

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void start_se();
void expand_elements_graph2();
void narrow_graph_elements2();
