﻿#pragma once

#include "mathematics.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

constexpr i64 roffer = 20; // предложений продажи, предложений покупки ( ВСЕГО = roffer * 2 );

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct _offer
{
	int price;
	int number;

	bool operator!=(const _offer p) const { return (price != p.price) || (number != p.number); }
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct _offers
{
	_offer offer[roffer];

	bool operator==(const _offers& p) const;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct _supply_and_demand // предложение и спрос
{
	_offers demand; // желающие купить
	_offers supply; // желающие продать
	time_t time;

	bool operator==(const _supply_and_demand& p) const; // время не учитывается при сравнении

	void clear()       noexcept { time = 0; } // метка пустого прайса
	bool empty() const noexcept { return (time == 0); } // проверка на пустоту 
	time_t time_to_minute() const { return time - (time % 60); } // обнулить секунды
	i64 time_hour();
	i64 time_minute();
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct _compression_stock_statistics;

struct _stock_statistics
{
	static constexpr double c_unpak = 0.01; // распаковка цен
	static constexpr double c_pak = 100; // упаковка цен

	void push_back(const _supply_and_demand& c);
	const std::vector<_supply_and_demand>& operator*() const { return sad; }
	const std::vector<_supply_and_demand>* operator->() const { return &sad; }
	const _supply_and_demand& operator[](i64 n) const { return sad[n]; }
	void operator=(_compression_stock_statistics& cs);
	void save_to_file(std::wstring_view fn);
	void load_from_file(std::wstring_view fn);
	i64 size() const { return sad.size(); }

private:
	static const i64 number_thread = 16; // !! 16, не менять!!
	std::vector<_supply_and_demand> sad;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct _compression_stock_statistics
{
	i64 size = 0; // количество записей
	_bit_vector data; // сжатые данные

	_compression_stock_statistics() = default;
	_compression_stock_statistics(const _stock_statistics &ss) { for (auto& i : *ss) add(i); }
	_compression_stock_statistics(std::wstring_view fn) { load_from_file(fn); }

	bool add(const _supply_and_demand& c); // добавить цены (сжать)
	bool read(_supply_and_demand& c); // прочитать цены (расжать)
	void save_to_file(std::wstring_view fn);
	void load_from_file(std::wstring_view fn);
	void push_to(_stack& mem);
	void pop_from(_stack& mem);

private:
	std::vector<_offer> base_buy; // база покупки (первых 20 - последние цены)
	std::vector<_offer> base_sale; // база продажи (первых 20 - последние цены)
	time_t back_time = 0; // время прочитанных цен

	bool add0(const _supply_and_demand& c); // не дельта!
	bool add1(const _supply_and_demand& c); // дельта
	bool add12(const _offer* v1, std::vector<_offer>& v0, i64 izm);
	bool coding_delta_number(i64 a, i64 b);
	i64  decoding_delta_number(i64 a);
	bool read0(_supply_and_demand& c);
	bool read1(_supply_and_demand& c);
	bool read12(_offer* v1, std::vector<_offer>& v0);
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct _cdf // структура частот для сжатия чисел с переменным количеством бит
{
	struct _frequency
	{
		i64   first = 0; // первое число кодируемое
		uchar bit = 0; // количество дополнительных бит
		u64   prefix = 0; // обязательные биты (ограничены 1)
	};

	struct _mapf
	{
		i64    first = 0;  // первое число кодируемое
		uchar  bit = 0;  // количество дополнительных бит
		_mapf* next[2] = {}; // следующие ветви

		~_mapf() { delete next[0]; delete next[1]; }
	};

	std::vector<_frequency> fr; // распредление интервалов
	_mapf frd; // дерево декодирования
	_basic_statistics* bst = nullptr; // для переподбора

	_cdf() = default;
	_cdf(const std::vector<_frequency>& a, _basic_statistics* b = nullptr);

	void clear() { fr.clear(); }
	bool coding(i64 a, _bit_vector& bs) const noexcept; // закодировать число в битовый поток (return false если ошибка)
	i64  decoding(_bit_vector& bs) const noexcept; // декодировать число из битового потока
	void calc(const _statistics& st, i64 n, i64 min_value, i64 max_value); // n - количество интервалов
	void to_clipboard(); // скопировать в буффер обмена
//	double calc_size1(const _statistics& st); // сколько в битах (в среднем) будет весить одно число
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct _cdf3 // структура частот для сжатия малого количества чисел с переменным количеством бит
{
	struct _mapf
	{
		i64    first = 0;  // число кодируемое
		_mapf* next[2] = {}; // следующие ветви

		~_mapf() { delete next[0]; delete next[1]; }
	};

	i64 start = 0;
	std::vector<u64> prefix; // нужный префикс
	_mapf frd; // дерево декодирования
	_basic_statistics* bst = nullptr; // для переподбора

	_cdf3() = default;
	_cdf3(i64 start_, const std::vector<u64>& a, _basic_statistics* b = nullptr);

	void clear() { prefix.clear(); }
	bool coding(i64 a, _bit_vector& bs) const noexcept; // закодировать число в битовый поток (return false если ошибка)
	i64  decoding(_bit_vector& bs) const noexcept; // декодировать число из битового потока
	void calc(const _statistics& st, i64 min_value, i64 max_value); // построить дерево хаффмана
	void to_clipboard(); // скопировать в буффер обмена
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

inline _stock_statistics stock_statistics; // посекундные цены

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
