﻿#pragma once

#define NOMINMAX
#include <windows.h>
#include <string>
#include <vector>
#include <fstream>
#include <functional>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

using uchar  = unsigned char;
using ushort = unsigned short;
using uint   = unsigned int;
using u64    = unsigned long long;
using i64    = long long;

using astr   = const char*;
using wstr   = const wchar_t*;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

constexpr i64 number_thread = 16; // 16, не менять
constexpr double pi = 3.1415926535897932384626;

constexpr i64 bit8[256] = {
	0, 1, 1, 2, 1, 2, 2, 3, 1, 2, 2, 3, 2, 3, 3, 4,	1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5,
	1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5,	2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
	1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5,	2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
	2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,	3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,
	1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5,	2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
	2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,	3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,
	2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,	3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,
	3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,	4, 5, 5, 6, 5, 6, 6, 7, 5, 6, 6, 7, 6, 7, 7, 8 };

inline i64 bit16(ushort x) { return bit8[x & 255] + bit8[x >> 8]; }

constexpr i64 position1_8[256] = {
	0, 1, 2, 2, 3, 3, 3, 3, 4, 4, 4, 4, 4, 4, 4, 4, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
	6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
	7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
	7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
	8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
	8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
	8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
	8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8 };

i64 position1_64(u64 a);

constexpr uint color_set[32] = { // набор разных цветов одной яркости
	0xFF0080FD, 0xFFEF0000, 0xFF9E3BFF, 0xFF938700, 0xFF12AA00, 0xFFD34E0D, 0xFF7470DC, 0xFF4D9682,
	0xFFC80FCE, 0xFFB06381,	0xFFE22653, 0xFF009D93, 0xFFC4439D, 0xFF258ECB, 0xFF659B00, 0xFF3F77FF,
	0xFF887E87, 0xFF9659DD, 0xFFB57000, 0xFF39A337, 0xFF5D84BE, 0xFFAF1FFB, 0xFFDE028A, 0xFF768F51,
	0xFFB400FC,	0xFFE23700, 0xFFCB29B2, 0xFFA3755A, 0xFFC45D37, 0xFFEB171B, 0xFF966DA8, 0xFFAC4ACF };

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

constexpr uint c00 = 0;          // прозрачный цвет
constexpr uint cc0 = 0xFF000000; // цвет фона
constexpr uint cc1 = 0xFF208040; // цвет 1
constexpr uint cc2 = 0xFF40FF80; // цвет 2
constexpr uint cc3 = 0xFFA0FFC0; // цвет 3
constexpr uint cc4 = 0xFF104020; // цвет 4

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

inline std::wstring exe_path; // путь к запущенному exe файлу

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define t_t template <typename _t>
#define t_b template <typename _b>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

i64 bit_for_value(u64 k); // k - количество чисел. (1) = 0, (2) = 1, (4) = 2

void set_clipboard_text(astr text);
void set_clipboard_text(std::wstring_view text);
void os_pordis(double min, double max, i64 maxN, double& mi, double& step, double min_step = 0.0);

bool save_file(std::wstring_view fn, const char* data, i64 n);
bool load_file(std::wstring_view fn, char** data, i64* n);
bool load_file(std::wstring_view fn, std::vector<uchar>& res);

std::wstring string_to_wstring(std::string_view s);
std::string wstring_to_string(std::wstring_view b);
wstr uint64_to_wstr_hex(u64 a);
std::wstring substr(std::wstring_view s, i64 n, i64 k); // подстрока которая не кидает исключения

std::wstring double_to_wstring(double a, int z);
std::string  double_to_string(double a, int z);

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct _rnd
{ // (a = 521, b = 353)
	_rnd(i64 p = 0)       { init(p); }
	u64 operator()()      { i = (i + 1) & 1023; return (d[i] = d[(i + 503) & 1023] ^ d[(i + 671) & 1023]); }
	i64 operator()(i64 m) { i = (i + 1) & 1023; return (d[i] = d[(i + 503) & 1023] ^ d[(i + 671) & 1023]) % m;}
	double operator()(double n, double k) { return ((*this)() & 0xFFFFFFFF) * (k - n) / 0xFFFFFFFF + n; }
	void init(u64 p);

private:
	u64 d[1024]; // предыдущие числа
	u64 i; // последняя позиция
};

