#include <algorithm>

#include "mathematics.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

i64 _multi_interval::find(double a)
{
	i64 l = border.size();
	if (l < 2) return -1; // должно быть минимум 2 границы
	i64 n = upper_bound(border.begin(), border.end(), a) - border.begin();
	if (n < l) return n - 1; // для n = 0 -> -1 так и получается
	if (border.back() != a) return -1;
	return l - 2;
}

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

_matrix _matrix::operator<<(const _matrix& a) const noexcept
{
	if (a.empty()) return *this;
	if (empty()) return a;
	if (size.y != a.size.y) return _matrix();
	_matrix b(size.y, size.x + a.size.x);
	double* rez = b.data;
	double* m1 = data;
	double* m2 = a.data;
	for (i64 j = 0; j < size.y; j++)
	{
		for (i64 i = 0; i < size.x; i++, rez++, m1++) *rez = *m1;
		for (i64 i = 0; i < a.size.x; i++, rez++, m2++)*rez = *m2;
	}
	return b;
}

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

_matrix _matrix::this_mul_transpose() const noexcept
{
	_matrix b(size.y, size.y);
	for (i64 j = 0; j < size.y; j++)
		for (i64 i = j; i < size.y; i++)
		{
			double s = 0;
			double* a1 = data + (j * size.x);
			double* a2 = data + (i * size.x);
			for (i64 k = 0; k < size.x; k++, a1++, a2++) s += (*a1) * (*a2);
			b[j][i] = b[i][j] = s;
		}
	return b;
}

_matrix _matrix::operator*(const _matrix& a) const noexcept
{
	if (size.x != a.size.y) return _matrix();
	_matrix b(size.y, a.size.x);
	for (i64 j = 0; j < b.size.y; j++)
		for (i64 i = 0; i < b.size.x; i++)
		{
			double s = 0;
			double* a1 = data + (j * size.x);
			double* a2 = a.data + i;
			for (i64 k = 0; k < size.x; k++)
			{
				s += (*a1) * (*a2);
				a1++;
				a2 += a.size.x;
			}
			b[j][i] = s;
		}
	return b;
}

_matrix _matrix::operator-(const _matrix& a) const noexcept
{
	if (size != a.size) return _matrix();
	_matrix b(size);
	i64 n = size.square();
	for (i64 i = 0; i < n; i++) b.data[i] = data[i] - a.data[i];
	return b;
}

void _matrix::operator+=(const _matrix& a) noexcept
{
	if (size != a.size) return;
	i64 n = size.x * size.y;
	for (i64 i = 0; i < n; i++) data[i] += a.data[i];
}

_matrix::_matrix(i64 ry, i64 rx, double z) noexcept : size{ rx, ry }
{
	i64 n = size.y * size.x;
	if (n == 0) return;
	data = new double[n];
	for (i64 i = 0; i < n; i++) data[i] = z;
}

_matrix::_matrix(i64 ry, i64 rx, std::function<double(i64, i64)> fun) noexcept : size{ rx, ry }
{
	if (size.empty()) return;
	data = new double[size.x * size.y];
	for (i64 j = 0; j < size.y; j++)
		for (i64 i = 0; i < size.x; i++)
			data[j * size.x + i] = fun(j, i);
}

_matrix::_matrix(i64 ry, std::function<double(i64)> fun)  noexcept : size{ 1, ry }
{
	if (size.y == 0) return;
	data = new double[size.y];
	for (i64 i = 0; i < size.y; i++) data[i] = fun(i);
}

_matrix::_matrix(i64 ry) noexcept : size{ 1, ry }
{
	if (size.y) data = new double[size.y];
}

_matrix::_matrix(i64 ry, i64 rx)  noexcept : size{ rx,ry }
{
	if (!size.empty()) data = new double[size.square()];
}

_matrix::_matrix(_isize r)  noexcept : size(r)
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

