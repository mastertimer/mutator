#pragma once

#include "ui.h"
#include "exchange_basic.h"
#include <deque>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct _basic_curve2 // база для кривых
{
	virtual void draw(_bitmap& bm, i64 n, _area area) = 0; // нарисовать 1 элемент
	virtual _interval get_y(i64 n) = 0; // дипазон рисования по y
	virtual ~_basic_curve2() {}
};

struct _candle_curve2 : public _basic_curve2 // классические свечи
{
	void draw(_bitmap& bm, i64 n, _area area) override; // нарисовать 1 элемент
	_interval get_y(i64 n) override; // дипазон рисования по y
};

struct _prices_curve3 : public _basic_curve2 // посекундный спрос/предложение
{
	static const int max_part = 22000; // максимально количество элементов ss
	std::deque<_supply_and_demand> part_ss; // часть супер-статистики
	i64 begin_ss = 0; // начало куска супер-статистики

	void draw(_bitmap& bm, i64 n, _area area) override; // нарисовать 1 элемент
	_interval get_y(i64 n) override; // дипазон рисования по y
};

struct _prices_curve4 : public _basic_curve2 // посекундный спрос/предложение (цвет - дельта)
{
	static const int max_part = 22000; // максимально количество элементов ss
	std::deque<_supply_and_demand> part_ss; // часть супер-статистики
	i64 begin_ss = 0; // начало куска супер-статистики

	void draw(_bitmap& bm, i64 n, _area area) override; // нарисовать 1 элемент
	_interval get_y(i64 n) override; // дипазон рисования по y
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct _e_exchange_graph : public _ui_element
{
	std::vector<_basic_curve2*> curve; // кривая, 2я версия
	i64 size_el = 6; // размер элемента

	_e_exchange_graph(_ui* ui_);
	~_e_exchange_graph();
	void draw(_trans tr) override;
	void update() override;
	bool mouse_down_left2(_xy r) override; // начало перетаскивания
	void mouse_move_left2(_xy r) override; // процесс перетаскивания

private:
	i64 x_tani = 0; // предыдущая координата x при перетаскивании
	int v_vib = 0; // диапазон полосы прокрутки
};

inline _e_exchange_graph* graph2 = nullptr; // график

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void load_se();
void expand_elements_graph2();
void narrow_graph_elements2();
