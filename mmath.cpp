#include <string>
#pragma hdrstop
#include "mmath.h"

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void matrix::operator=(matrix&& a) noexcept
{
	if (data_) delete[] data_;
	data_ = a.data_;
	ry_ = a.ry_;
	rx_ = a.rx_;
	a.data_ = nullptr;
	a.ry_ = 0;
}

void matrix::operator=(const matrix& a) noexcept
{
	resize(a.ry_, a.rx_);
	memcpy(data_, a.data_, ry_ * rx_ * sizeof(double));
}

matrix::matrix(matrix&& a) noexcept : data_(a.data_), ry_(a.ry_), rx_(a.rx_)
{
	a.data_ = nullptr;
	a.ry_ = 0;
}

matrix::matrix(const matrix& a) noexcept : ry_(a.ry_), rx_(a.rx_)
{
	if (ry_ * rx_ == 0) return;
	data_ = new double[ry_ * rx_];
	memcpy(data_, a.data_, ry_ * rx_ * sizeof(double));
}

matrix matrix::operator<<(const matrix& a) const noexcept
{
	if (a.empty()) return *this;
	if (empty()) return a;
	if (ry_ != a.ry_) return matrix();
	matrix b(ry_, rx_ + a.rx_);
	double* rez = b.data_;
	double* m1 = data_;
	double* m2 = a.data_;
	for (int j = 0; j < ry_; j++)
	{
		for (int i = 0; i < rx_; i++, rez++, m1++) *rez = *m1;
		for (int i = 0; i < a.rx_; i++, rez++, m2++)* rez = *m2;
	}
	return b;
}

matrix matrix::transpose() const noexcept
{
	matrix b(rx_, ry_);
	double* rez = b.data_;
	for (size_t j = 0; j < rx_; j++)
	{
		double* ist = data_ + j;
		for (size_t i = 0; i < ry_; i++, rez++, ist += rx_)	rez = ist;
	}
	return b;
}

matrix matrix::this_mul_transpose() const noexcept
{
	matrix b(ry_, ry_);
	for (size_t j = 0; j < ry_; j++)
		for (size_t i = j; i < ry_; i++)
		{
			double s = 0;
			double* a1 = data_ + (j * rx_);
			double* a2 = data_ + (i * rx_);
			for (size_t k = 0; k < rx_; k++, a1++, a2++) s += (*a1) * (*a2);
			b[j][i] = b[i][j] = s;
		}
	return b;
}

matrix matrix::operator*(const matrix& a) const noexcept
{
	if (rx_ != a.ry_) return matrix();
	matrix b(ry_, a.rx_);
	for (size_t j = 0; j < b.ry_; j++)
		for (size_t i = 0; i < b.rx_; i++)
		{
			double s = 0;
			double* a1 = data_ + (j * rx_);
			double* a2 = a.data_ + i;
			for (size_t k = 0; k < rx_; k++)
			{
				s += (*a1) * (*a2);
				a1++;
				a2 += a.rx_;
			}
			b[j][i] = s;
		}
	return b;
}

matrix matrix::operator-(const matrix& a) const noexcept
{
	if ((rx_ != a.rx_) || (ry_ != a.ry_)) return matrix();
	matrix b(ry_, rx_);
	size_t n = rx_ * ry_;
	for (size_t i = 0; i < n; i++) b.data_[i] = data_[i] - a.data_[i];
	return b;
}

void matrix::operator+=(const matrix& a) noexcept
{
	if ((rx_ != a.rx_) || (ry_ != a.ry_)) return;
	size_t n = rx_ * ry_;
	for (size_t i = 0; i < n; i++) data_[i] += a.data_[i];
}

matrix::matrix(size_t ry, size_t rx, double z) noexcept : ry_(ry), rx_(rx)
{
	size_t n = ry_ * rx_;
	if (n == 0) return;
	data_ = new double[n];
	for (size_t i = 0; i < n; i++) data_[i] = z;
}

matrix::matrix(size_t ry, size_t rx, const std::function<double(size_t, size_t)>& fun) noexcept : ry_(ry), rx_(rx)
{
	if (ry_ * rx_ == 0) return;
	data_ = new double[ry_ * rx_];
	int n = 0;
	for (size_t j = 0; j < ry_; j++)
		for (size_t i = 0; i < rx_; i++, n++)
			data_[n] = fun(j, i);
}

matrix::matrix(size_t ry, const std::function<double(size_t)>& fun)  noexcept : ry_(ry), rx_(1)
{
	if (ry_ == 0) return;
	data_ = new double[ry_];
	for (size_t i = 0; i < ry_; i++) data_[i] = fun(i);
}

