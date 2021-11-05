#include "basic.h"

#define NOMINMAX
#include <windows.h>
#include <fstream>

i64 position1_64(u64 a)
{ // **
	if (a >> 32)
	{
		if (a >> 48)
		{
			if (i64 k = position1_8[a >> 56]) return k + 56;
			return position1_8[(a >> 48) & 255] + 48;
		}
		if (i64 k = position1_8[(a >> 40) & 255]) return k + 40;
		return position1_8[(a >> 32) & 255] + 32;
	}
	if (a & 0xffff0000ULL)
	{
		if (i64 k = position1_8[(a >> 24) & 255]) return k + 24;
		return position1_8[(a >> 16) & 255] + 16;
	}
	if (i64 k = position1_8[(a >> 8) & 255]) return k + 8;
	return position1_8[a & 255];
}

void set_clipboard_text(std::wstring_view text)
{
	if (OpenClipboard(0))//открываем буфер обмена
	{
		HGLOBAL hgBuffer;
		char* chBuffer;
		EmptyClipboard(); //очищаем буфер
		size_t ll = text.size() * 2 + 2;
		hgBuffer = GlobalAlloc(GMEM_DDESHARE, ll);//выделяем память
		if (!hgBuffer) goto end;
		chBuffer = (char*)GlobalLock(hgBuffer); //блокируем память
		if (!chBuffer) goto end;
		memcpy(chBuffer, text.data(), ll);
		GlobalUnlock(hgBuffer);//разблокируем память
		SetClipboardData(CF_UNICODETEXT, hgBuffer);//помещаем текст в буфер обмена
	end:
		CloseClipboard(); //закрываем буфер обмена
	}
}

void set_clipboard_text(astr text)
{
	if (OpenClipboard(0))//открываем буфер обмена
	{
		HGLOBAL hgBuffer;
		char* chBuffer;
		EmptyClipboard(); //очищаем буфер
		size_t ll = strlen(text) + 1;
		hgBuffer = GlobalAlloc(GMEM_DDESHARE, ll);//выделяем память
		if (!hgBuffer) goto end;
		chBuffer = (char*)GlobalLock(hgBuffer); //блокируем память
		if (!chBuffer) goto end;
		strcpy_s(chBuffer, ll, LPCSTR(text));
		GlobalUnlock(hgBuffer);//разблокируем память
		SetClipboardData(CF_TEXT, hgBuffer);//помещаем текст в буфер обмена
	end:
		CloseClipboard(); //закрываем буфер обмена
	}
}

bool save_file(std::wstring_view fn, const char* data, i64 n)
{
	std::ofstream f(fn.data(), std::ofstream::binary);
	if (!f) return false;
	f.write((char*)data, n);
	return f.good();
}

bool load_file(std::wstring_view fn, char** data, i64* n)
{
	*data = 0;
	*n    = 0;
	std::ifstream f(fn.data(), std::ofstream::binary);
	if (!f) return false;
	f.seekg(0, f.end);
	auto siz = f.tellg();
	*data    = new char[siz];
	f.seekg(0);
	f.read(*data, siz);
	if (!f.good())
	{
		delete[] * data;
		*data = 0;
		return false;
	}
	*n = siz;
	return true;
}

