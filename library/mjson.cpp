#include <map>
#include <sstream>
#include <iomanip>

#include "mjson.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

_wjson& _wjson::str(std::string_view name, bool lin)
{
	add_start(name) << '{';
	if (lin)
		file << ' ';
	else
		file << std::endl;
	start = true;
	arra.push_back(false);
	line.push_back(lin);
	return *this;
}

_wjson& _wjson::arr(std::string_view name, bool lin)
{
	add_start(name) << '[';
	if (lin)
		file << ' ';
	else
		file << std::endl;
	start = true;
	arra.push_back(true);
	line.push_back(lin);
	return *this;
}

std::ofstream& _wjson::add_start(std::string_view name)
{
	if (!start)
	{
		file << ",";
		if (line.back())
			file << ' ';
		else
			file << std::endl;
	}
	start = false;
	if (!line.back()) cout();
	if (name != "")	file << "\"" << name << "\": ";
	return file;
}

_wjson& _wjson::end()
{
	if (!start)
		if (line.back())
			file << ' ';
		else
			file << std::endl;
	bool a = arra.back();
	arra.pop_back();
	if (!line.back()) cout();
	line.pop_back();
	file << ((a) ? ']' : '}');
	start = false;
	return *this;
}

_wjson::_wjson(wstr fn)
{
	file.open(fn);
	file << std::setprecision(18);
	line.push_back(false);
	str();
}

std::ofstream& _wjson::cout()
{
	for (uint i = 0; i < arra.size(); i++) file << "\t";
	return file;
}

std::string wstring_to_string(std::wstring_view b)
{
	static std::map<wchar_t, char> conv = {
	    // нельзя " и #
	    {L'!', '!'},   {L'$', '$'}, {L'%', '%'}, {L'&', '&'}, {L'(', '('}, {L')', ')'},   {L'*', '*'}, {L'+', '+'},
	    {L',', ','},   {L'-', '-'}, {L'.', '.'}, {L'/', '/'}, {L'0', '0'}, {L'1', '1'},   {L'2', '2'}, {L'3', '3'},
	    {L'4', '4'},   {L'5', '5'}, {L'6', '6'}, {L'7', '7'}, {L'8', '8'}, {L'9', '9'},   {L':', ':'}, {L';', ';'},
	    {L'<', '<'},   {L'=', '='}, {L'>', '>'}, {L'?', '?'}, {L'@', '@'}, {L'A', 'A'},   {L'B', 'B'}, {L'C', 'C'},
	    {L'D', 'D'},   {L'E', 'E'}, {L'F', 'F'}, {L'G', 'G'}, {L'H', 'H'}, {L'I', 'I'},   {L'J', 'J'}, {L'K', 'K'},
	    {L'L', 'L'},   {L'M', 'M'}, {L'N', 'N'}, {L'O', 'O'}, {L'P', 'P'}, {L'Q', 'Q'},   {L'R', 'R'}, {L'S', 'S'},
	    {L'T', 'T'},   {L'U', 'U'}, {L'V', 'V'}, {L'W', 'W'}, {L'X', 'X'}, {L'Y', 'Y'},   {L'Z', 'Z'}, {L'[', '['},
	    {L'\\', '\\'}, {L']', ']'}, {L'^', '^'}, {L'_', '_'}, {L'`', '`'}, {L'\'', '\''}, {L'a', 'a'}, {L'b', 'b'},
	    {L'c', 'c'},   {L'd', 'd'}, {L'e', 'e'}, {L'f', 'f'}, {L'g', 'g'}, {L'h', 'h'},   {L'i', 'i'}, {L'j', 'j'},
	    {L'k', 'k'},   {L'l', 'l'}, {L'm', 'm'}, {L'n', 'n'}, {L'o', 'o'}, {L'p', 'p'},   {L'q', 'q'}, {L'r', 'r'},
	    {L's', 's'},   {L't', 't'}, {L'u', 'u'}, {L'v', 'v'}, {L'w', 'w'}, {L'x', 'x'},   {L'y', 'y'}, {L'z', 'z'},
	    {L'А', 'А'},   {L'Б', 'Б'}, {L'В', 'В'}, {L'Г', 'Г'}, {L'Д', 'Д'}, {L'Е', 'Е'},   {L'Ё', 'Ё'}, {L'Ж', 'Ж'},
	    {L'З', 'З'},   {L'И', 'И'}, {L'Й', 'Й'}, {L'К', 'К'}, {L'Л', 'Л'}, {L'М', 'М'},   {L'Н', 'Н'}, {L'О', 'О'},
	    {L'П', 'П'},   {L'Р', 'Р'}, {L'С', 'С'}, {L'Т', 'Т'}, {L'У', 'У'}, {L'Ф', 'Ф'},   {L'Х', 'Х'}, {L'Ц', 'Ц'},
	    {L'Ч', 'Ч'},   {L'Ш', 'Ш'}, {L'Щ', 'Щ'}, {L'Ъ', 'Ъ'}, {L'Ы', 'Ы'}, {L'Ь', 'Ь'},   {L'Э', 'Э'}, {L'Ю', 'Ю'},
	    {L'Я', 'Я'},   {L'а', 'а'}, {L'б', 'б'}, {L'в', 'в'}, {L'г', 'г'}, {L'д', 'д'},   {L'е', 'е'}, {L'ё', 'ё'},
	    {L'ж', 'ж'},   {L'з', 'з'}, {L'и', 'и'}, {L'й', 'й'}, {L'к', 'к'}, {L'л', 'л'},   {L'м', 'м'}, {L'н', 'н'},
	    {L'о', 'о'},   {L'п', 'п'}, {L'р', 'р'}, {L'с', 'с'}, {L'т', 'т'}, {L'у', 'у'},   {L'ф', 'ф'}, {L'х', 'х'},
	    {L'ц', 'ц'},   {L'ч', 'ч'}, {L'ш', 'ш'}, {L'щ', 'щ'}, {L'ъ', 'ъ'}, {L'ы', 'ы'},   {L'ь', 'ь'}, {L'э', 'э'},
	    {L'ю', 'ю'},   {L'я', 'я'}, {L' ', ' '}};
	static const char  zz[] = "0123456789abcdef";
	std::ostringstream res;
	for (auto i : b)
	{
		char r = conv[i];
		if (r)
		{
			res << r;
			continue;
		}
		ushort e = i;
		res << '#' << zz[(e >> 12) & 15] << zz[(e >> 8) & 15] << zz[(e >> 4) & 15] << zz[e & 15];
	}
	return res.str();
}

