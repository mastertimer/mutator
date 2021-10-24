#pragma once

#include "basic.h"

struct _bit_vector
{
	std::vector<u64> data;
	uchar            bit      = 64; // сколько бит заполнено в последнем числе
	i64              bit_read =  0; // позиция бита для чтения

	bool operator==(const _bit_vector& b) const;
	bool operator!=(const _bit_vector& b) const { return !(*this == b); }

	void push(u64 a);
	void push(u64 a, uchar n);
	void pushnod(u64 a, u64 n); // добавить n одинаковых бит
	void pushn1(u64 a);         // добавить ограниченное количество бит, 1xxxxxxxx

	u64  pop();
	u64  pop(uchar n);
	u64  pop_safely();

	i64  size() const;  // в битах
	void resize(i64 v); // в битах

	bool empty() const;
	void clear();
};

_stack& operator<<(_stack& o, const _bit_vector& p);
_stack& operator>>(_stack& o, _bit_vector& p);
