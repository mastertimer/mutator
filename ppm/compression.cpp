#include "compression.h"

#include <map>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace
{
	constexpr u64 h0 =           0; // 0/4
	constexpr u64 h1 =  0x40000000; // 1/4
	constexpr u64 h2 =  0x80000000; // 2/4
	constexpr u64 h3 =  0xc0000000; // 3/4
	constexpr u64 h4 = 0x100000000; // 4/4

	const double klog2 = -1.0 / log(2);
	constexpr double golden_phi = 0.618033988749894848; // 2.0 / (std::sqrt(5.0) + 1.0);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static double local_maximum(const std::function<double(double)>& func, double a, double b)
{
	if (a > b) std::swap(a, b);
	double c = b - (b - a) * golden_phi;
	double d = a + (b - a) * golden_phi;
	double fc = func(c);
	double fd = func(d);

	while (c > a && d > c && b > d)
		if (fd > fc)
		{
			a = c;
			c = d;
			d = a + (b - a) * golden_phi;
			fc = fd;
			fd = func(d);
		}
		else
		{
			b = d;
			d = c;
			c = b - (b - a) * golden_phi;
			fd = fc;
			fc = func(c);
		}

	return std::max({ fc, fd, func(a), func(b) });
}

double max_of_function(std::function<double(double)> func, double a, double b)
{
	if (b < a) std::swap(a, b);
	constexpr i64 intervals_number = 2207;
	double dx = (b - a) / intervals_number;
	double precision = std::max(abs(a), abs(b)) * 1e-13;
	auto f0 = func(a);
	auto f1 = func(a + dx);
	double maxx = std::max(f0, func(b));
	for (i64 i = 2; i <= intervals_number; i++)
	{
		double x2 = (i == intervals_number) ? b : a + dx * i;
		double f2 = func(x2);
		if (f1 >= f2 && f1 >= f0)
		{
			double gs = local_maximum(func, a + dx * (i - 2), x2);
			maxx = std::max(maxx, gs);
		}
		f0 = f1;
		f1 = f2;
	}
	return maxx;
}

double entropy1bit(double p)
{
	if (p == 0 || p == 1.0) return 0;
	const double q = 1.0 - p;
	return klog2 * (p * log(p) + q * log(q));
}

int fac(int x)
{
	static const int table[] = { 1, 1, 2, 6, 24, 120, 720, 5040, 40320, 362880, 3628800, 39916800, 479001600 };
	if ((x >= 0) && (x <= 12)) return table[x];
	return 0;
}

double pow(double a, int n) // a^n
{
	auto nn = abs(n);
	double res = 1;
	for (int i = 1; i <= nn; i++) res *= a;
	return (n >= 0) ? res : (1.0 / res);
}

// энтропия сжатого бита
// n - количество предыдущих бит
// p1 - реальная вероятность 1
// k1[i] - вероятность 1, если до этого было i (из n) 1
double entropy_of_compressed(int n, double p1, std::vector<double>& k1)
{
	if (k1.size() != n + 1) return 0;
	double e = 0;
	double p0 = 1.0 - p1;
	int fac_n = fac(n);
	for (int i = 0; i <= n; i++)
	{
		int n_i = n - i;
		int c_i_n = fac_n / (fac(i) * fac(n_i));
		double p = pow(p1, i) * pow(p0, n_i);
		e += c_i_n * p * (p1 * log(k1[i]) + p0 * log(1 - k1[i]));
	}
	return klog2 * e;
}

double delta_entropy(int n, double p1, std::vector<double>& k1)
{
	return entropy_of_compressed(n, p1, k1) - entropy1bit(p1);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void arithmetic_coding(const std::vector<uchar>& data, _bit_vector& res)
{
	res.clear();
	if (data.empty()) return;
	u64 bit_size = position1_64(data.size());
	res.push(bit_size, 6);
	res.push(data.size(), bit_size);
	u64 frequency[256];
	for (auto& i : frequency) i = 1;
	u64 summ_frequency = 256;
	u64 begin = h0, end = h4;
	u64 bad_bit = 0;
	for (u64 c : data)
	{
		u64 chs = 0;
		for (u64 i = 0; i < c; i++) chs += frequency[i];
		u64 eb = end - begin;
		end = begin + eb * (chs + frequency[c]++) / summ_frequency;
		begin += eb * chs / summ_frequency++;
		for (;;)
		{
			if (u64 bi = (begin >= h2); bi || (end <= h2))
			{
				res.push(bi);
				if (bad_bit) { res.pushnod(bi ^ 1, bad_bit); bad_bit = 0; }
				begin = (begin - h2 * bi) << 1;
				end = (end - h2 * bi) << 1;
				continue;
			}
			if ((begin < h1) || (end > h3)) break;
			begin = (begin - h1) << 1;
			end = (end - h1) << 1;
			bad_bit++;
		}
	}
	u64 c = ((begin <= h1) && (end >= h2));
	res.push(c ^ 1);
	res.pushnod(c, bad_bit + 1);
}

void arithmetic_decoding(_bit_vector& data, std::vector<uchar>& res)
{
	res.clear();
	if (data.empty()) return;
	data.bit_read = 0;
	i64 size = data.pop(data.pop(6));
	res.reserve(size);
	u64 frequency[256];
	for (auto& i : frequency) i = 1;
	u64 summ_frequency = 256;
	u64 begin = h0, end = h4, x = 0;
	for (i64 i = 0; i < 32; i++) x = (x << 1) + data.pop_safely();
	for (i64 i = 0; i < size; i++)
	{
		i64 c = 0;
		u64 chs = 0;
		u64 eb = end - begin;
		for (u64 y = ((x - begin + 1) * summ_frequency - 1) / eb; y >= (chs += frequency[c]); c++);
		end = begin + eb * chs / summ_frequency;
		begin += eb * (chs - frequency[c]++) / summ_frequency++;
		res.push_back(c);
		for (;;)
		{
			u64 delta = (end <= h2) ? h0 : (begin >= h2) ? h2 : ((begin >= h1) && (end <= h3)) ? h1 : h4;
			if (delta == h4) break;
			begin = (begin - delta) << 1;
			end = (end - delta) << 1;
			x = ((x - delta) << 1) + data.pop_safely();
		}
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

double information(std::vector<uchar>& a, double* permutations, double* frequency)
{
	if (a.empty()) return 0.0;
	i64 k[256] = {};
	for (auto i : a) k[i]++;
	i64 v = a.size();
	double s1 = 0.0;
	for (i64 i = 2; i <= v; i++) s1 += log((double)i);
	for (auto j : k)
		for (i64 i = 2; i <= j; i++)
			s1 -= log((double)i);
	s1 /= log(256.0);
	double s2 = 0.0;
	for (i64 i = 1; i < 256; i++) s2 += log((double)(v + i)) - log((double)i); // 256 - вариантов символов
	s2 /= log(256.0);
	if (permutations) *permutations = s1;
	if (frequency) *frequency = s2;
	return s1 + s2;
}

double size_arithmetic_coding(std::vector<uchar>& a, double frequency0)
{
	double s = 0.0;
	double frequency[256];
	for (auto& i : frequency) i = frequency0;
	double vv = 256 * frequency0;
	for (auto c : a) s += log(frequency[c]++ / vv++);
	return -s / log(256.0);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

i64 _frequency2::size()
{
	i64 s = 0;
	for (i64 i = 0; i < number; i++) s += frequency[i];
	return s;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void stir_vector(std::vector<uchar> &v) // перемешать
{
	for (i64 i = (i64)v.size() - 1; i > 0; i--) std::swap(v[i], v[rnd(i + 1)]);
}

std::vector<uchar> generate_vector(_frequency2& f)
{
	std::vector<uchar> res;
	res.reserve(f.size());
	for (i64 i = 0; i < f.number; i++) res.insert(res.end(), f.frequency[i], i);
	for (i64 i = (i64)res.size() - 1; i > 0; i--) std::swap(res[i], res[rnd(i + 1)]);
	return res;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

constexpr double kkk4 = 22.0; // 22.0
//                  0   1    2    3    4    5    6    7   8    9   10   11   12  13   14  15  16
double kkk6[] = { 300, 15, 100, 190, 3.3, 220, 600, 1.7, 60, 256, 240, 500, 400, 22, 900, 12,  5 }; // 22.0

struct _frequency
{
	double frequency[256];

	void start() noexcept
	{
		for (int i = 0; i < 256; i++) frequency[i] = kkk4 / 256.0;
	}

	void start(double kk5) noexcept
	{
		for (int i = 0; i < 256; i++) frequency[i] = kk5 / 256.0;
	}

	void norm()
	{
		double s = 0;
		for (int i = 0; i < 256; i++) s += frequency[i];
		s = kkk4 / s;
		for (int i = 0; i < 256; i++) frequency[i] *= s;
	}

	void norm(double kk5)
	{
		double s = 0;
		for (int i = 0; i < 256; i++) s += frequency[i];
		s = kk5 / s;
		for (int i = 0; i < 256; i++) frequency[i] *= s;
	}

	void norm(u64 rr, u64* f)
	{
		double s = 0;
		for (int i = 0; i < 256; i++) s += frequency[i];
		s = rr / s;
		f[0] = 0;
		for (int i = 1; i <= 256; i++)
		{
			u64 a = (u64)(frequency[i - 1] * s);
			if (a == 0) a = 1;
			f[i] = f[i - 1] + a;
		}
	}
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct _ppc
{
	u64 frequency = 0;
	std::map<uchar, _ppc> next;
};

uchar ppm(const std::vector<uchar>& data, std::vector<uchar>& res, u64 g)
{
	res.clear();
	if (data.empty()) return 0;


	u64 bad_bit = 0; // количество плохих бит
	uchar bit_size = 0;
	u64 s2 = data.size();
	while (s2) { bit_size++; s2 >>= 1; }
	uchar r1 = bit_size - 1; // активный байт
	uchar r1n = 6; // количество бит в активном байте
	auto otgruz = [&](uchar bit)
	{
		r1 = (r1 << 1) + bit;
		if (++r1n == 8)
		{
			res.push_back(r1);
			r1 = r1n = 0;
		}
	};
	for (uchar ii = 0; ii < bit_size; ii++) otgruz((data.size() >> ii) & 1);
	u64 frequency[257]; // частичные суммы
	for (u64 ii = 0; ii < 257; ii++) frequency[ii] = ii;
	u64 begin = h0; // начало рабочего диапазона
	u64 end = h4; // конец рабочего диапазона


	_ppc ppc;
	_frequency f;
	for (u64 i = 0; i < data.size(); i++)
	{
		u64 c = data[i]; // активный символ



		f.start();
		for (auto& jj : ppc.next) f.frequency[jj.first] += jj.second.frequency;
		_ppc* ppc2 = &ppc;
		for (u64 j = 1; j <= g; j++)
		{
			if (j > i) break;
			f.norm();
			ppc2 = &ppc2->next[data[i - j]];
			for (auto& jj : ppc2->next) f.frequency[jj.first] += jj.second.frequency;
		}
		//		f.norm();
		f.norm(1073741000, frequency);

		ppc.frequency++;
		ppc.next[(uchar)c].frequency++;
		ppc2 = &ppc;
		for (u64 j = 1; j <= g; j++)
		{
			if (j > i) break;
			ppc2 = &ppc2->next[data[i - j]];
			ppc2->next[(uchar)c].frequency++;
		}



		u64 eb = end - begin;
		end = begin + eb * frequency[c + 1] / frequency[256];
		begin += eb * frequency[c] / frequency[256];
	start:
		if (end <= h2)
		{
			otgruz(0);
			while (bad_bit) { otgruz(1); bad_bit--; }
			begin <<= 1;
			end <<= 1;
			goto start;
		}
		if (begin >= h2)
		{
			otgruz(1);
			while (bad_bit) { otgruz(0); bad_bit--; }
			begin = (begin - h2) << 1;
			end = (end - h2) << 1;
			goto start;
		}
		if ((begin >= h1) && (end <= h3))
		{
			begin = (begin - h1) << 1;
			end = (end - h1) << 1;
			bad_bit++;
			goto start;
		}
		for (u64 j = c + 1; j < 257; j++) frequency[j]++;
	}
	uchar c = ((begin <= h1) && (end >= h2));
	otgruz(c ^ 1);
	otgruz(c);
	while (bad_bit) { otgruz(c); bad_bit--; }
	if (r1n) res.push_back(r1 << (8 - r1n));
	return r1n;
}

uchar ppm2(const std::vector<uchar>& data, std::vector<uchar>& res, u64 g)
{
	res.clear();
	if (data.empty()) return 0;


	u64 bad_bit = 0; // количество плохих бит
	uchar bit_size = 0;
	u64 s2 = data.size();
	while (s2) { bit_size++; s2 >>= 1; }
	uchar r1 = bit_size - 1; // активный байт
	uchar r1n = 6; // количество бит в активном байте
	auto otgruz = [&](uchar bit)
	{
		r1 = (r1 << 1) + bit;
		if (++r1n == 8)
		{
			res.push_back(r1);
			r1 = r1n = 0;
		}
	};
	for (uchar ii = 0; ii < bit_size; ii++) otgruz((data.size() >> ii) & 1);
	u64 frequency[257]; // частичные суммы
	for (u64 ii = 0; ii < 257; ii++) frequency[ii] = ii;
	u64 begin = h0; // начало рабочего диапазона
	u64 end = h4; // конец рабочего диапазона


	_ppc ppc;
	_frequency f;
	for (u64 i = 0; i < data.size(); i++)
	{
		u64 c = data[i]; // активный символ



		f.start(kkk6[0]);
		for (auto& jj : ppc.next) f.frequency[jj.first] += jj.second.frequency;
		_ppc* ppc2 = &ppc;
		for (u64 j = 1; j <= g; j++)
		{
			if (j > i) break;
			f.norm(kkk6[j]);
			ppc2 = &ppc2->next[data[i - j]];
			for (auto& jj : ppc2->next) f.frequency[jj.first] += jj.second.frequency;
		}
		//		f.norm();
		f.norm(1073741000, frequency);

		ppc.frequency++;
		ppc.next[(uchar)c].frequency++;
		ppc2 = &ppc;
		for (u64 j = 1; j <= g; j++)
		{
			if (j > i) break;
			ppc2 = &ppc2->next[data[i - j]];
			ppc2->next[(uchar)c].frequency++;
		}



		u64 eb = end - begin;
		end = begin + eb * frequency[c + 1] / frequency[256];
		begin += eb * frequency[c] / frequency[256];
	start:
		if (end <= h2)
		{
			otgruz(0);
			while (bad_bit) { otgruz(1); bad_bit--; }
			begin <<= 1;
			end <<= 1;
			goto start;
		}
		if (begin >= h2)
		{
			otgruz(1);
			while (bad_bit) { otgruz(0); bad_bit--; }
			begin = (begin - h2) << 1;
			end = (end - h2) << 1;
			goto start;
		}
		if ((begin >= h1) && (end <= h3))
		{
			begin = (begin - h1) << 1;
			end = (end - h1) << 1;
			bad_bit++;
			goto start;
		}
		for (u64 j = c + 1; j < 257; j++) frequency[j]++;
	}
	uchar c = ((begin <= h1) && (end >= h2));
	otgruz(c ^ 1);
	otgruz(c);
	while (bad_bit) { otgruz(c); bad_bit--; }
	if (r1n) res.push_back(r1 << (8 - r1n));
	return r1n;
}

double ppm_test(const std::vector<uchar>& data, u64 g)
{
	if (data.empty()) return 0;
	double size = 0;
	_ppc ppc;
	_frequency f;
	for (u64 i = 0; i < data.size(); i++)
	{
		u64 c = data[i]; // активный символ
		f.start(256);
		for (auto& jj : ppc.next)
		{
			double mm = 256.0 / kkk4;
			f.frequency[jj.first] += jj.second.frequency * mm;
		}
		_ppc* ppc2 = &ppc;
		for (u64 j = 1; j <= g; j++)
		{
			if (j > i) break;
			f.norm(256);
			ppc2 = &ppc2->next[data[i - j]];
			for (auto& jj : ppc2->next)
			{
				double mm = 256.0 / kkk4;
				f.frequency[jj.first] += jj.second.frequency * mm;
			}
		}
		f.norm(1);
		size += log(f.frequency[c]);
		ppc.frequency++;
		ppc.next[(uchar)c].frequency++;
		ppc2 = &ppc;
		for (u64 j = 1; j <= g; j++)
		{
			if (j > i) break;
			ppc2 = &ppc2->next[data[i - j]];
			ppc2->next[(uchar)c].frequency++;
		}

	}
	return -size / log(256);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct nab
{
	uchar num; // количество
	uchar numnum; // количество количества
};
