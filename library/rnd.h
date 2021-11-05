#pragma once

#include "basic.h"

struct _rnd
{ // (a = 521, b = 353)
	_rnd(i64 p = 0) { init(p); }
	u64 operator()() { i = (i + 1) & 1023; return (d[i] = d[(i + 503) & 1023] ^ d[(i + 671) & 1023]); }
	i64 operator()(i64 m) { i = (i + 1) & 1023; return (d[i] = d[(i + 503) & 1023] ^ d[(i + 671) & 1023]) % m; }
	double operator()(double n, double k) { return ((*this)() & 0xFFFFFFFF) * (k - n) / 0xFFFFFFFF + n; }
	void init(u64 p);

private:
	u64 d[1024]; // предыдущие числа
	u64 i; // последн¤¤ позици¤
};

inline _rnd rnd;
