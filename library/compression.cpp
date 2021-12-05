#include "compression.h"
#include "rnd.h"

#include <map>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace
{
	constexpr u64 h0 =           0; // 0/4
	constexpr u64 h1 =  0x40000000; // 1/4
	constexpr u64 h2 =  0x80000000; // 2/4
	constexpr u64 h3 =  0xc0000000; // 3/4
	constexpr u64 h4 = 0x100000000; // 4/4
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

struct _frequency
{
	double frequency[256];

	void start() noexcept
	{
		for (int i = 0; i < 256; i++) frequency[i] = kkk4 / 256.0;
	}

	void norm()
	{
		double s = 0;
		for (int i = 0; i < 256; i++) s += frequency[i];
		s = kkk4 / s;
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
		r1 = (r1 << 1ui8) + bit;
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

