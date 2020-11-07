#pragma once

/*//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

оракул

/*//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <deque>

#include "basic_oracle.h"
#include "sable_stat.h"

struct _oracle3 : public _basic_curve
{
	struct _element_oracle
	{
		int time = 0; // время
		_iinterval ncc; // диапазон цен

		ushort min = 0; // разброс по y
		ushort max = 0; // разброс по y

		bool operator < (int a) const noexcept { return (time < a); } // для алгоритма поиска по времени
	};
	static const int max_part = 22000; // максимально количество элементов ss

	std::deque<_prices> part_ss; // часть супер-статистики
	i64 begin_ss = 0; // начало куска супер-статистики
	std::vector<_element_oracle> zn; // данные

	i64  get_n()                              override { return zn.size(); } // количество элементов
	void get_n_info(i64 n, _element_chart* e) override; // получить краткую информацию n-го элемента
	void get_t_info(int t, _element_chart* e) override; // получить краткую информацию элемента со временем >= t
	void draw(i64 n, _area area)              override; // нарисовать 1 элемент
	void recovery()                           override; // выполнить
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

