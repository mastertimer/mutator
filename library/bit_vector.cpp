#include "bit_vector.h"

#define mask1(b) (((b) >= 64) ? 0xffffffffffffffff : ((1ui64 << (b)) - 1)) // маска u64 из b бит

void _bit_vector::save(_stack& mem)
{
	mem << data << bit;
}

void _bit_vector::load(_stack& mem)
{
	mem >> data >> bit;
	bit_read = 0;
}

void _bit_vector::resize(i64 v)
{
	i64 rp = (i64)data.size();
	i64 r = (v + 63) >> 6;
	bit = v & 63;
	if (bit == 0) bit = 64;
	if (rp == r)
	{
		if (r > 0) data.back() &= mask1(bit);
		return;
	}
	data.resize(r);
}

u64 _bit_vector::pop1() noexcept
{
	i64 r = bit_read >> 6;
	uchar bi = bit_read & 63;
	bit_read++;
	return ((data[r] >> bi) & 1);
}

u64 _bit_vector::pop1_safely() noexcept
{
	i64 r = bit_read >> 6;
	uchar bi = bit_read & 63;
	bit_read++;
	if (r >= (i64)data.size()) return 0;
	return ((data[r] >> bi) & 1);
}

u64 _bit_vector::popn(uchar n) noexcept
{
	if (n == 0) return 0;
	i64 r = bit_read >> 6;
	uchar bi = bit_read & 63;
	bit_read += n;
	if (bi + n <= 64) return (data[r] >> bi) & mask1(n);
	return ((data[r] >> bi) | (data[r + 1] << (64ui8 - bi))) & mask1(n);
}

void _bit_vector::push1(u64 a) noexcept
{
	a &= 1;
	if (bit == 64)
	{
		data.push_back(a);
		bit = 1;
		return;
	}
	data.back() |= (a << bit++);
}

void _bit_vector::pushnod(u64 a, u64 n) noexcept
{
	u64 aa = (a & 1) ? 0xffffffffffffffff : 0;
	if (n <= 64)
	{
		pushn(aa, n);
		return;
	}
	if (bit < 64)
	{
		u64 nn = 64 - bit;
		pushn(aa, nn);
		n -= nn;
	}
	for (; n >= 64; n -= 64) pushn(aa, 64);
	pushn(aa, n);
}

void _bit_vector::pushn(u64 a, uchar n) noexcept
{
	if (n == 0) return;
	a &= mask1(n);
	if (bit == 64)
	{
		data.push_back(a);
		bit = n;
		return;
	}
	data.back() |= (a << bit);
	bit += n;
	if (bit <= 64) return;
	bit -= 64;
	data.push_back(a >> (n - bit));
}

void _bit_vector::pushn1(u64 a) noexcept
{
	uchar n = 0;
	while ((a >> n) > 1) n++;
	pushn(a, n);
}