matrix::matrix(size_t ry) noexcept : ry_(ry), rx_(1)
{
	if (ry_) data_ = new double[ry_];
}

matrix::matrix(size_t ry, size_t rx)  noexcept : ry_(ry), rx_(rx)
{
	if (ry_ * rx_) data_ = new double[ry_ * rx_];
}

matrix::~matrix()
{
	if (data_) delete[] data_;
}

void matrix::resize(size_t ry, size_t rx)
{
	if (rx * ry > rx_ * ry_)
	{
		if (data_) delete[] data_;
		data_ = new double[rx * ry];
	}
	ry_ = ry;
	rx_ = rx;
}

void matrix::MinMax(double* mi, double* ma)
{
	size_t vdata = rx_ * ry_;
	if (vdata == 0)
	{
		*mi = 1.0;
		*ma = 0;
		return;
	}
	double mii = data_[0];
	double maa = data_[0];
	for (int i = 1; i < vdata; i++)
	{
		if (data_[i] < mii) mii = data_[i];
		if (data_[i] > maa) maa = data_[i];
	}
	*mi = mii;
	*ma = maa;
}

void matrix::Push(Stack* mem)
{
	mem->Push(rx_);
	mem->Push(ry_);
	mem->PushData(data_, sizeof(double) * rx_ * ry_);
}

void matrix::Pop(Stack* mem)
{
	size_t rx, ry;
	mem->Pop(rx);
	mem->Pop(ry);
	resize(ry, rx);
	mem->PopData(data_, sizeof(double) * rx_ * ry_);
}

void matrix::set_diagonal_matrix(size_t n, double dz) noexcept
{
	resize(n, n);
	memset(data_, 0, n * n * sizeof(double));
	for (size_t i = 0; i < n; i++) data_[i * n + i] = dz;
}

void matrix::FindAllEigenVectors(matrix& R, matrix& A) const noexcept
{
	// Сборник научных программ на Фортране. Вып. 2. Матричная алгебра и линейная
	// алгебра. Нью-Йорк, 1960-1971, пер. с англ. (США). М., "Статистика", 1974.
	// 224 с.
	// стр - 187, подпрограмма EIGEN
	double ANORM, ANRMX, THR, X, Y, SINX, SINX2, COSX, COSX2, SINCS;
	constexpr double RANGE = 1.0E-13;                                     // было -13
//	constexpr double LIM = 1.0E-11;                                       // -6
	A = *this;
	size_t N = A.rx_;
	R.set_diagonal_matrix(N, 1.0);
	ANORM = 0;
	for (int j = 0; j < N; j++)
		for (int i = j + 1; i < N; i++)
			ANORM += A[j][i] * A[j][i];
	if (ANORM > 0)
	{
		ANORM = sqrt(2 * ANORM);
		ANRMX = ANORM * RANGE / N;
		THR = ANORM;
		do
		{
			THR = THR / N;
			bool ind;
			do
			{
				ind = false;
				for (size_t L = 0; L < N - 1; L++)
					for (size_t M = L + 1; M < N; M++)
						if (abs(A[L][M]) >= THR)
						{
							ind = true;
							X = 0.5 * (A[L][L] - A[M][M]);
							Y = -A[L][M] / sqrt(A[L][M] * A[L][M] + X * X);
							if (X < 0) Y = -Y;
							SINX = Y / sqrt(2 * (1 + sqrt(1 - Y * Y)));
							SINX2 = SINX * SINX;
							COSX = sqrt(1 - SINX2);
							COSX2 = COSX * COSX;
							SINCS = SINX * COSX;
							for (size_t i = 0; i < N; i++)
							{
								if ((i != L) && (i != M))
								{
									X = A[i][L] * COSX - A[i][M] * SINX;
									A[M][i] = A[i][M] = A[i][L] * SINX + A[i][M] * COSX;
									A[L][i] = A[i][L] = X;
								}
								X = R[i][L] * COSX - R[i][M] * SINX;
								R[i][M] = R[i][L] * SINX + R[i][M] * COSX;
								R[i][L] = X;
							}
							X = 2 * A[L][M] * SINCS;
							Y = A[L][L] * COSX2 + A[M][M] * SINX2 - X;
							X = A[L][L] * SINX2 + A[M][M] * COSX2 + X;
							A[M][L] = A[L][M] = (A[L][L] - A[M][M]) * SINCS + A[L][M] * (COSX2 - SINX2);
							A[L][L] = Y;
							A[M][M] = X;
						}
			} while (ind);
		} while (THR > ANRMX);
	}
/*	for (int i = 0; i < N; i++)                                                 
		for (int j = i + 1; j < N; j++)
			if (A[i][i] < A[j][j])
			{
				X = A[i][i];
				A[i][i] = A[j][j];
				A[j][j] = X;
				R.PerestCol(i, j);
			}

	// конец подпрограммы EIGEN

	for (int j = 0; j < N; j++)
		for (int i = 0; i < N; i++)
			if (i != j) A[j][i] = 0;
	double SumD = Abs().shpoor();//   /N
	double SumL = abs(A[0][0]);
	for (int i = 1; i < N; i++)
	{
		SumL += abs(A[i][i]);
		//    if ((::Abs(A[i][i]/SumD)) < 1.0E-8)
		if (abs(1.0 - SumL / SumD) < LIM)
			//    if (A[i][i] <= 0)
		{
			Vl = i - 1;
			A.ry = i - 1;
			A.rx = i - 1;
			R.rx = i - 1;
			return;
		}
	}
	Vl = rx_;*/
}