bool load_file(std::wstring_view fn, std::vector<uchar>& res)
{
	res.clear();
	std::ifstream f(fn.data(), std::ofstream::binary);
	if (!f) return false;
	f.seekg(0, f.end);
	auto siz = f.tellg();
	res.resize(siz);
	f.seekg(0);
	f.read((char*)res.data(), siz);
	if (!f.good())
	{
		res.clear();
		return false;
	}
	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool _stack::operator==(const _stack& a) const
{
	if (size != a.size) return false;
	return (memcmp(data, a.data, size) == 0);
}

_stack::_stack(i64 r)
{
	data     = (r) ? new char[r] : 0;
	capacity = r;
	size     = 0;
	adata    = 0;
}

_stack::_stack(const _stack& a) : capacity(a.size), size(a.size), adata(a.adata)
{
	data = new char[capacity];
	memcpy(data, a.data, size);
}

_stack::_stack(_stack&& a) : data(a.data), capacity(a.capacity), size(a.size), adata(a.adata)
{
	a.data = nullptr;
	a.capacity = a.size = a.adata = 0;
}

_stack::_stack(void* data2, int vdata)
{
	adata    = 0;
	size     = vdata;
	capacity = vdata;
	data     = new char[capacity];
	memcpy(data, data2, size);
}

void _stack::erase(i64 n, i64 k)
{
	if ((n < 0) || (n >= size)) return;
	if ((k <= 0) || (n + k > size)) return;
	size -= k;
	adata = 0;
	if (n == size) return;
	memmove(&data[n], &data[n + k], size - n);
}

bool _stack::save_to_file(std::wstring_view fn)
{ 
	return save_file(fn, data, size);
}

bool _stack::load_from_file(std::wstring_view fn)
{
	if (data) delete[] data;
	data     = 0;
	capacity = 0;
	size     = 0;
	adata    = 0;
	i64 ll   = 0;
	if (!load_file(fn, &data, &ll)) return false;
	size = capacity = ll;
	return true;
}

_stack& _stack::operator<<(const _stack& a)
{
	*this << a.size;
	push_data(a.data, a.size);
	return *this;
}

_stack& _stack::operator<<(const std::wstring& a)
{
	uint l = (uint)a.size();
	*this << l;
	push_data(a.data(), l * 2ULL);
	return *this;
}

void _stack::push_data(const void* data2, i64 vdata)
{
	if (size + vdata > capacity) set_capacity((size + vdata) * 2);
	memcpy(data + size, data2, vdata);
	size += vdata;
}

void _stack::push_fill(int vdata, char c)
{
	if (size + vdata > capacity) set_capacity((size + vdata) * 2);
	memset(data + size, c, vdata);
	size += vdata;
}

_stack& _stack::operator>>(_stack& a)
{
	u64 v = 0;
	*this >> v;
	a.size  = 0;
	a.adata = 0;
	if (v == 0) return *this;
	a.set_capacity(v);
	pop_data(a.data, v);
	a.size = v;
	return *this;
}

_stack& _stack::operator>>(std::wstring& s)
{
	uint v = 0;
	*this >> v;
	s.resize(v);
	if (v > 0) pop_data(&s[0], v * 2ULL);
	return *this;
}

void _stack::pop_data(void* data2, i64 vdata)
{
	if (adata + vdata > size) return;
	memcpy(data2, data + adata, vdata);
	adata += vdata;
}

void _stack::revert(i64 bytes)
{
	if (adata < bytes)
		adata = 0;
	else
		adata -= bytes;
}
void _stack::skip(i64 bytes)
{
	if (adata + bytes > size)
		adata = size;
	else
		adata += bytes;
}

void _stack::set_capacity(i64 rdata)
{
	if (rdata <= capacity) return;
	capacity    = rdata;
	char* data2 = new char[capacity];
	if (size) memcpy(data2, data, size);
	delete[] data;
	data = data2;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

std::wstring string_to_wstring(std::string_view s)
{
		static const wchar_t conv[256] = {
		L'\x0', L'\x1', L'\x2', L'\x3', L'\x4', L'\x5', L'\x6', L'\x7', L'\x8', L'\x9', L'\xa', L'\xb', L'\xc', L'\xd',
		L'\xe', L'\xf', L'\x10', L'\x11', L'\x12', L'\x13', L'\x14', L'\x15', L'\x16', L'\x17', L'\x18', L'\x19',
		L'\x1a', L'\x1b', L'\x1c', L'\x1d', L'\x1e', L'\x1f', L' ', L'!', L'\"', L'#', L'$', L'%', L'&', L'\'', L'(',
		L')', L'*', L'+', L',', L'-', L'.', L'/', L'0', L'1', L'2', L'3', L'4', L'5', L'6', L'7', L'8', L'9', L':',
		L';', L'<', L'=', L'>', L'?', L'@', L'A', L'B', L'C', L'D', L'E', L'F', L'G', L'H', L'I', L'J', L'K', L'L',
		L'M', L'N', L'O', L'P', L'Q', L'R', L'S', L'T', L'U', L'V', L'W', L'X', L'Y', L'Z', L'[', L'\\', L']', L'^',
		L'_', L'`', L'a', L'b', L'c', L'd', L'e', L'f', L'g', L'h', L'i', L'j', L'k', L'l', L'm', L'n', L'o', L'p',
		L'q', L'r', L's', L't', L'u', L'v', L'w', L'x', L'y', L'z', L'{', L'|', L'}', L'~', L' ', L' ', L' ', L' ',
		L' ', L' ', L' ', L' ', L' ', L' ', L' ', L' ', L' ', L' ', L' ', L' ', L' ', L' ', L' ', L' ', L' ', L' ',
		L' ', L' ', L' ', L' ', L' ', L' ', L' ', L' ', L' ', L' ', L' ', L' ', L' ', L' ', L' ', L' ', L' ', L' ',
		L' ', L'Ё', L' ', L' ', L' ', L' ', L' ', L' ', L' ', L' ', L' ', L' ', L' ', L' ', L' ', L' ', L' ', L'ё',
		L' ', L' ', L' ', L' ', L' ', L' ', L' ', L'А', L'Б', L'В', L'Г', L'Д', L'Е', L'Ж', L'З', L'И', L'Й', L'К',
		L'Л', L'М', L'Н', L'О', L'П', L'Р', L'С', L'Т', L'У', L'Ф', L'Х', L'Ц', L'Ч', L'Ш', L'Щ', L'Ъ', L'Ы', L'Ь',
		L'Э', L'Ю', L'Я', L'а', L'б', L'в', L'г', L'д', L'е', L'ж', L'з', L'и', L'й', L'к', L'л', L'м', L'н', L'о',
		L'п', L'р', L'с', L'т', L'у', L'ф', L'х', L'ц', L'ч', L'ш', L'щ', L'ъ', L'ы', L'ь', L'э', L'ю', L'я'};
		std::wstring res(s.size(), L' ');
	for (int i = 0; i < s.size(); i++) res[i] = conv[(uchar)s[i]];
	return res;
}

std::wstring substr(std::wstring_view s, i64 n, i64 k)
{
	if (n >= (i64)s.size()) return L"";
	if (n < 0)
	{
		k += n;
		n = 0;
	}
	if (k < 0) k = 0;
	if (n + k > (i64)s.size()) k = s.size() - n;
	return std::wstring(s.data() + n, k);
}

std::wstring double_to_wstring(double a, int z)
{
	static const double delta[16] = {5E-1, 5E-2,  5E-3,  5E-4,  5E-5,  5E-6,  5E-7,  5E-8,
	                                 5E-9, 5E-10, 5E-11, 5E-12, 5E-13, 5E-14, 5E-15, 5E-16};
	std::wstring        s;
	if (a < 0)
	{
		a = -a;
		s = L"-";
	}
	if (z < 16) a += delta[z];
	i64 b = static_cast<i64>(a);
	s += std::to_wstring(b);
	if (z < 1) return s;
	int l = (int)s.size();
	s.resize(1LL + l + z);
	s[l] = L'.';
	for (int i = 1; i <= z; i++)
	{
		a -= b;
		a *= 10;
		b               = static_cast<i64>(a);
		s[(i64)l + i] = L'0' + (short)b;
	}
	return s;
}

std::string double_to_string(double a, int z)
{
	static const double delta[16] = { 5E-1, 5E-2, 5E-3, 5E-4, 5E-5, 5E-6, 5E-7, 5E-8, 5E-9, 5E-10, 5E-11, 5E-12, 5E-13,
		5E-14, 5E-15, 5E-16 };
	std::string s;
	if (a < 0)
	{
		a = -a;
		s = "-";
	}
	if (z < 16)	a += delta[z];
	i64 b = static_cast<i64>(a);
	s += std::to_string(b);
	if (z < 1) return s;
	int l = (int)s.size();
	s.resize(1i64 + l + z);
	s[l] = '.';
	for (int i = 1; i <= z; i++)
	{
		a -= b;
		a *= 10;
		b = static_cast<i64>(a);
		s[(i64)l + i] = '0' + (char)b;
	}
	return s;
}

wstr uint64_to_wstr_hex(u64 a)
{
	static const wchar_t zz[] = L"0123456789abcdef";
	static wchar_t       s[]  = L"1234567890123456";
	int                  k0   = 0;
	for (int i = 15; i >= 0; i--)
	{
		int b = a & 15;
		s[i]  = zz[b];
		a >>= 4;
		if (b == 0)
			k0++;
		else
			k0 = 0;
	}
	if (k0 == 16) k0 = 15;
	return (s + k0);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void memset32(uint* destination, uint value, i64 size)
{
	for (i64 i = 0; i < size; i++) destination[i] = value;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
