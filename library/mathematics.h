#pragma once

#include "basic.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct _one_stat // единица статистики
{
	i64 value;   // значение / цена
	i64 number;  // количество

	bool operator!=(_one_stat a) const noexcept { return ((value != a.value) || (number != a.number)); }
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct _basic_statistics // базовая разреженная статистика с нулями
{
	std::vector<i64> data;
	i64 start = 0;

	void push(i64 x); // добавить число в статистику
	i64 number() const noexcept;
	i64 number(i64 be, i64 en) const noexcept; // количество в интервале [be,en)
	i64 operator[](i64 x) const noexcept;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct _statistics // сжатая статистика
{
	std::vector<_one_stat> data;
	typedef std::vector<_one_stat>::iterator _it;
	typedef std::vector<_one_stat>::const_iterator _cit;

	_statistics() = default;
	_statistics(const _basic_statistics& a) { *this = a; }

	void clear() { data.clear(); }
	i64 min_value() const noexcept { return (data.empty()) ? 0 : data.front().value; }
	i64 max_value() const noexcept { return (data.empty()) ? 0 : data.back().value; }
	i64 number(_cit be, _cit en) const noexcept; // количество в интервале [be,en)
	i64 number(i64 be, i64 en) const noexcept; // количество в интервале [be,en)
	i64 number() const noexcept { return number(data.begin(), data.end()); } // общее количество

	i64 first_zero(); // номер первого нулевого элемента начиная со start (-1 если не нашлось)
	i64 number_not_zero() { return data.size(); } // количество значений с ненулевым количеством
	double arithmetic_size(_it be, _it en); // арифметический размер в битах
	double arithmetic_size() { return arithmetic_size(data.begin(), data.end()); }

	void operator=(const _basic_statistics& a);
	void operator+=(const _statistics& a);
	bool operator==(const _statistics& a) const noexcept;

	i64 operator[](i64 n) const noexcept; // value -> number

	_matrix to_matrix(); // два столбца - x и y
	_matrix to_matrix(i64 mi, i64 ma); // два столбца - x и y
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

