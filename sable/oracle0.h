#pragma once

/*//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

классические минутные свечи
без предсказания

/*//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "basic_oracle.h"

struct _mctds_candle : public _basic_curve // источник данных для временного графика - классические свечи
{
	struct _cen_pak // свечка
	{
		ushort min   = 0; // минимальная цена
		ushort max   = 0; // макимальная цена
		ushort first = 0; // первая цена
		ushort last  = 0; // последняя цена
		double cc    = 0; // средняя цена
		_iinterval ncc;   // диапазон цен
		int time     = 0; // общее время

		bool operator < (time_t a) const noexcept { return (time < a); } // для алгоритма поиска по времени
	};

	std::vector<_cen_pak> cen1m; // упакованные цены по минутам

	i64  get_n()                              override { return cen1m.size(); } // количество элементов
	void get_n_info(i64 n, _element_chart* e) override; // получить краткую информацию n-го элемента
	void get_t_info(time_t t, _element_chart* e) override; // получить краткую информацию элемента со временем >= t
	void draw(i64 n, _area area)              override; // нарисовать 1 элемент
	void recovery()                           override; // обновить
	void save_to_file()                       override;
	void load_from_file()                     override;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