inline _rnd rnd;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct _stack
{
	char* data;
	i64   capacity;
	i64   size;
	i64   adata; // активный указатель

	_stack(i64 r = 0)              noexcept; // r - зарезервировать размер
	_stack(const _stack &a)        noexcept;
	_stack(_stack&& a)             noexcept;
	_stack(void* data2, int vdata) noexcept; // инициализация куском памяти
	~_stack() { delete[] data; }

	bool operator==(const _stack& a) const noexcept;

	void clear() { size  = 0; adata = 0; }
	void erase(i64 n, i64 k);
	bool save_to_file(std::wstring_view fn);
	bool load_from_file(std::wstring_view fn);

	t_b _stack& operator<<(const std::vector<_b>& b) noexcept;
	t_b _stack& operator<<(_b a) noexcept;
	    _stack& operator<<(const _stack& a) noexcept;
	    _stack& operator<<(const std::wstring& a) noexcept;
	    void    push_data(const void* data2, i64 vdata);
	    void    push_fill(int vdata, char c); // занести кучу одинаковых символов

	    _stack& operator>>(_stack& a) noexcept;
	    _stack& operator>>(std::wstring& s) noexcept;
	t_b _stack& operator>>(std::vector<_b>& b) noexcept;
	t_b _stack& operator>>(_b& a) noexcept;
	    void    pop_data(void* data2, i64 vdata);

	void revert(i64 bytes); // вернуть данные
	void skip(i64 bytes);   // пропустить данные

  private:
	void set_capacity(i64 rdata); // изменить размер массива в большую сторону
};

t_b _stack& _stack::operator<<(const std::vector<_b>& b) noexcept
{
	*this << (uint)b.size();
	for (auto& i : b) *this << i;
	return *this;
}

t_b _stack& _stack::operator<<(_b a) noexcept
{
	if (size + (i64)sizeof(_b) > capacity) set_capacity((size + sizeof(_b)) * 2);
	*((_b*)(data + size)) = a;
	size += sizeof(_b);
	return *this;
}

t_b _stack& _stack::operator>>(std::vector<_b>& b) noexcept
{
	uint v;
	*this >> v;
	b.resize(v);
	for (auto& i : b) *this >> i;
	return *this;
}