//вычисление псевдообратной матрицы
matrix matrix::pseudoinverse() const noexcept
{
	double LIM = 1.0E-13;                                       // min -10 max -15  было -11 -13
	matrix BQ, BL;
	FindAllEigenVectors(BQ, BL);
	double SumD = 0;
	for (size_t i = 0; i < ry_; i++) 
		if (BL[i][i] > SumD) SumD = BL[i][i];
	LIM *= SumD;
	matrix Aplus(ry_, ry_, 0);
	for (size_t j = 0; j < ry_; j++)
	{
		double lam = BL[j][j];
		if (lam < LIM) continue;
		lam = 1.0 / lam;
		double* rez = Aplus.data_;
		double* ist = BQ.data_ + j;
		for (size_t y = 0; y < ry_; y++)
		{
			double mn = lam * ist[y * ry_];
			for (size_t x = 0; x < ry_; x++, rez++)
				* rez += mn * ist[x * ry_];
		}
	}
	return Aplus;
}

double matrix::linear_prediction(const matrix& k) const noexcept
{
	size_t rk = k.size();
	size_t r = size();
	if (r * rk == 0) return 0.0;
	if (rk > r) return data_[r-1];
	r -= rk;
	double s = 0.0;
	for (size_t ii = 0; ii < rk; ii++) s += k.data_[ii] * data_[ii + r];
	return s;
}

matrix matrix::linear_prediction(const matrix& k, size_t start, size_t n, size_t ots) const noexcept
{
	matrix b(n, 1, 0);
	if (((k.rx_ != 1) && (k.ry_ != 1)) || ((rx_ != 1) && (ry_ != 1))) return b; // должен быть вектор
	size_t rk = k.size();
	size_t r = size();
	if ((start < rk) || (start > r)) return b;
	for (size_t i = 0; i < n; i++)
	{
		double s = 0;
		for (size_t ii = 0; ii < rk; ii++)
		{
			__int64 p = (__int64)ii - (__int64)rk + (__int64)i - (__int64)ots;
			s += k.data_[ii] * ((p >= 0) ? b.data_[p] : data_[start + p]);
		}
		b.data_[i] = s;
	}
	return b;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

double MatrixColumn::Delta(size_t y)
{
	if ((x_ < 0) || (x_ >= m_->rx_) || (y < 1) || (y >= m_->ry_)) return 0.0;
	return m_->data_[y * (m_->rx_) + x_] - m_->data_[(y - 1) * (m_->rx_) + x_];
}

void MatrixColumn::MinMax(double* mi, double* ma)
{
	if ((x_ < 0) || (x_ >= m_->rx_))
	{
		*mi = 1.0;
		*ma = 0;
		return;
	}
	double* da = m_->data_;
	double mii = da[x_];
	double maa = da[x_];
	size_t vdata = m_->rx_ * m_->ry_;
	for (size_t i = x_ + m_->rx_; i < vdata; i += m_->rx_)
	{
		if (da[i] < mii) mii = da[i];
		if (da[i] > maa) maa = da[i];
	}
	*mi = mii;
	*ma = maa;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
extern PRNG prng;

matrix get_noise(size_t n, double k)
{

	matrix b(n);
	for (size_t i = 0; i < n; i++)
		b[0][i] = (((double)prng.Random()/(double)(unsigned __int64)0xFFFFFFFFFFFFFFFF) - 0.5) * k;
	return b;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
