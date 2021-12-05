#include "arithmetic_coding.h"

namespace
{
	constexpr u64 h0 =           0; // 0/4
	constexpr u64 h1 =  0x40000000; // 1/4
	constexpr u64 h2 =  0x80000000; // 2/4
	constexpr u64 h3 =  0xc0000000; // 3/4
	constexpr u64 h4 = 0x100000000; // 4/4
}

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
