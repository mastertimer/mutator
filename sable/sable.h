#pragma once

#include <deque>

#include "tetron.h"
#include "sable_stat.h"
#include "basic_oracle.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

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
	};

	std::vector<_cen_pak> cen1m; // упакованные цены по минутам

	i64 get_n()                               override { return cen1m.size(); } // количество элементов
	void get_n_info(i64 n, _element_chart* e) override; // получить краткую информацию n-го элемента
	void get_t_info(int t, _element_chart* e) override; // получить краткую информацию элемента со временем >= t
	void draw(i64 n, _area area)              override; // нарисовать 1 элемент
	void recovery()                           override; // обновить
	void push(_stack* mem);
	void pop(_stack* mem);
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct _view_stat : public _basic_curve // тестовое отображение статистических данных
{
	struct _cen_pak     // данные по минуте
	{
		int time   = 0; // общее время
		_iinterval ncc; // диапазон цен
		ushort min = 0; // минимальная цена
		ushort max = 0; // макимальная цена
		int k = 0;      // количество совпадений

		bool operator < (int a) const noexcept { return (time < a); } // для алгоритма поиска по времени
	};

	std::vector<_cen_pak> cen1m; // упакованные цены по минутам

	i64 get_n()                               override { return cen1m.size(); } // количество элементов
	void get_n_info(i64 n, _element_chart* e) override; // получить краткую информацию n-го элемента
	void get_t_info(int t, _element_chart* e) override; // получить краткую информацию элемента со временем >= t
	void draw(i64 n, _area area)              override; // нарисовать 1 элемент
	void recovery()                           override; // обновить
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct _latest_events // последние события
{
	char event[4]; // [0] - последнее событие
	int minute[4]; // на какой минуте случилось
	double   x[4]; // значения

	int  start();  // ща будет рост в X минут
	bool stop();   // ща будет падение
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

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

		bool operator < (int a) const noexcept { return (time < a); } // для алгоритма поиска по времени
	};

	std::vector<_element_nervous> zn; // данные

	i64 get_n() override { return zn.size(); } // количество элементов
	void get_n_info(i64 n, _element_chart* e) override; // получить краткую информацию n-го элемента
	void get_t_info(int t, _element_chart* e) override; // получить краткую информацию элемента со временем >= t
	void draw(i64 n, _area area)              override; // нарисовать 1 элемент
	void recovery() override; // выполнить
	void push(_stack* mem);
	void pop(_stack* mem);
	_latest_events get_latest_events(i64 nn); // получить последние события
	i64 prediction() override;                // отрезок времени роста
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct _nervous_oracle2 : public _basic_curve // нервозный предсказатель новый
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

		bool operator < (int a) const noexcept { return (time < a); } // для алгоритма поиска по времени
	};

	std::vector<_element_nervous> zn; // данные

	i64 get_n() override { return zn.size(); } // количество элементов
	void get_n_info(i64 n, _element_chart* e) override; // получить краткую информацию n-го элемента
	void get_t_info(int t, _element_chart* e) override; // получить краткую информацию элемента со временем >= t
	void draw(i64 n, _area area)              override; // нарисовать 1 элемент
	void recovery() override; // выполнить
	void push(_stack* mem);
	void pop(_stack* mem);
	_latest_events get_latest_events(i64 nn); // получить последние события
	i64 prediction() override;                // отрезок времени роста
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct _oracle3 : public _basic_curve // оракул 3-я версия
{
	struct _element_oracle
	{
		int time   = 0; // время
		_iinterval ncc; // диапазон цен

		ushort min = 0; // разброс по y
		ushort max = 0; // разброс по y

		bool operator < (int a) const noexcept { return (time < a); } // для алгоритма поиска по времени
	};
	static const int max_part = 22000; // максимально количество элементов ss

	std::deque<_prices> part_ss; // часть супер-статистики
	i64 begin_ss = 0; // начало куска супер-статистики
	std::vector<_element_oracle> zn; // данные

	i64 get_n() override { return zn.size(); } // количество элементов
	void get_n_info(i64 n, _element_chart* e) override; // получить краткую информацию n-го элемента
	void get_t_info(int t, _element_chart* e) override; // получить краткую информацию элемента со временем >= t
	void draw(i64 n, _area area)              override; // нарисовать 1 элемент
	void recovery() override; // выполнить
};

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