t_b _stack& _stack::operator>>(_b& a) noexcept
{
	if (adata + (i64)sizeof(_b) > size) return *this;
	a = *((_b*)(data + adata));
	adata += sizeof(_b);
	return *this;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

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

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

inline uint hash_func(void* const& a) { return (uint)((((u64)a) >> 4) * 27644437); }
inline uint hash_func(const u64& a) { return (uint)(a * 27644437); }

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// хэш таблица, с мгновенной очисткой, и любыми данными, но с дополнительной переменной id
t_t struct __hash_table
{
	struct _hash_info
	{
		u64 id = 0; // код занятости
		_t a = {}; // данные
	};

	struct _iterator
	{
		_hash_info* hi;
		__hash_table* ht;

		bool operator!=(const _iterator& b) { return hi != b.hi; }
		_iterator& operator++();
		_t& operator*() const { return hi->a; }
		_t* operator->() const { return &hi->a; }
		bool life() { if (hi->id == ht->id) return false; hi->id = ht->id; ht->size++; return true; } // оживление
		operator bool() { return (hi->id == ht->id); }
	};

	_hash_info* data = nullptr;
	uint capacity = 0;
	uint size = 0; // количество элементов

	~__hash_table() noexcept { delete[] data; }
	void clear() noexcept { id++; size = 0; }
	bool insert(const _t& b) noexcept; // return - элемента небыло? (!!присваивание в любом случае!!)
	bool insert(_t&& b) noexcept; // return - элемента небыло? (!!присваивание в любом случае!!)
	t_b _iterator find(const _b& x); // хэш функции должны одинаковые значания выдавать
	t_b void erase(const _b& x) { erase(find(x)); } // удалить элемент
	void erase(_t* x); // удалить элемент
	void erase(const _iterator& x); // удалить элемент

	_iterator begin() noexcept;
	_iterator end() noexcept { return { (data + capacity), this }; }

private:
	u64 id = 1; // рабочий код занятости

	void reserve() noexcept;
};

t_t void __hash_table<_t>::erase(const _iterator& x)
{
	if (x.hi->id != id) return; // элемента нет
	uint n = (uint)(x.hi - data);
	uint max_i = capacity - 1;
	for (uint j = (n + 1) & max_i; data[j].id == id; j = (j + 1) & max_i)
	{
		uint j0 = hash_func(data[j].a) & max_i;
		if ((j < j0) ^ (n >= j0) ^ (n < j)) continue;
		data[n] = std::move(data[j]);
		n = j;
	}
	data[n].id = 0;
	size--;
}

t_t void __hash_table<_t>::erase(_t* x)
{
	uint n = (uint)(((u64)x - (u64)data) / sizeof(_hash_info));
	if (n < capacity)
		erase(_iterator{ &data[n], this });
	else
		erase(find(*x));
}

t_t t_b typename __hash_table<_t>::_iterator __hash_table<_t>::find(const _b& x)
{
	if (size * 2 >= capacity) reserve(); // 3x - 1.25 итерации, 2x - 1.5 итерации, 1.5x - 2.0 итерации
	uint i;
	uint max_i = capacity - 1;
	for (i = hash_func(x) & max_i; data[i].id == id; i = (i + 1) & max_i) if (data[i].a == x) break;
	return { &data[i], this };
}

t_t void __hash_table<_t>::reserve() noexcept
{ // 3x - 1.25 итерации, 2x - 1.5 итерации, 1.5x - 2.0 итерации
	uint capacity_old = capacity;
	capacity = (capacity) ? capacity * 2 : 2;
	_hash_info* data2 = new _hash_info[capacity]();
	for (uint j = 0; j < capacity_old; j++)
	{
		if (data[j].id != id) continue;
		uint i;
		uint max_i = capacity - 1;
		for (i = hash_func(data[j].a) & max_i; data2[i].id == id; i = (i + 1) & max_i);
		data2[i] = std::move(data[j]);
	}
	delete[] data;
	data = data2;
}

t_t bool __hash_table<_t>::insert(const _t& b) noexcept
{
	_iterator n = find(b);
	n.hi->a = b;
	return n.life();
}

t_t bool __hash_table<_t>::insert(_t&& b) noexcept
{
	_iterator n = find(b);
	n.hi->a = std::move(b);
	return n.life();
}

t_t typename __hash_table<_t>::_iterator __hash_table<_t>::begin() noexcept
{
	for (uint i = 0; i < capacity; i++)	if (data[i].id == id) return { (data + i), this };
	return { (data + capacity), this };
}

t_t typename __hash_table<_t>::_iterator& __hash_table<_t>::_iterator::operator++()
{
	_hash_info* en = (ht->data + ht->capacity);
	for (++hi; hi != en; ++hi) if (hi->id == ht->id) break;
	return *this;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// хэш таблица парных элементов с медленной очисткой, и _t() означает отсутствие элемента, и нельзя добавлять!
template <typename _t, typename _tt>
struct _hash_map
{
	struct _hash_info
	{
		_t a = {}; // данные
		_tt b = {}; // данные
	};

	struct _iterator
	{
		_hash_info* hi;
		_hash_map* ht;

		bool operator!=(const _iterator& b) { return hi != b.hi; }
		_iterator& operator++();
		_tt& operator*() const { return hi->b; }
		_tt* operator->() const { return &hi->b; }
		void  life() { if (!hi->a) ht->size++; } // оживление !! в начале
		operator bool() { return hi->a; }
	};

	_hash_info* data = nullptr;
	uint capacity = 0;
	uint size = 0; // количество элементов

	~_hash_map() noexcept { delete[] data; }
	void insert(_t a, _tt b) noexcept; // нужно ли const& ??
	_iterator find(_t x) noexcept; // поиск места
	void erase(_t b); // удалить элемент

	_iterator begin() noexcept;
	_iterator end() noexcept { return { (data + capacity), this }; }

private:
	void reserve() noexcept;
};

template <typename _t, typename _tt>
void _hash_map<_t, _tt>::insert(_t a, _tt b) noexcept
{
	_iterator n = find(a);
	n.life();
	*n.hi = { a,b };
}

template <typename _t, typename _tt>
typename _hash_map<_t, _tt>::_iterator& _hash_map<_t, _tt>::_iterator::operator++()
{
	_hash_info* en = (ht->data + ht->capacity);
	for (++hi; hi != en; ++hi) if (hi->a) break;
	return *this;
}

template <typename _t, typename _tt>
typename _hash_map<_t, _tt>::_iterator _hash_map<_t, _tt>::find(_t x) noexcept
{
	if (size * 2 >= capacity) reserve(); // 3x - 1.25 итерации, 2x - 1.5 итерации, 1.5x - 2.0 итерации
	uint i;
	uint max_i = capacity - 1;
	for (i = hash_func(x) & max_i; data[i].a; i = (i + 1) & max_i) if (data[i].a == x) break;
	return { &data[i], this };
}

template <typename _t, typename _tt>
void _hash_map<_t, _tt>::reserve() noexcept
{ // 3x - 1.25 итерации, 2x - 1.5 итерации, 1.5x - 2.0 итерации
	uint capacity_old = capacity;
	capacity = (capacity) ? capacity * 2 : 2;
	_hash_info* data2 = new _hash_info[capacity]();
	for (uint j = 0; j < capacity_old; j++)
		if (data[j].a)
		{
			uint i;
			uint max_i = capacity - 1;
			for (i = hash_func(data[j].a) & max_i; data2[i].a; i = (i + 1) & max_i);
			data2[i] = std::move(data[j]);
		}
	delete[] data;
	data = data2;
}

template <typename _t, typename _tt>
typename _hash_map<_t, _tt>::_iterator _hash_map<_t, _tt>::begin() noexcept
{
	for (uint i = 0; i < capacity; i++)	if (data[i].a) return { (data + i), this };
	return { (data + capacity), this };
}

template <typename _t, typename _tt>
void _hash_map<_t, _tt>::erase(_t b)
{
	auto x = find(b);
	if (!x) return;
	uint n = (uint)(x.hi - data);
	uint max_i = capacity - 1;
	for (uint j = (n + 1) & max_i; data[j].a; j = (j + 1) & max_i)
	{
		uint j0 = hash_func(data[j].a) & max_i;
		if ((j < j0) ^ (n >= j0) ^ (n < j)) continue;
		data[n] = std::move(data[j]);
		n = j;
	}
	data[n] = {}; // и a и b должны обнулиться
	size--;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

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

void memset32(uint* destination, uint value, i64 size);

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
