#pragma once

#include "basic.h"

struct _bit_vector
{
	std::vector<u64> data;
	uchar bit = 64; // сколько бит заполнено в последнем числе
	i64 bit_read = 0; // позиция бита для чтения

	void push1(u64 a) noexcept; // добавить 1 бит
	void pushn(u64 a, uchar n) noexcept; // добавить n бит
	void pushnod(u64 a, u64 n) noexcept; // добавить n одинаковых бит
	void pushn1(u64 a) noexcept; // добавить ограниченное количество бит, 1xxxxxxxx
	u64 pop1() noexcept; // прочитать 1 бит
	u64 pop1_safely() noexcept; // прочитать 1 бит (безопасно)
	u64 popn(uchar n) noexcept; // прочитать n бит
	i64 size() const noexcept { return (i64)data.size() * 64 - (64 - bit); } // в битах!!!
	bool empty() const noexcept { return (size() == 0); }
	void resize(i64 v);
	void clear() { data.clear(); bit = 64; bit_read = 0; }

	void save(_stack& mem);
	void load(_stack& mem);
};
