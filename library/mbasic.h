#pragma once

#include <cstring>
#include <filesystem>
#include <string>
#include <vector>
#include <sstream>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define t_t template <typename _t>
#define t_b template <typename _b>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

constexpr double pi = 3.1415926535897932384626;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

using int8          = short;
using int16         = short;
using int32         = int;
using int64         = long long;

using uint8         = unsigned char;
using uint16        = unsigned short;
using uint32        = unsigned int;
using uint64        = unsigned long long;

using uchar         = unsigned char;
using ushort        = unsigned short;
using uint          = unsigned int;

using astr          = const char*;
using wstr          = const wchar_t*;

using _string       = std::string;
using _wstring      = std::wstring;
using _wstring_view = std::wstring_view;
using _string_view  = std::string_view;
using _path         = std::filesystem::path;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

enum class _cursor { normal, size_all, hand_point, size_we, size_ns, drag }; // виды курсора

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void set_cursorx(_cursor x); // !!! разобраться, где разместить

bool save_file(_path fn, const void* data, uint n);
bool load_file(_path fn, char** data, uint* n);

_wstring string_to_wstring(std::string_view s);
_wstring u8string_to_wstring(std::string s);
wstr     uint64_to_wstr_hex(uint64 a);

_wstring double_to_string(double a, int z);

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct _prng
{
	_prng(int64 p = 0) { init(p); }
	void init(uint64 p);
	uint64 random()
	{
		i = (i + 1) & 1023;
		return d[i] = d[(i + 503) & 1023] ^ d[(i + 671) & 1023]; // a = 521, b = 353
	}
	int64 random(int64 m)
	{
		i = (i + 1) & 1023;
		return (d[i] = d[(i + 503) & 1023] ^ d[(i + 671) & 1023]) % m; // a = 521, b = 353
	}

private:
	uint64 d[1024]; // предыдущие числа
	uint64 i; // последняя позиция
};

inline _prng prng;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct _stack
{
	char*  data;
	uint64 capacity;
	uint64 size;
	uint64 adata; // активный указатель

	_stack(int64 r = 0);              // конструктор, r - зарезервировать размер
	_stack(void* data2, int vdata); // конструктор, инициализация куском памяти
	~_stack() { delete[] data; }
	void clear()
	{
		size  = 0;
		adata = 0;
	}
	void erase(uint64 N, uint64 K);
	bool save_to_file(_path fn);
	bool load_from_file(_path fn);

	t_b _stack& operator<<(const std::vector<_b>& b) noexcept;
	t_b _stack& operator<<(_b a) noexcept;
	_stack&    operator<<(const _stack& a) noexcept;
	_stack&    operator<<(const _wstring& a) noexcept;
	void       push_data(const void* data2, uint64 vdata);
	void       push_fill(int vdata, char c); // занести кучу одинаковых символов
	void       push_int24(int a);            // записать 3 байта

	_stack&    operator>>(_stack& a) noexcept;
	_stack&    operator>>(_wstring& s) noexcept;
	t_b _stack& operator>>(std::vector<_b>& b) noexcept;
	t_b _stack& operator>>(_b& a) noexcept;
	void       pop_data(void* data2, uint64 vdata);
	t_b void    pop_end(_b& a); // извлечь переменную из стека С КОНЦА
	void       pop_int24(int& a);

	void revert(size_t bytes); // вернуть данные
	void skip(size_t bytes);   // пропустить данные

  private:
	void set_capacity(uint64 rdata); // изменить размер массива в большую сторону
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct _multi_string
{
	std::vector<std::wstring> line;

	_multi_string() : line(1) {}
	void push(_stack* mem) { *mem << line; }
	void pop(_stack* mem) { *mem >> line; }
	bool delete_char(int y, int x);
	void insert_char(int y, int x, wchar_t c);
	void div2line(int y, int x);
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

t_b _stack& _stack::operator<<(const std::vector<_b>& b) noexcept
{
	*this << (uint)b.size();
	for (auto& i : b) *this << i;
	return *this;
}

t_b _stack& _stack::operator<<(_b a) noexcept
{
	if (size + sizeof(_b) > capacity) set_capacity((size + sizeof(_b)) * 2);
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
	if (adata + sizeof(_b) > size) return *this;
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
