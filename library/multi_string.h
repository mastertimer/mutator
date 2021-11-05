#pragma once

#include "basic.h"

struct _multi_string
{
	std::vector<std::wstring> line;

	_multi_string() : line(1) {}
	void push(_stack* mem) { *mem << line; }
	void pop(_stack* mem) { *mem >> line; }
	bool delete_char(i64 y, i64 x);
	void insert_char(i64 y, i64 x, wchar_t c);
	void div2line(i64 y, i64 x);
};
