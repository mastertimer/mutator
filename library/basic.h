#pragma once

#include <vector>
#include <string>
#include <filesystem>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

using uchar  = unsigned char;
using ushort = unsigned short;
using uint   = unsigned int;
using u64    = unsigned long long;
using i64    = long long;

#define t_t template <typename _t>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

constexpr double pi = 3.1415926535897932384626;

inline std::filesystem::path exe_path; // путь к запущенному exe файлу

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

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
inline i64 bit32(uint x) { return bit8[x & 255] + bit8[(x >> 8) & 255] + bit8[(x >> 16) & 255] + bit8[x >> 24]; }

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

constexpr uchar position1_8[256] = {
	0, 1, 2, 2, 3, 3, 3, 3, 4, 4, 4, 4, 4, 4, 4, 4, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
	6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
	7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
	7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
	8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
	8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
	8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
	8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8 };

uchar position1_64(u64 a);

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

constexpr uint hex_to_byte[256] = {
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 0, 0, 0, 0, 0, 0,
	0,10,11,12,13,14,15, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0,10,11,12,13,14,15, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

void memset32(uint* destination, uint value, i64 size);

bool save_file(const std::filesystem::path& fn, const char* data, i64 n);
bool load_file(const std::filesystem::path& fn, char** data, i64* n, i64 spare_bytes = 0);
bool load_file(const std::filesystem::path& fn, std::vector<uchar>& res);

std::wstring string_to_wstring(std::string_view s);

std::wstring double_to_wstring(double a, int z);
std::string  double_to_string(double a, int z);
const wchar_t* uint64_to_wstr_hex(u64 a);

std::wstring substr(std::wstring_view s, i64 n, i64 k); // подстрока которая не кидает исключения
std::string date_to_ansi_string(time_t time);
bool string_to_mem(std::string_view s, void* d, i64 size);

// шаг переменной на графической оси (для графиков функций)
void os_pordis(double min, double max, i64 maxN, double& mi, double& step, double min_step = 0.0);

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct _rnd
{ // (a = 521, b = 353)
	_rnd(i64 p = 0) { init(p); }
	u64 operator()() { i = (i + 1) & 1023; return (d[i] = d[(i + 503) & 1023] ^ d[(i + 671) & 1023]); }
	i64 operator()(i64 m) { i = (i + 1) & 1023; return (d[i] = d[(i + 503) & 1023] ^ d[(i + 671) & 1023]) % m; }
	double operator()(double n, double k) { return ((*this)() & 0xFFFFFFFF) * (k - n) / 0xFFFFFFFF + n; }
	void init(u64 p);

private:
	u64 d[1024]; // предыдущие числа
	u64 i; // последняя позиция
};

inline _rnd rnd;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

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

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct _stack
{
	char* data;
	i64   capacity;
	i64   size;
	i64   adata; // активный указатель

	_stack(i64 r = 0); // r - зарезервировать размер
	_stack(const _stack &a);
	_stack(_stack&& a) noexcept;
	_stack(void* data2, int vdata); // инициализация куском памяти
	~_stack() { delete[] data; }

	bool operator==(const _stack& a) const;

	void clear() { size  = 0; adata = 0; }
	void erase(i64 n, i64 k);
	bool save_to_file(const std::filesystem::path& fn);
	bool load_from_file(const std::filesystem::path& fn);

	t_t _stack& operator<<(const std::vector<_t>& b);
	t_t _stack& operator<<(_t a);
	    _stack& operator<<(const _stack& a);
		_stack& operator<<(const _bit_vector& p);
		_stack& operator<<(const std::wstring& a);
	    void    push_data(const void* data2, i64 vdata);
	    void    push_fill(int vdata, char c); // занести кучу одинаковых символов

	    _stack& operator>>(_stack& a);
		_stack& operator>>(_bit_vector& p);
		_stack& operator>>(std::wstring& s);
	t_t _stack& operator>>(std::vector<_t>& b);
	t_t _stack& operator>>(_t& a);
	    void    pop_data(void* data2, i64 vdata);

	void revert(i64 bytes); // вернуть данные
	void skip(i64 bytes);   // пропустить данные

private:
	void set_capacity(i64 rdata); // изменить размер массива в большую сторону
};

t_t _stack& _stack::operator<<(const std::vector<_t>& b)
{
	*this << (uint)b.size();
	for (auto& i : b) *this << i;
	return *this;
}

t_t _stack& _stack::operator<<(_t a)
{
	if (size + (i64)sizeof(_t) > capacity) set_capacity((size + sizeof(_t)) * 2);
	*((_t*)(data + size)) = a;
	size += sizeof(_t);
	return *this;
}

t_t _stack& _stack::operator>>(std::vector<_t>& b)
{
	uint v;
	*this >> v;
	b.resize(v);
	for (auto& i : b) *this >> i;
	return *this;
}

t_t _stack& _stack::operator>>(_t& a)
{
	if (adata + (i64)sizeof(_t) > size) return *this;
	a = *((_t*)(data + adata));
	adata += sizeof(_t);
	return *this;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
