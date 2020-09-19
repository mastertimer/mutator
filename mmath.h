#pragma once
#include <functional>

#include "mgeneral.h"

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct matrix;

struct MatrixColumn                                                       // столбец матрицы
{
	matrix* m_;                                                           // указатель на матрицу
	size_t x_;                                                            // номер столбца

	void MinMax(double* mi, double* ma);                                  // найти диапазон
	double Delta(size_t y = 1);                                           // разница 2-х элементов [y] - [y-1]
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct matrix
{
	friend MatrixColumn;

	matrix() = default;
	matrix(const matrix& a) noexcept;                                     // конструктор копии
	matrix(matrix&& a) noexcept;                                          // конструктор переноса
	matrix(size_t ry, size_t rx) noexcept;                                // сразу нужный размер матрицы
	matrix(size_t ry, size_t rx, double z) noexcept;                      // сразу нужный размер матрицы и заполнить числом
	matrix(size_t ry, size_t rx, const std::function<double(size_t, size_t)> &fun ) noexcept; // сразу нужный размер матрицы и задать из функции
	matrix(size_t ry) noexcept;                                           // сразу нужный размер вектора
	matrix(size_t ry, const std::function<double(size_t)>& fun) noexcept; // сразу нужный размер вектора и задать из функции
	~matrix();

	void operator=(const matrix& a) noexcept;                             // присваивание
	void operator=(matrix&& a) noexcept;                                  // присваивание переносом
	void operator+=(const matrix& a) noexcept;                            // добавить матрицу

	void set_diagonal_matrix(size_t n, double dz) noexcept;               // задать матрицу диагонального вида

	inline double* operator[](size_t n) { return data_ + (n * rx_); }     // нужная строчка
	void resize(size_t ry, size_t rx);                                    // изменить размер марицы ! данные не сохраняются
	inline bool empty() const noexcept { return rx_ * ry_ == 0; }         // проверка на нулевой размер
	inline size_t size_x() const noexcept { return rx_; }                 // размер по x
	inline size_t size_y() const noexcept { return ry_; }                 // размер по y
	inline size_t size() const noexcept { return ry_ * rx_; }             // размер
	inline MatrixColumn Column(size_t x) { return { this, x }; }          // столбец матрицы

	void MinMax(double* mi, double* ma);                                  // найти диапазон

	matrix transpose() const noexcept;                                    // транспонированная матрица
	matrix this_mul_transpose() const noexcept;                           // текущая матрица умножить на транспонированную себя
	matrix pseudoinverse() const noexcept;                                // псевдообратная матрица

	void FindAllEigenVectors(matrix& R, matrix& A) const noexcept;        // собственные вектора и собственные числа
	matrix linear_prediction(const matrix &k, size_t start, size_t n, size_t ots = 0) const noexcept; // линейчное предсказание на основе коэффициентов
	double linear_prediction(const matrix& k) const noexcept;             // линейчное предсказание на основе коэффициентов

	matrix operator<<(const matrix& a) const noexcept;                    // склеить матрицы (количество строк должно быть одинаковым)
	matrix operator*(const matrix& a) const noexcept;                     // умножение матриц
	matrix operator-(const matrix& a) const noexcept;                     // разность матриц

	void Push(Stack* mem);                                                // сохранить в стек
	void Pop(Stack* mem);                                                 // извлечь из стека

private:
	double* data_ = nullptr;                                              // данные
	size_t ry_ = 0;                                                       // количество строк
	size_t rx_ = 0;                                                       // количество столбцов
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
matrix get_noise(size_t n, double k);                      // сгенерировать шум
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
