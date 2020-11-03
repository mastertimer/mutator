#include "mathematics.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void _matrix::operator=(_matrix&& a) noexcept
{
	if (data) delete[] data;
	data = a.data;
	size = a.size;
	a.data = nullptr;
	a.size = {};
}

void _matrix::operator=(const _matrix& a) noexcept
{
	resize(a.size);
	memcpy(data, a.data, size.square() * sizeof(double));
}

_matrix::_matrix(_matrix&& a) noexcept : data(a.data), size(a.size)
{
	a.data = nullptr;
	a.size = {};
}

_matrix::_matrix(const _matrix& a) noexcept : size(a.size)
{
	if (size.empty()) return;
	data = new double[size.square()];
	memcpy(data, a.data, size.square() * sizeof(double));
}

/*matrix matrix::operator<<(const matrix& a) const noexcept
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
		for (int i = 0; i < a.rx_; i++, rez++, m2++)*rez = *m2;
	}
	return b;
}*/

/*matrix matrix::transpose() const noexcept
{
	matrix b(rx_, ry_);
	double* rez = b.data_;
	for (size_t j = 0; j < rx_; j++)
	{
		double* ist = data_ + j;
		for (size_t i = 0; i < ry_; i++, rez++, ist += rx_)	rez = ist;
	}
	return b;
}*/

/*matrix matrix::this_mul_transpose() const noexcept
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
}*/

/*matrix matrix::operator*(const matrix& a) const noexcept
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
}*/

/*matrix matrix::operator-(const matrix& a) const noexcept
{
	if ((rx_ != a.rx_) || (ry_ != a.ry_)) return matrix();
	matrix b(ry_, rx_);
	size_t n = rx_ * ry_;
	for (size_t i = 0; i < n; i++) b.data_[i] = data_[i] - a.data_[i];
	return b;
}*/

/*void matrix::operator+=(const matrix& a) noexcept
{
	if ((rx_ != a.rx_) || (ry_ != a.ry_)) return;
	size_t n = rx_ * ry_;
	for (size_t i = 0; i < n; i++) data_[i] += a.data_[i];
}*/

/*matrix::matrix(size_t ry, size_t rx, double z) noexcept : ry_(ry), rx_(rx)
{
	size_t n = ry_ * rx_;
	if (n == 0) return;
	data_ = new double[n];
	for (size_t i = 0; i < n; i++) data_[i] = z;
}*/

/*matrix::matrix(size_t ry, size_t rx, const std::function<double(size_t, size_t)>& fun) noexcept : ry_(ry), rx_(rx)
{
	if (ry_ * rx_ == 0) return;
	data_ = new double[ry_ * rx_];
	int n = 0;
	for (size_t j = 0; j < ry_; j++)
		for (size_t i = 0; i < rx_; i++, n++)
			data_[n] = fun(j, i);
}*/

_matrix::_matrix(i64 ry, const std::function<double(i64)>& fun)  noexcept : size{ 1, ry }
{
	if (ry == 0) return;
	data = new double[ry];
	for (i64 i = 0; i < ry; i++) data[i] = fun(i);
}

/*matrix::matrix(size_t ry) noexcept : ry_(ry), rx_(1)
{
	if (ry_) data_ = new double[ry_];
}*/

_matrix::_matrix(i64 ry, i64 rx)  noexcept : size{ rx,ry }
{
	if (!size.empty()) data = new double[size.square()];
}

void _matrix::resize(_isize r)
{
	if (r.square() > size.square())
	{
		delete[] data;
		data = new double[r.square()];
	}
	size = r;
}

/*void matrix::MinMax(double* mi, double* ma)
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
}*/

/*void matrix::Push(_stack* mem)
{
	*mem << rx_ << ry_;
	mem->push_data(data_, sizeof(double) * rx_ * ry_);
}*/

/*void matrix::Pop(_stack* mem)
{
	size_t rx, ry;
	*mem >> rx >> ry;
	resize(ry, rx);
	mem->pop_data(data_, sizeof(double) * rx_ * ry_);
}*/

/*void matrix::set_diagonal_matrix(size_t n, double dz) noexcept
{
	resize(n, n);
	memset(data_, 0, n * n * sizeof(double));
	for (size_t i = 0; i < n; i++) data_[i * n + i] = dz;
}*/

/*void matrix::FindAllEigenVectors(matrix& R, matrix& A) const noexcept
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
		//for (int i = 0; i < N; i++)
		//	for (int j = i + 1; j < N; j++)
		//		if (A[i][i] < A[j][j])
		//		{
		//			X = A[i][i];
		//			A[i][i] = A[j][j];
		//			A[j][j] = X;
		//			R.PerestCol(i, j);
		//		}

		//// конец подпрограммы EIGEN

		//for (int j = 0; j < N; j++)
		//	for (int i = 0; i < N; i++)
		//		if (i != j) A[j][i] = 0;
		//double SumD = Abs().shpoor();//   /N
		//double SumL = abs(A[0][0]);
		//for (int i = 1; i < N; i++)
		//{
		//	SumL += abs(A[i][i]);
		//	//    if ((::Abs(A[i][i]/SumD)) < 1.0E-8)
		//	if (abs(1.0 - SumL / SumD) < LIM)
		//		//    if (A[i][i] <= 0)
		//	{
		//		Vl = i - 1;
		//		A.ry = i - 1;
		//		A.rx = i - 1;
		//		R.rx = i - 1;
		//		return;
		//	}
		//}
		//Vl = rx_;
}*/

//вычисление псевдообратной матрицы
/*matrix matrix::pseudoinverse() const noexcept
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
				*rez += mn * ist[x * ry_];
		}
	}
	return Aplus;
}*/