constexpr uint hex_to_byte[256] = {
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 0, 0, 0, 0, 0, 0,
	0,10,11,12,13,14,15, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0,10,11,12,13,14,15, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

wstr string_to_wstring2(std::string_view s)
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
	L'п', L'р', L'с', L'т', L'у', L'ф', L'х', L'ц', L'ч', L'ш', L'щ', L'ъ', L'ы', L'ь', L'э', L'ю', L'я' };

	static std::vector<wchar_t> temp;
	temp.clear();

	for (u64 i = 0; i < s.size(); i++)
	{
		char c = s[i];
		if (c != '#')
		{
			temp.push_back(conv[(uchar)c]);
			continue;
		}
		ushort aa = (hex_to_byte[(uchar)s[i + 1]] << 12) + (hex_to_byte[(uchar)s[i + 2]] << 8) + 
			(hex_to_byte[(uchar)s[i + 3]] << 4) + hex_to_byte[(uchar)s[i + 4]];
		i += 4;
		temp.push_back(aa);
	}
	temp.push_back(0);
	return temp.data();
}

_wjson& _wjson::add_hex(std::string_view name, u64 b)
{
	add_start(name) << "0x" << std::hex << b << std::dec;
	return *this;
}

_wjson& _wjson::add(std::string_view name, std::wstring_view b)
{
	add_start(name) << "\"" << wstring_to_string(b) << "\"";
	return *this;
}

_wjson& _wjson::add(std::string_view name, const _multi_string& b)
{
	arr(name);
	for (auto& i : b.line) add(i);
	end();
	return *this;
}

_wjson& _wjson::add(std::string_view name, _trans b)
{
	str(name, true);
	add("scale", b.scale);
	add("offset", b.offset);
	end();
	return *this;
}

_wjson& _wjson::add(std::string_view name, _xy b)
{
	str(name, true);
	add("x", b.x);
	add("y", b.y);
	end();
	return *this;
}

_wjson& _wjson::add(std::string_view name, _area b)
{
	str(name, true);
	add("x", b.x);
	add("y", b.y);
	end();
	return *this;
}

_wjson& _wjson::add(std::string_view name, _interval b)
{
	str(name, true);
	add("min", b.min);
	add("max", b.max);
	end();
	return *this;
}

_wjson& _wjson::add(std::string_view name, const _picture& b)
{
	arr(name);
	for (int j = 0; j < b.size.y; j++) add_mem(&b.data[j * b.size.x], b.size.x * 4ULL);
	end();
	return *this;
}

