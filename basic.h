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

#define bit16(x) (bit8[(x)&255]+bit8[((x)>>8)&255]) // количество 1-бит в 16-битном числе

constexpr uint color_set[32] = { // набор разных цветов одной яркости
	0xFF0080FD, 0xFFEF0000, 0xFF9E3BFF, 0xFF938700, 0xFF12AA00, 0xFFD34E0D, 0xFF7470DC, 0xFF4D9682,
	0xFFC80FCE, 0xFFB06381,	0xFFE22653, 0xFF009D93, 0xFFC4439D, 0xFF258ECB, 0xFF659B00, 0xFF3F77FF,
	0xFF887E87, 0xFF9659DD, 0xFFB57000, 0xFF39A337, 0xFF5D84BE, 0xFFAF1FFB, 0xFFDE028A, 0xFF768F51,
	0xFFB400FC,	0xFFE23700, 0xFFCB29B2, 0xFFA3755A, 0xFFC45D37, 0xFFEB171B, 0xFF966DA8, 0xFFAC4ACF };

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

inline std::wstring exe_path; // путь к запущенному exe файлу

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define t_t template <typename _t>
#define t_b template <typename _b>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool save_file(wstr fn, const char* data, i64 n);
bool load_file(wstr fn, char** data, i64* n);

std::wstring string_to_wstring(std::string_view s);
wstr         uint64_to_wstr_hex(u64 a);

