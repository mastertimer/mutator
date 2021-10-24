#include "bit_vector.h"

namespace
{

	inline u64 mask1(uchar b) noexcept // b единиц
	{ // *
		return (b >= 64) ? 0xffffffffffffffff : ((1ULL << b) - 1);
	}

}

i64 _bit_vector::size() const
{ // *
	return data.size() * 64 + bit - 64;
}

bool _bit_vector::empty() const
{ // *
	return size() == 0;
}

void _bit_vector::clear()
{ // *
	data.clear();
	bit = 64;
	bit_read = 0;
}

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

u64 _bit_vector::pop() noexcept
{
	i64 r = bit_read >> 6;
	uchar bi = bit_read & 63;
	bit_read++;
	return ((data[r] >> bi) & 1);
}

u64 _bit_vector::pop_safely() noexcept
{
	i64 r = bit_read >> 6;
	uchar bi = bit_read & 63;
	bit_read++;
	if (r >= (i64)data.size()) return 0;
	return ((data[r] >> bi) & 1);
}

u64 _bit_vector::pop(uchar n) noexcept
{
	if (n == 0) return 0;
	i64 r = bit_read >> 6;
	uchar bi = bit_read & 63;
	bit_read += n;
	if (bi + n <= 64) return (data[r] >> bi) & mask1(n);
	return ((data[r] >> bi) | (data[r + 1] << (64ui8 - bi))) & mask1(n);
}

void _bit_vector::push(u64 a) noexcept
{ // *
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
		push(aa, n);
		return;
	}
	if (bit < 64)
	{
		u64 nn = 64 - bit;
		push(aa, nn);
		n -= nn;
	}
	for (; n >= 64; n -= 64) push(aa, 64);
	push(aa, n);
}

void _bit_vector::push(u64 a, uchar n) noexcept
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
	push(a, n);
}
