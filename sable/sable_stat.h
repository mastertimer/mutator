#pragma once

#include "mathematics.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

constexpr i64 roffer = 20; // предложений продажи, предложений покупки ( ВСЕГО = roffer * 2 );

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct _prices // массив спроса предложения с удобными типами
{
	_one_stat buy[roffer];  // предложение покупки (порядок с самого выгодного)
	_one_stat sale[roffer]; // предложение продажи (порядок с самого выгодного)
	time_t    time;         // время

	void clear()       noexcept { time = 0; } // метка пустого прайса
	bool empty() const noexcept { return (time == 0); } // проверка на пустоту 
	bool operator==(const _prices& p) const noexcept; // время не учитывается при сравнении
	bool operator!=(const _prices& p) const noexcept; // время не учитывается при сравнении

	time_t time_to_minute() { return time - (time % 60); } // обнулить секунды
	i64 time_hour();
	i64 time_minute();
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct _sable_stat // статистика цен, сжатая  (в 2 раза меньше, в 3 раза медленней предыдущей версии)
{
	struct _info_pak // дополнительная информация для упаковки
	{
		bool ok = false;   // есть данные
		int r = 0;     // общий размер
		int r_pro = 0; // размер продаж
		int r_pok = 0; // размер покупок
	};

	i64 size = 0; // количество записей
	_prices back{}; // последние цены
	_bit_vector data; // сжатые данные
	static constexpr time_t old_dtime = 160; // разность времени, после которого цены считаются устаревшими
	static constexpr double c_unpak = 0.01; // распаковка цен
	static constexpr double c_pak = 100; // упаковка цен

	bool add(const _prices& c); // добавить цены (сжать)
	bool read(i64 n, _prices& c, _info_pak* inf = nullptr); // прочитать цены (расжать)
	void save_to_file(wstr fn);
	void load_from_file(wstr fn);
	void clear(); // удалить все данные

private:
	std::vector<i64> udata; // указатель на место сжатых данных кратных step_pak_cc
	std::vector<_one_stat> base_buy; // база покупки для записи (первых 20 - последние цены)
	std::vector<_one_stat> base_sale; // база продажи для записи (первых 20 - последние цены)
	std::vector<_one_stat> base_buy_r; // база покупки для чтения (первых 20 - последние цены)
	std::vector<_one_stat> base_sale_r; // база продажи для чтения (первых 20 - последние цены)
	static constexpr i64 step_pak_cc = 100; // период ключевых цен
	_prices read_cc{}; // последние прочитанные цены
	i64 read_n = -666; // номер последних прочитанных цен
	_info_pak ip_last, ip_n; // дополнительная информация

	bool add0(const _prices& c); // не дельта!
	bool add1(const _prices& c); // дельта
	bool add12(const _one_stat* v1, std::vector<_one_stat>& v0, i64 izm);
	bool coding_delta_number(i64 a, i64 b);
	i64  decoding_delta_number(i64 a);
	bool read0(_prices& c);
	bool read1(_prices& c);
	bool read12(_one_stat* v1, std::vector<_one_stat>& v0);
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

inline _sable_stat sss;// сжатые цены

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