_wjson& _wjson::add_mem(std::string_view name, void* b, u64 size)
{
	static const char zz[] = "0123456789abcdef";
	uchar*            c    = (uchar*)b;
	std::string       s(size * 2, ' ');
	for (u64 i = 0; i < size; i++)
	{
		s[i * 2]     = zz[(c[i] >> 4) & 15];
		s[i * 2 + 1] = zz[c[i] & 15];
	}
	add(name, s);
	return *this;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

_rjson::_rjson(wstr fn)
{
	file.open(fn);
	if (!file.good()) { error = 55; return; }
	obj();
}

bool _rjson::obj(std::string_view name)
{
	if (!read_start(name)) return false;
	char c = 0;
	file >> c;
	if ((c == ']') || (c == '}'))
	{
		file.unget();
		null = true;
		return false;
	}
	if (c != '{') { error = 1; return false; }
	arra.push_back(false);
	start = true;
	return true;
}

bool _rjson::arr(std::string_view name)
{
	if (!read_start(name)) return false;
	char c = 0;
	file >> c;
	if ((c == ']') || (c == '}'))
	{
		file.unget();
		null = true;
		return false;
	}
	if (c != '[') { error = 2; return false; }
	arra.push_back(true);
	start = true;
	return true;
}

void _rjson::end()
{
	if (error) return;
	bool arr = arra.back();
	arra.pop_back();
	char c = 0;
	file >> c;
	if (c != ((arr) ? ']' : '}')) error = 3;
	null = false;
	start = false;
}

astr _rjson::read_just_string()
{
	if (error) return "";
	char c = 0;
	file >> c;
	if (c != '\"') { error = 4; return ""; }
	static std::vector<char> buff;
	buff.clear();
	while (true)
	{
		char c = 0;
		file.get(c);
		if (!file.good()) { error = 5; return ""; }
		if (c == '\"') break;
		buff.push_back(c);
	}
	buff.push_back(0);
	return buff.data();
}

bool _rjson::read_start(std::string_view name)
{
	if (error || null) return false;
	if (!start)
	{
		char c = 0;
		file >> c;
		if ((c == ']') || (c == '}'))
		{
			file.unget();
			null = true;
			return false;
		}
		if (c != ',') { error = 6; return false; }
	}
	start = false;
	if (name == "") return true;
	std::string s = read_just_string();
	if (s != name) { error = 7; return false; }
	char c = 0;
	file >> c;
	if (c == ':') return true;
	error = 8;
	return false;
}

astr _rjson::read_string(std::string_view name)
{
	if (!read_start(name)) return "";
	return read_just_string();
}

void _rjson::read(std::string_view name, char& b)
{
	i64 a;
	read(name, a);
	if (error || null) return;
	b = (char)a;
}

void _rjson::read(std::string_view name, uint& b)
{
	u64 a;
	read(name, a);
	if (error || null) return;
	b = (uint)a;
}

void _rjson::read(std::string_view name, u64& b)
{
	if (!read_start(name)) return;
	char c = 0;
	file >> c;
	if (c == '0')
	{
		char c2 = file.get();
		if ((c2 == 'x') || (c2 == 'X'))
		{
			file >> std::hex >> b >> std::dec;
			if (!file.good()) error = 9;
			return;
		}
		file.unget();
	}
	file.unget();
	file >> b;
	if (!file.good()) error = 10;
}

void _rjson::read(std::string_view name, i64& b)
{
	if (!read_start(name)) return;
	file >> b;
	if (!file.good()) error = 14;
}

void _rjson::read(std::string_view name, double& b)
{
	if (!read_start(name)) return;
	file >> b;
	if (!file.good()) error = 15;
}

_interval _rjson::read_area(std::string_view name)
{
	_interval res;
	if (!obj(name)) return res;
	read("min", res.min);
	read("max", res.max);
	end();
	return res;
}

_area _rjson::read_area2(std::string_view name)
{
	_area res;
	if (!obj(name)) return res;
	res.x = read_area("x");
	res.y = read_area("y");
	end();
	return res;
}

void _rjson::read(std::string_view name, _xy& b)
{
	if (!obj(name)) return;
	read("x", b.x);
	read("y", b.y);
	end();
}

_trans _rjson::read_trans(std::string_view name)
{
	_trans res;
	if (!obj(name)) return res;
	read("scale", res.scale);
	read("offset", res.offset);
	end();
	return res;
}

bool string_to_mem(std::string_view s, void * d, i64 size)
{
	if (s.size() != size * 2ULL) return false;
	unsigned char* dd = (unsigned char*)d;
	for (u64 i = 0; i < s.size(); i += 2)
		*dd++ = (hex_to_byte[(uchar)s[i]] << 4) + hex_to_byte[(uchar)s[i+1]];
	return true;
}

void _rjson::read(std::string_view name, _multi_string& b)
{
	b.line.clear();
	if (!arr(name)) return;
	while (!error)
	{
		astr s = read_string();
		if (null) break;
		b.line.push_back(string_to_wstring2(s));
	}
	end();
}

void _rjson::read(std::string_view name, _picture& b)
{
	std::vector<std::string> temp;
	if (!arr(name)) return;
	while (!error)
	{
		astr s = read_string();
		if (null) break;
		temp.push_back(s);
	}
	end();
	if (temp.size() == 0) { b.resize({ 0, 0 });	return; }
	int rx = (int)(temp[0].size() / 8);
	b.resize({ rx, (i64)temp.size() });
	for (int j = 0; j < temp.size(); j++)
		if (!string_to_mem(temp[j], &b.data[j * b.size.x], b.size.x * 4))
		{
			error = 12;
			return;
		}
	b.set_transparent();
}

void _rjson::read_mem(std::string_view name, void* b, u64 size)
{
	if (!read_start(name)) return;
	if (!string_to_mem(read_just_string(), b, (int)size)) error = 13;
}

void _rjson::read(std::string_view name, std::wstring& b)
{
	b = string_to_wstring2(read_string(name));
}
