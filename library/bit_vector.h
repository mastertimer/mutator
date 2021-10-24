#pragma once

#include "basic.h"

struct _bit_vector
{
	std::vector<u64> data;
	uchar bit = 64; // сколько бит заполнено в последнем числе
	i64 bit_read = 0; // позиция бита для чтения

	void push(u64 a) noexcept;
	void push(u64 a, uchar n) noexcept;
	void pushnod(u64 a, u64 n) noexcept; // добавить n одинаковых бит
	void pushn1(u64 a) noexcept; // добавить ограниченное количество бит, 1xxxxxxxx
	u64 pop() noexcept;
	u64 pop(uchar n) noexcept;
	u64 pop_safely() noexcept;
	i64 size() const; // в битах
	bool empty() const;
	void resize(i64 v);
	void clear();

	void save(_stack& mem);
	void load(_stack& mem);
};
