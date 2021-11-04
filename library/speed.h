#pragma once

#include "basic.h"

t_t struct _bank
{
	_t** data = nullptr;
	uint size = 0;
	uint capacity = 0;

	~_bank();
	void push() { size--; } // без проверок! пользоваться без ошибок!
	_t* pop() { if (size == capacity) reserve(size * 2 + 1); return data[size++]; }

private:
	void reserve(uint rdata);
};

t_t void _bank<_t>::reserve(uint rdata)
{
	if (rdata <= capacity) return;
	_t** data2 = new _t * [rdata];
	memcpy(data2, data, sizeof(_t*) * capacity);
	delete[] data;
	data = data2;
	for (uint i = capacity; i < rdata; i++) data[i] = new _t;
	capacity = rdata;
}

t_t _bank<_t>::~_bank()
{
	for (uint i = 0; i < capacity; i++) delete data[i];
	delete[] data;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

t_t struct _speed
{
	_t* a = nullptr;

	_speed() { a = bank.pop(); a->clear(); }
	_speed(bool go) { if (go) { a = bank.pop(); a->clear(); } }
	_speed(const _speed<_t>& b) = delete;
	_speed<_t>& operator=(const _speed<_t>& b) = delete;
	_speed<_t>& operator=(_speed<_t>&& b) = delete;
	~_speed() { if (a) bank.push(); }
	void start() { a = bank.pop(); a->clear(); }
	void stop() { bank.push(); a = nullptr; }
	operator _t* () { return a; }
	_t* operator->() { return a; }

private:
	inline static _bank<_t> bank;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

