#pragma once

#include "basic.h"

#include <fstream>

struct _trans;
struct _xy;
struct _area;
struct _interval;
struct _picture;
struct _multi_string;

struct _wjson
{
	_wjson(std::wstring_view fn);
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
	_wjson& add(std::string_view name, const _picture& b, uint c00, uint cc1);
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

	_rjson(std::wstring_view fn);

	bool obj(std::string_view name = ""); // стуктура
	bool arr(std::string_view name = ""); // массив
	void end(); // конец структуры или массива

	astr      read_string(std::string_view name = "");
	_area     read_area2( std::string_view name = "");
	_trans    read_trans( std::string_view name = "");
	_interval read_area(  std::string_view name = "");

	void read(std::string_view name, _picture& b, uint c00, uint cc1);
	void read(std::string_view name, _multi_string& b);
	void read(std::string_view name, _xy& b);
	void read(std::string_view name, double& b);
	void read(std::string_view name, i64& b);
	void read(std::string_view name, u64& b);
	void read(std::string_view name, uint& b);
	void read(std::string_view name, char& b);
	void read(std::string_view name, bool& b);
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
