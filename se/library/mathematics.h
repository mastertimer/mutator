#pragma once

#include "geometry.h"

#include <functional>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// нахождение всех минимумов функции fun на интервале a, (n - стартовое количество разбиений)
std::vector<double> minimum(std::function<double(double)> fun, _interval a, i64 n = 10007);

// подсчет количества минимумов
i64 number_minimum(std::function<double(double)> fun, _interval a, i64 n = 10007);

// граница между 1 и многими минимумами
double only1min(std::function<double(double, double)> fun, _interval a, _interval b, i64 na = 10007, i64 nb = 101);

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

