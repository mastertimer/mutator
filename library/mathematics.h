#pragma once

#include "basic.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// нахождение всех минимумов функции fun на интервале a, (n - стартовое количество разбиений)
std::vector<double> minimum(std::function<double(double)> fun, _interval a, i64 n = 10007);

// подсчет количества минимумов
i64 number_minimum(std::function<double(double)> fun, _interval a, i64 n = 10007);

// граница между 1 и многими минимумами
double only1min(std::function<double(double, double)> fun, _interval a, _interval b, i64 na = 10007, i64 nb = 101);

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct _multi_interval //      примерно ~~      |   | |      |  |    |           ||| |    -> [)[)[)[]
{
	std::vector<double> border; // границы

	_multi_interval() = default;
	_multi_interval(const std::vector<double>& a) : border(a) {}

	i64 count() { if (border.empty()) return 0; return border.size() - 1; } // количество интервалов
	i64 find(double a); // найти номер интервала, в котором лежит число
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct _matrix;

struct matrix_column // столбец матрицы
{
	_matrix* m; // указатель на матрицу
	i64 x; // номер столбца

	bool min_max(double* mi, double* ma); // найти диапазон
//	double Delta(size_t y = 1); // разница 2-х элементов [y] - [y-1]
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct _matrix
{
	double* data = nullptr;
	_isize size;

	_matrix() = default;
	_matrix(const _matrix& a) noexcept;
	_matrix(_matrix&& a) noexcept;
	_matrix(_isize r) noexcept;
	_matrix(i64 ry, i64 rx) noexcept;
	_matrix(i64 ry, i64 rx, double z) noexcept;// заполнить числом z
	_matrix(i64 ry, i64 rx, std::function<double(i64, i64)> fun) noexcept; // задать из функции fun
	_matrix(i64 ry) noexcept; // вектор
	_matrix(i64 ry, std::function<double(i64)> fun) noexcept; // вектор задать из функции
	~_matrix() { delete[] data; }

	void operator=(const _matrix& a) noexcept;
	void operator=(_matrix&& a) noexcept;
	void operator+=(const _matrix& a) noexcept;

	void set_diagonal_matrix(i64 n, double dz) noexcept; // задать матрицу диагонального вида

	double* operator[](i64 n) { return data + (n * size.x); } // нужная строчка
	void resize(_isize r); // изменить размер матрицы ! данные не сохраняются
	bool empty() const noexcept { return size.empty(); } // проверка на нулевой размер
	matrix_column column(i64 x) { return { this, x }; } // столбец матрицы

//	void MinMax(double* mi, double* ma); // найти диапазон

//	matrix transpose() const noexcept; // транспонированная матрица
	_matrix this_mul_transpose() const noexcept; // текущая матрица умножить на транспонированную себя
	_matrix pseudoinverse() const noexcept; // псевдообратная матрица

	void FindAllEigenVectors(_matrix& R, _matrix& A) const noexcept; // собственные вектора и собственные числа
	 // линейное предсказание на основе коэффициентов
	_matrix linear_prediction(const _matrix& k, i64 start, i64 n, i64 ots = 0) const noexcept;
//	double linear_prediction(const matrix& k) const noexcept; // линейное предсказание на основе коэффициентов

	_matrix operator<<(const _matrix& a) const noexcept; // склеить матрицы (количество строк должно быть одинаковым)
	_matrix operator*(const _matrix& a) const noexcept;
	_matrix operator-(const _matrix& a) const noexcept;

//	void Push(_stack* mem); // сохранить в стек
//	void Pop(_stack* mem); // извлечь из стека
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

_matrix get_noise(i64 n, double k); // сгенерировать шум

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
	void clear() noexcept { data.clear(); }
	i64 number() const noexcept;
	i64 number(i64 be, i64 en) const noexcept; // количество в интервале [be,en)
	i64 operator[](i64 x) const noexcept;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct _interval_statistics // ограниченная интервальная статистика
{
	struct _element_is // элемент интервальной статистики
	{
		_interval interval; // интервал диапазона
		double mean = 0; // среднее значение
		i64 number = 0; // количество чисел в этом диапазоне
	};
	
	std::vector<_element_is> element;

	void init_equiprobable(_basic_statistics &bs, i64 n, double k); // инициализировать равновероятным
	double min_mean(); // левая граница первого элемента
	double max_mean(); // правая граница последнего элемента
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