std::wstring double_to_string(double a, int z);
std::string  double_to_astring(double a, int z);

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct _rnd
{ // (a = 521, b = 353)
	_rnd(i64 p = 0)       noexcept { init(p); }
	u64 operator()()      noexcept { i = (i + 1) & 1023; return (d[i] = d[(i + 503) & 1023] ^ d[(i + 671) & 1023]); }
	i64 operator()(i64 m) noexcept { i = (i + 1) & 1023; return (d[i] = d[(i + 503) & 1023] ^ d[(i + 671) & 1023]) % m;}
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

	_stack(i64 r = 0)              noexcept; // конструктор, r - зарезервировать размер
	_stack(const _stack &a)        noexcept; // конструктор копии
	_stack(_stack&& a)             noexcept; // конструктор переноса
	_stack(void* data2, int vdata) noexcept; // конструктор, инициализация куском памяти
	~_stack() { delete[] data; }

	bool operator==(const _stack& a) const noexcept;

	void clear() { size  = 0; adata = 0; }
	void erase(i64 n, i64 k);
	bool save_to_file(wstr fn);
	bool load_from_file(wstr fn);

	t_b _stack& operator<<(const std::vector<_b>& b) noexcept;
	t_b _stack& operator<<(_b a) noexcept;
	    _stack& operator<<(const _stack& a) noexcept;
	    _stack& operator<<(const std::wstring& a) noexcept;
	    void    push_data(const void* data2, i64 vdata);
	    void    push_fill(int vdata, char c); // занести кучу одинаковых символов
	    void    push_int24(int a);            // записать 3 байта

	    _stack& operator>>(_stack& a) noexcept;
	    _stack& operator>>(std::wstring& s) noexcept;
	t_b _stack& operator>>(std::vector<_b>& b) noexcept;
	t_b _stack& operator>>(_b& a) noexcept;
	    void    pop_data(void* data2, i64 vdata);
	t_b void    pop_end(_b& a); // извлечь переменную из стека С КОНЦА
	    void    pop_int24(int& a);

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

t_b void _stack::pop_end(_b& a)
{
	if (sizeof(_b) > size) return;
	size -= sizeof(_b);
	a = *((_b*)(data + size));
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

struct _ixy // индекс, номер
{
	i64 x, y;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct _xy
{
	double x, y;

	operator _ixy() const noexcept
	{
		_ixy res{ (i64)x, (i64)y };
		if (x < 0.0) if (x != res.x) res.x--;
		if (y < 0.0) if (y != res.y) res.y--;
		return res;
	}

	_xy operator-()         const noexcept { return { -x,  -y }; }

	_xy operator-(_xy b)    const noexcept { return { x - b.x, y - b.y }; }
	_xy operator+(_xy b)    const noexcept { return { x + b.x, y + b.y }; }

	_xy operator*(double b) const noexcept { return { x * b, y * b }; }

	void operator+=(_xy b)        noexcept { x += b.x; y += b.y; }
	void operator-=(_xy b)        noexcept { x -= b.x; y -= b.y; }

	void operator*=(double b)     noexcept { x *= b; y *= b; }
	void operator/=(double b)     noexcept { x /= b; y /= b; }

	double len()            const noexcept { return sqrt(x * x + y * y); }  // длина вектора
	double len2()           const noexcept { return x * x + y * y; } // квадрат длины вектора

	double scalar(_xy b)    const noexcept { return x * b.x + y * b.y; } // скалярное произведение

	_xy rotation(double b)  const noexcept; // поворот
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct _iarea;

struct _isize // [0...x), [0...y)
{
	i64 x = 0, y = 0;

	bool   empty()   const noexcept { return ((x <= 0) || (y <= 0)); }
	i64     square()  const noexcept { return ((x <= 0) || (y <= 0)) ? 0 : (x * y); }
	_xy    center()  const noexcept { return { x * 0.5, y * 0.5 }; }
	_isize correct() const noexcept { if ((x <= 0) || (y <= 0)) return { 0, 0 }; return { x, y }; } // для удобства

	bool operator==(_isize s) const noexcept { return (x == s.x) && (y == s.y); }
	bool operator!=(_isize s) const noexcept { return (x != s.x) || (y != s.y); }
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/*struct _size2 // [0...x], [0...y]
{
	_size x = 0.0, y = 0.0;

	bool empty()   const noexcept { return (x < 0) || (y < 0); }
	_coo2 center() const noexcept { return { x * 0.5, y * 0.5 }; }
};*/

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct _iinterval // [...)
{
	i64 min = 0;
	i64 max = 0;

	void operator&=(const _iinterval& b) noexcept { if (b.min > min) min = b.min; if (b.max < max) max = b.max; }

	i64 size()    const noexcept { return (min < max) ? (max - min) : 0; }
	bool empty() const noexcept { return (max <= min); }
};

struct _iarea
{
	_iinterval x, y; // [...) [...)

	_iarea() = default;
	_iarea(_isize b) : x{ 0, b.x }, y{ 0, b.y } {}
	_iarea(_iinterval x_, _iinterval y_) : x(x_), y(y_) {}

	bool operator!=(_isize b) const noexcept;

	void operator&=(const _iarea& b) noexcept { x &= b.x; y &= b.y; }

	_iarea operator&(const _iarea& b) const noexcept { _iarea c(*this); c &= b; return c; }

	bool empty() const noexcept { return (x.min >= x.max) || (y.min >= y.max); }
	_isize size() const noexcept { if (empty()) return { 0,0 }; return { x.max - x.min, y.max - y.min }; }

	_iarea move(_ixy d) const noexcept { return { {x.min + d.x, x.max + d.x}, {y.min + d.y, y.max + d.y} }; }
};

inline _iarea move(_isize b, _ixy d) { return { {d.x, b.x + d.x}, {d.y, b.y + d.y} }; }

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct _interval // [...]
{
	double min = 1;
	double max = 0;

	operator _iinterval() const noexcept
	{
		_iinterval res{ (i64)min, (i64)max };
		if (min < 0.0) if (min != res.min) res.min--;
		if ((max >= 0.0) || (max == res.max)) res.max++;
		return res;
	}

	double operator()(double k) const noexcept { return min + (max - min) * k; }; // [0..1 -> min..max]
	double get_k(double a) const noexcept { return (a - min) / (max - min); } // [min..max -> 0..1]

	void operator&=(const _interval& b) noexcept { if (b.min > min) min = b.min; if (b.max < max) max = b.max; }

	double length() { return (max > min) ? (max - min) : 0; }
};

constexpr double de_i = 0.0001; // для больших чисел - не правильно!

struct _area
{
	_interval x, y; // [...] [...]

	_area() = default;
	_area(const _area&) = default;
	_area(_interval x_, _interval y_) : x(x_), y(y_) {}
	_area(_isize b) : x{ 0.0, b.x - de_i }, y{ 0.0, b.y - de_i } {}
	_area(_xy b) : x{ b.x, b.x }, y{ b.y, b.y } {}

	void operator=(const _isize b) noexcept { x = { 0, b.x - de_i }; y = { 0, b.y - de_i }; }

	operator _iarea() const noexcept { return { x, y }; }

	bool operator<=(const _area& b) const noexcept;
	bool operator<(const _area& b)  const noexcept; // внутри, грани могут касаться, но не равно
	bool inside(const _area& b)     const noexcept;    // внутри, грани не касаются!

	void operator&=(const _area& b) noexcept { x &= b.x; y &= b.y; }
	void operator+=(const _area& b) noexcept;

	_area operator&(const _area& b) const noexcept { _area c(*this); c &= b;	return c; }
	_area operator+(const _area& b) const noexcept { _area c(*this); c += b; return c; }

	bool empty() const noexcept { return (x.min > x.max) || (y.min > y.max); }
	void clear() noexcept { x = { 1.0, 0.0 }; }

	_area expansion(double b) const noexcept; // расширенная область во все стороны на b
	_area scaling(double b) const noexcept; // промасштабированная область во все стороны в b

	_xy center()       const noexcept { return { (x.max + x.min) * 0.5, (y.max + y.min) * 0.5 }; }
	_xy top_left()     const noexcept { return { x.min, y.min }; } // верхний левый угол
	_xy top_right()    const noexcept { return { x.max, y.min }; } // верхний правый угол
	_xy bottom_left()  const noexcept { return { x.min, y.max }; } // нижний левый угол
	_xy bottom_right() const noexcept { return { x.max, y.max }; } // нижний правый угол

	double radius(); // радиус вписанной окружности
	double min_length() { if (empty()) return 0.0; return std::min(x.max - x.min, y.max - y.min); } // минимальный размер

	_area move(_xy d) const noexcept { return { {x.min + d.x, x.max + d.x}, {y.min + d.y, y.max + d.y} }; }

	bool test(_xy b); // принадлежит ли точка области
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct _trans
{
	double scale = 1.0;
	_xy offset = { 0.0, 0.0 };

	_area operator()(const _area& b) const noexcept; // применение трансформации
	_xy  operator()(const _xy& b)    const noexcept; // применение трансформации
	double operator()(double b)      const noexcept { return scale * b; } // применение трансформации

	_trans operator*(_trans tr)      const noexcept;  // сместить и промасштабировать
	_trans operator/(_trans tr)      const noexcept;  // обратно сместить и промасштабировать

	void operator*=(_trans tr)             noexcept { offset += tr.offset * scale; scale *= tr.scale; }
	void operator/=(_trans tr); // обратно сместить и промасштабировать
	bool operator!=(const _trans& b) const noexcept;

	_trans inverse()                 const noexcept;   // обратная трансформация
	_xy inverse(_xy b)               const noexcept;
	_area inverse(const _area& b)    const noexcept;

	void MasToch(_xy b, double m); // промасштабировать вокруг точки
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

uint test_line(_xy p1, _xy p2, _xy b);

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct _picture
{
	uint* data = nullptr;
	_isize size;
	bool transparent = false;

	_picture() = default;
	explicit _picture(_isize r) noexcept;
	_picture(const _picture&) = delete;
	_picture(_picture&& move) noexcept;
	~_picture() noexcept { delete[] data; }

	_picture& operator=(_picture&& move) noexcept;
	void operator=(const _picture& move) noexcept;

	uint* sl(i64 y) const noexcept { return &data[y * size.x]; }
	void set_area(const _iarea& q) noexcept { area = q & size; }
	bool resize(_isize wh) noexcept;
	void set_transparent() noexcept; // узнать, есть ли прозрачные пиксели
	void clear(uint c = 0xFF000000) noexcept;

	// ниже не проверенные, или не универсальные функции

	void line(_ixy p1, _ixy p2, uint c, bool rep = false); // линия rep - полное замещение цвета
	void lines(_xy p1, _xy p2, double l, uint c); // точная линия заданной толщины
	void text16(i64 x, i64 y, std::string_view st, uint c); // простой текст высотой 16
	void text16n(i64 x, i64 y, astr s, i64 n, uint c); // простой текст высотой 16*n
	static _isize size_text16(std::string_view s, i64 n = 1); // размер текста *n
	void froglif(_xy p, double r, uchar* f, int rf, uint c, uint c2 = 0);

	void fill_circle(_xy p, double r, uint c);
	void fill_ring(_xy p, double r, double d, uint c, uint c2);
	void ring(_xy p, double r, double d, uint c);

	void fill_rectangle(_iarea r, uint c, bool rep = false);
	void fill_rect_d(double x1, double y1, double x2, double y2, uint c); // полупрозрачный пр-к на !!непр-й!! подложке

	void rectangle(_iarea oo, uint c);

	void draw(_ixy r, _picture& bm);
	void stretch_draw(_picture* bm, i64 x, i64 y, double m);
	void stretch_draw_speed(_picture* bm, i64 nXDest, i64 nYDest, double m);

	//	void text0(int x, int y, std::string_view s, int h, uint c, uint bg);

protected:
	_iarea area; // разрешенная область для рисования

	void line_vert_rep_speed(_ixy p, i64 y2, uint c); // вертикальная линия замещения без проверок диапазона

	void fill_rect_rep_speed(_iarea r, uint c); // прямоугольник - просто замена цвета без проверок диапазона
	void fill_rect_transparent_speed(_iarea r, uint c);
	void fill_rect_speed(_iarea r, uint c);
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct _bitmap : public _picture
{
	HBITMAP bitmap2;
	HDC hdc = 0;

	LOGFONT font;
	bool izm_font = true;// шрифт был изменен
	HFONT hfont = 0;
	uint f_c = 0; // цвет шрифта
	uint f_cf = 0; // цвет фона шрифта

	void set_font(wstr name, bool bold);
	void podg_font(int r); // подготовка шрифта в выводу
	void podg_cc(uint c, uint cf); // подготовка цветов к выводу

	explicit _bitmap(int rx3 = 0, int ry3 = 0);
	~_bitmap();

	bool resize(_isize wh);

	void text(int x, int y, std::wstring_view s, int h, uint c, uint bg);
	void text(int x, int y, std::string_view s, int h, uint c, uint bg);
	_isize size_text(std::wstring_view s, int h);
	_isize size_text(std::string_view s, int h);

	void grab_ecran_oo2(HWND hwnd); // украсть часть экрана
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct _hsva // цвет в формате hsv
{
	double h; // цветовой тон
	double s; // насыщенность
	double v; // яркость
	double a; // прозрачность

	_hsva() = default;
	_hsva(uint c) { *this = c; }
	_hsva(const _hsva& c) = default;

	void operator = (uint c);
	_hsva& operator = (const _hsva& c) = default;
	operator uint();
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

extern _bitmap temp_bmp;

_stack& operator<<(_stack& o, _picture const& p);
_stack& operator>>(_stack& o, _picture& p);

inline uint cclow(uint c) { return ((c >> 2) & 0x3F3F3F) + (c & 0xFF000000); }
uint brighten(uint c);

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct _wjson
{
	_wjson(wstr fn);
	~_wjson() { end(); }

	_wjson& str(std::string_view name = "", bool lin = false); // стуктура
	_wjson& arr(std::string_view name = "", bool lin = false); // массив
	_wjson& end(); // конец структуры или массива

	_wjson& add(std::string_view name, bool b) { add_start(name) << (b ? "true" : "false");        return *this; }
	_wjson& add(std::string_view name, char b) { add_start(name) << (i64)b;                        return *this; }
	_wjson& add(std::string_view name, u64 b)  { add_start(name) << b;                             return *this; }
	_wjson& add(std::string_view name, i64 b)  { add_start(name) << b;                             return *this; }
	_wjson& add(std::string_view name, double b) { add_start(name) << b;                           return *this; }
	_wjson& add(std::string_view name, std::string_view b) { add_start(name) << "\"" << b << "\""; return *this; }
	_wjson& add(std::string_view name, astr b) { add_start(name) << "\"" << b << "\"";             return *this; }

	_wjson& add(std::string_view name, std::wstring_view b); //!!!!!
	_wjson& add(std::string_view name, const _multi_string& b);
	_wjson& add(std::string_view name, _trans b);
	_wjson& add(std::string_view name, _xy b);
	_wjson& add(std::string_view name, _area b);
	_wjson& add(std::string_view name, const _picture& b);
	_wjson& add(std::string_view name, _interval b);

	template <typename _t> _wjson& add(_t& b) { return add("", b); }

	_wjson& add_hex(std::string_view name, u64 b);
	_wjson& add_hex(u64 b) { return add_hex("", b); }
	_wjson& add_mem(std::string_view name, void* b, u64 size); // блок памяти в виде строки
	_wjson& add_mem(void* b, u64 size) { return add_mem("", b, size); }

private:
	std::ofstream file;
	bool start = true;
	std::vector<bool> arra;
	std::vector<bool> line; // в одну строку записывать массив, структуру?

	std::ofstream& cout();
	std::ofstream& add_start(std::string_view name);
};

struct _rjson
{
	int error = 0; // если != 0, файл сломан, все функции сразу должны вылетать
	bool null = false;

	_rjson(wstr fn);

	bool obj(std::string_view name = ""); // стуктура
	bool arr(std::string_view name = ""); // массив
	void end(); // конец структуры или массива

	astr      read_string(std::string_view name = "");
	_area     read_area2( std::string_view name = "");
	_trans    read_trans( std::string_view name = "");
	_interval read_area(  std::string_view name = "");

	void read(std::string_view name, _picture& b);
	void read(std::string_view name, _multi_string& b);
	void read(std::string_view name, _xy& b);
	void read(std::string_view name, double& b);
	void read(std::string_view name, i64& b);
	void read(std::string_view name, u64& b);
	void read(std::string_view name, uint& b);
	void read(std::string_view name, char& b);
	void read(std::string_view name, std::wstring& b);

	void read_mem(std::string_view name, void* b, u64 size); // блок памяти в виде строки

private:
	std::ifstream file;
	bool start = true;
	std::vector<bool> arra; // {[{{[[[{, [ = true, { = false

	bool read_start(std::string_view name = "");

	astr read_just_string();
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct _matrix;

struct matrix_column // столбец матрицы
{
	_matrix* m; // указатель на матрицу
	i64 x; // номер столбца

	bool min_max(double* mi, double* ma); // найти диапазон
//	double Delta(size_t y = 1); // разница 2-х элементов [y] - [y-1]
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct _matrix
{
	double* data = nullptr;
	_isize size;

	_matrix() = default;
	_matrix(const _matrix& a) noexcept;
	_matrix(_matrix&& a) noexcept;
//	matrix(size_t ry, size_t rx) noexcept;
//	matrix(size_t ry, size_t rx, double z) noexcept;// заполнить числом z
//	matrix(size_t ry, size_t rx, const std::function<double(size_t, size_t)>& fun) noexcept; // задать из функции fun
//	matrix(size_t ry) noexcept; // вектор
	_matrix(i64 ry, const std::function<double(i64)>& fun) noexcept; // вектор задать из функции
	~_matrix() { delete[] data; }

	void operator=(const _matrix& a) noexcept;
	void operator=(_matrix&& a) noexcept;
//	void operator+=(const matrix& a) noexcept;

//	void set_diagonal_matrix(size_t n, double dz) noexcept; // задать матрицу диагонального вида

	double* operator[](i64 n) { return data + (n * size.x); } // нужная строчка
	void resize(_isize r); // изменить размер матрицы ! данные не сохраняются
//	inline bool empty() const noexcept { return rx_ * ry_ == 0; } // проверка на нулевой размер
//	inline size_t size_x() const noexcept { return rx_; } // размер по x
//	inline size_t size_y() const noexcept { return ry_; } // размер по y
//	inline size_t size() const noexcept { return ry_ * rx_; } // размер
	matrix_column column(i64 x) { return { this, x }; } // столбец матрицы

//	void MinMax(double* mi, double* ma); // найти диапазон

//	matrix transpose() const noexcept; // транспонированная матрица
//	matrix this_mul_transpose() const noexcept; // текущая матрица умножить на транспонированную себя
//	matrix pseudoinverse() const noexcept; // псевдообратная матрица

//	void FindAllEigenVectors(matrix& R, matrix& A) const noexcept; // собственные вектора и собственные числа
	 // линейное предсказание на основе коэффициентов
//	matrix linear_prediction(const matrix& k, size_t start, size_t n, size_t ots = 0) const noexcept;
//	double linear_prediction(const matrix& k) const noexcept; // линейное предсказание на основе коэффициентов

//	matrix operator<<(const matrix& a) const noexcept; // склеить матрицы (количество строк должно быть одинаковым)
//	matrix operator*(const matrix& a) const noexcept;
//	matrix operator-(const matrix& a) const noexcept;

//	void Push(_stack* mem); // сохранить в стек
//	void Pop(_stack* mem); // извлечь из стека
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//matrix get_noise(size_t n, double k); // сгенерировать шум

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
