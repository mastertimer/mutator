#pragma once

/*//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

нервозный предсказатель старый

/*//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "basic_oracle.h"

struct _nervous_oracle : public _basic_curve // нервозный предсказатель старый
{
	struct _element_nervous // краткая информация элемента графика
	{
		time_t time    = 0; // время
		_iinterval ncc;     // диапазон цен

		ushort min_pok = 0; // минимальная покупка
		ushort max_pok = 0; // максимальная покупка
		ushort min_pro = 0; // минимальная продажа
		ushort max_pro = 0; // максимальная продажа

		int    v_r     = 0; // количество слагаемых
		double r       = 0; // средний размер
		double r_pok   = 0; // средний размер покупки
		double r_pro   = 0; // средний размер продажи

		bool operator < (time_t a) const noexcept { return (time < a); } // для алгоритма поиска по времени
	};

	std::vector<_element_nervous> zn; // данные

	i64  get_n()                              override { return zn.size(); } // количество элементов
	void get_n_info(i64 n, _element_chart* e) override; // получить краткую информацию n-го элемента
	void get_t_info(time_t t, _element_chart* e) override; // получить краткую информацию элемента со временем >= t
	void draw(i64 n, _area area)              override; // нарисовать 1 элемент
	void recovery()                           override; // выполнить
	i64  prediction()                         override; // отрезок времени роста
	void save_to_file()                       override;
	void load_from_file()                     override;

	_latest_events get_latest_events(i64 nn); // получить последние события
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

