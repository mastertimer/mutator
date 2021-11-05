#pragma once

#include <vector>
#include <string>

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

void memset32(uint* destination, uint value, i64 size);

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

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

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
