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

_stack& operator<<(_stack& o, const _bit_vector& p)
{ // *
	o << p.data << p.bit;
	return o;
}

_stack& operator>>(_stack& o, _bit_vector& p)
{ // *
	o >> p.data >> p.bit;
	p.bit_read = 0;
	return o;
}

bool _bit_vector::operator==(const _bit_vector& b) const
{ // *
	return (bit == b.bit) && (data == b.data);
}

void _bit_vector::resize(i64 v)
{ // *
	if (v < 0) v = 0;
	data.resize((v + 63) >> 6);
	bit = v & 63;
	if (bit == 0) bit = 64;
	if (v > 0) data.back() &= mask1(bit);
}

u64 _bit_vector::pop()
{ // *
	i64 r = bit_read >> 6;
	uchar bi = bit_read & 63;
	bit_read++;
	return (data[r] >> bi) & 1;
}

u64 _bit_vector::pop_safely()
{ // *
	i64 r = bit_read >> 6;
	uchar bi = bit_read & 63;
	bit_read++;
	if (r >= (i64)data.size()) return 0;
	return (data[r] >> bi) & 1;
}

u64 _bit_vector::pop(uchar n)
{ // *
	if (n == 0) return 0;
	i64 r = bit_read >> 6;
	uchar bi = bit_read & 63;
	bit_read += n;
	if (bi + n <= 64) return (data[r] >> bi) & mask1(n);
	return ((data[r] >> bi) | (data[r + 1] << (64ui8 - bi))) & mask1(n);
}

void _bit_vector::push(u64 a)
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

void _bit_vector::pushnod(u64 a, u64 n)
{ // *
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
	for (; n >= 64; n -= 64) data.push_back(aa);
	push(aa, n);
}

void _bit_vector::push(u64 a, uchar n)
{ // *
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

void _bit_vector::pushn1(u64 a)
{ // *
	push(a, position1_64(a >> 1));
}