/*double matrix::linear_prediction(const matrix& k) const noexcept
{
	size_t rk = k.size();
	size_t r = size();
	if (r * rk == 0) return 0.0;
	if (rk > r) return data_[r - 1];
	r -= rk;
	double s = 0.0;
	for (size_t ii = 0; ii < rk; ii++) s += k.data_[ii] * data_[ii + r];
	return s;
}*/

/*matrix matrix::linear_prediction(const matrix& k, size_t start, size_t n, size_t ots) const noexcept
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
}*/

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/*double MatrixColumn::Delta(size_t y)
{
	if ((x_ < 0) || (x_ >= m_->rx_) || (y < 1) || (y >= m_->ry_)) return 0.0;
	return m_->data_[y * (m_->rx_) + x_] - m_->data_[(y - 1) * (m_->rx_) + x_];
}*/

bool matrix_column::min_max(double* mi, double* ma)
{
	if ((x < 0) || (x >= m->size.x) || (m->size.y < 1)) return false;
	double* da = m->data;
	double mii = da[x];
	double maa = da[x];
	i64 vdata = m->size.square();
	for (i64 i = x + m->size.x; i < vdata; i += m->size.x)
	{
		if (da[i] < mii) mii = da[i];
		if (da[i] > maa) maa = da[i];
	}
	*mi = mii;
	*ma = maa;
	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/*matrix get_noise(size_t n, double k)
{

	matrix b(n);
	for (size_t i = 0; i < n; i++)
		b[0][i] = (((double)rnd() / (double)(unsigned __int64)0xFFFFFFFFFFFFFFFF) - 0.5) * k;
	return b;
}*/

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void _basic_statistics::push(i64 x)
{
	if (x - start >= (i64)data.size()) data.resize(x - start + 1, 0);
	if (x < start) { data.insert(data.begin(), start - x, 0); start = x; }
	data[x - start]++;
}

i64 _basic_statistics::number() const noexcept
{
	i64 s = 0;
	for (auto i : data) s += i;
	return s;
}

i64 _basic_statistics::number(i64 be, i64 en) const noexcept
{
	be -= start;
	en -= start;
	if (be < 0) be = 0;
	if (en > (i64)data.size()) en = (i64)data.size();
	i64 s = 0;
	for (i64 i = be; i < en; i++) s += data[i];
	return s;
}

i64 _basic_statistics::operator[](i64 x) const noexcept
{
	if (x - start >= (i64)data.size()) return 0;
	if (x < start) return 0;
	return data[x - start];
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

i64 _statistics::number(i64 be, i64 en) const noexcept
{
	auto li1 = std::lower_bound(data.begin(), data.end(), be, [](_one_stat a, i64 b) { return (a.value < b); });
	auto li2 = std::lower_bound(data.begin(), data.end(), en, [](_one_stat a, i64 b) { return (a.value < b); });
	return number(li1, li2);
}

i64 _statistics::number(_cit be, _cit en) const noexcept
{
	i64 s = 0;
	for (auto i = be; i != en; ++i) s += i->number;
	return s;
}

bool _statistics::operator==(const _statistics& a) const noexcept
{
	if (data.size() != a.data.size()) return false;
	for (i64 i = 0; i < (i64)data.size(); i++)
		if (data[i] != a.data[i]) return false;
	return true;
}

void _statistics::operator+=(const _statistics& a)
{
	_statistics res;
	auto i = data.begin();
	auto ai = a.data.begin();
	while ((i != data.end()) && (ai != a.data.end()))
	{
		if (i->value < ai->value)
		{
			res.data.push_back(*i);
			++i;
			continue;
		}
		if (ai->value < i->value)
		{
			res.data.push_back(*ai);
			++ai;
			continue;
		}
		res.data.push_back({ i->value, i->number + ai->number });
		++i;
		++ai;
	}
	for (; i != data.end(); ++i) res.data.push_back(*i);
	for (; ai != a.data.end(); ++ai) res.data.push_back(*ai);
	*this = res;
}

void _statistics::operator=(const _basic_statistics& a)
{
	i64 k = 0;
	for (auto i : a.data) if (i) k++;
	data.clear();
	data.reserve(k);
	for (i64 i = 0; i < (i64)a.data.size(); i++)
		if (a.data[i]) data.push_back({ a.start + i, a.data[i] });
}

i64 _statistics::first_zero()
{
	for (i64 i = 1; i < (i64)data.size(); i++)
		if (data[i].value != data[i - 1].value + 1)
			return data[i - 1].value + 1;
	return -1;
}

double _statistics::arithmetic_size(_it be, _it en)
{
	double s = 0;
	double n = 1.0 / number(be, en);
	for (auto i = be; i != en; ++i) s += i->number * log(i->number * n);
	return -s / log(2.0);
}

_matrix _statistics::to_matrix()
{
	_matrix res(data.size(), 2);
	for (i64 i = 0; i < (i64)data.size(); i++)
	{
		res[i][0] = data[i].value;
		res[i][1] = data[i].number;
	}
	return res;
}

_matrix _statistics::to_matrix(i64 mi, i64 ma)
{
	i64 n;
	i64 k = data.size() - 1;
	for (n = 0; n <= k; n++) if (data[n].value >= mi) break;
	for (; k >= 0; k--) if (data[k].value <= ma) break;
	if (k < n) return {};
	_matrix res(k - n + 1, 2);
	for (i64 i = n; i <= k; i++)
	{
		res[i - n][0] = data[i].value;
		res[i - n][1] = data[i].number;
	}
	return res;
}

i64 _statistics::operator[](i64 n) const noexcept
{
	auto li = std::lower_bound(data.begin(), data.end(), n, [](_one_stat a, i64 b) { return (a.value < b); });
	if (li == data.end()) return 0;
	return (li->value == n) ? li->number : 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