void _matrix::set_diagonal_matrix(i64 n, double dz) noexcept
{
	resize({ n, n });
	memset(data, 0, n * n * sizeof(double));
	for (i64 i = 0; i < n; i++) data[i * n + i] = dz;
}

void _matrix::FindAllEigenVectors(_matrix& R, _matrix& A) const noexcept
{
	// Сборник научных программ на Фортране. Вып. 2. Матричная алгебра и линейная
	// алгебра. Нью-Йорк, 1960-1971, пер. с англ. (США). М., "Статистика", 1974.
	// 224 с.
	// стр - 187, подпрограмма EIGEN
	double ANORM, ANRMX, THR, X, Y, SINX, SINX2, COSX, COSX2, SINCS;
	constexpr double RANGE = 1.0E-13;                                     // было -13
//	constexpr double LIM = 1.0E-11;                                       // -6
	A = *this;
	size_t N = A.size.x;
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
}

//вычисление псевдообратной матрицы
_matrix _matrix::pseudoinverse() const noexcept
{
	double LIM = 1.0E-13;                                       // min -10 max -15  было -11 -13
	_matrix BQ, BL;
	FindAllEigenVectors(BQ, BL);
	double SumD = 0;
	for (i64 i = 0; i < size.y; i++)
		if (BL[i][i] > SumD) SumD = BL[i][i];
	LIM *= SumD;
	_matrix Aplus(size.y, size.y, 0);
	for (i64 j = 0; j < size.y; j++)
	{
		double lam = BL[j][j];
		if (lam < LIM) continue;
		lam = 1.0 / lam;
		double* rez = Aplus.data;
		double* ist = BQ.data + j;
		for (i64 y = 0; y < size.y; y++)
		{
			double mn = lam * ist[y * size.y];
			for (i64 x = 0; x < size.y; x++, rez++)
				*rez += mn * ist[x * size.y];
		}
	}
	return Aplus;
}

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

_matrix _matrix::linear_prediction(const _matrix& k, i64 start, i64 n, i64 ots) const noexcept
{
	_matrix b(n, 1, 0);
	if (((k.size.x != 1) && (k.size.y != 1)) || ((size.x != 1) && (size.y != 1))) return b; // должен быть вектор
	i64 rk = k.size.square();
	i64 r = size.square();
	if ((start < rk) || (start > r)) return b;
	for (i64 i = 0; i < n; i++)
	{
		double s = 0;
		for (i64 ii = 0; ii < rk; ii++)
		{
			i64 p = ii - rk + i - ots;
			s += k.data[ii] * ((p >= 0) ? b.data[p] : data[start + p]);
		}
		b.data[i] = s;
	}
	return b;
}

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

_matrix get_noise(i64 n, double k)
{
	_matrix b(n);
	for (i64 i = 0; i < n; i++) b[0][i] = rnd(-k, k);
	return b;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void _basic_statistics::push(i64 x)
{
	if (data.empty()) start = x;
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

double _interval_statistics::min_mean()
{
	if (element.empty()) return 0;
	return element.front().mean;
}

double _interval_statistics::max_mean()
{
	if (element.empty()) return 0;
	return element.back().mean;
}

void _interval_statistics::init_equiprobable(_basic_statistics& bs, i64 n, double k)
{
	element.resize(n);
	i64 nbs = bs.number();
	i64 ii = 0;
	i64 s = 0;
	for (i64 i = 0; i < n; i++)
	{
		i64 ss = nbs * (i + 1) / n;
		i64 s_p = s;
		i64 ii_p = i;
		double mean = 0;
		while ((s < ss) && (ii < (i64)bs.data.size()))
		{
			mean += ((ii + bs.start) * k) * bs.data[ii];
			s += bs.data[ii];
			ii++;
		}
		element[i].interval.min = (ii_p + bs.start - 0.5) * k;
		element[i].interval.max = (ii + bs.start - 0.5) * k;
		if (s - s_p) mean /= (s - s_p);
		element[i].mean = mean;
		element[i].number = s - s_p;
	}
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
