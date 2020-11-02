#pragma once

#include "basic.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

constexpr i64 roffer = 20; // предложений продажи, предложений покупки ( ВСЕГО = roffer * 2 );

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct _offer // предложение
{
	ushort c; // цена
	int k; // количество

	bool operator!=(_offer p) const noexcept { return (c != p.c) || (k != p.k); }
};

struct _one_stat // единица статистики
{
	i64 value;   // значение / цена
	i64 number;  // количество

	void operator=(_offer a) noexcept { value = a.c; number = a.k; }
	bool operator!=(_one_stat a) const noexcept { return ((value != a.value) || (number != a.number)); }
	bool operator!=(_offer a) const noexcept { return ((value != a.c) || (number != a.k)); }
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct _date_time // дата и время
{
	uchar month, day, hour, minute, second; // (месяц+год), день, час, мин, сек

	operator int() const noexcept; // преобразовать в целое число (в секундах)
	int to_minute(); // преобразовать в целое число без секунд (в секундах)
	void operator=(int a); // присвоить число
	void now(); // присвоить текущее время
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct _prices // массив спроса предложения
{
	_offer pok[roffer]; // предложение покупки (порядок с самого выгодного)
	_offer pro[roffer]; // предложение продажи (порядок с самого выгодного)
	_date_time time;    // время

	void clear()       noexcept { time.second = 200; } // метка пустого прайса
	bool empty() const noexcept { return (time.second == 200); } // проверка на пустоту
	bool operator==(const _prices& p) const noexcept; // время не учитывается при сравнении
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct _prices2 // массив спроса предложения с удобными типами
{
	_one_stat buy[roffer];  // предложение покупки (порядок с самого выгодного)
	_one_stat sale[roffer]; // предложение продажи (порядок с самого выгодного)
	time_t    time;         // время

	_prices2() = default;
	_prices2(const _prices& a);

	void clear()       noexcept { time = 0; } // метка пустого прайса
	bool empty() const noexcept { return (time == 0); } // проверка на пустоту 
	bool operator==(const _prices2& p) const noexcept; // время не учитывается при сравнении
	bool operator!=(const _prices& p) const noexcept;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct _bit_vector // вектор с побитовой записью / чтением
{
	std::vector<u64> data;
	uchar bit = 64; // сколько бит заполнено в последнем числе
	i64 bit_read = 0; // позиция бита для чтения

	void push1(u64 a) noexcept; // добавить 1 бит
	void pushn(u64 a, uchar n) noexcept; // добавить n бит
	void pushn1(u64 a) noexcept; // добавить ограниченное количество бит, 1xxxxxxxx
	u64 pop1() noexcept; // прочитать 1 бит
	u64 popn(uchar n) noexcept; // прочитать n бит
	i64 size() const noexcept { return (i64)data.size() * 64 - (64 - bit); }
	void resize(i64 v);

	void save(_stack &mem);
	void load(_stack &mem);
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
	_prices2 last_cc{}; // последние цены
	_bit_vector data; // сжатые данные
	static constexpr time_t old_dtime = 160; // разность времени, после которого цены считаются устаревшими
	static constexpr double c_unpak = 0.01; // распаковка цен

	bool add(const _prices2& c); // добавить цены (сжать)
	bool read(i64 n, _prices2& c, _info_pak* inf = nullptr); // прочитать цены (расжать)
	void save_to_file(wstr fn);
	void load_from_file(wstr fn);

private:
	std::vector<i64> udata; // указатель на место сжатых данных кратных step_pak_cc
	std::vector<_one_stat> base_buy; // база покупки для записи (первых 20 - последние цены)
	std::vector<_one_stat> base_sale; // база продажи для записи (первых 20 - последние цены)
	std::vector<_one_stat> base_buy_r; // база покупки для чтения (первых 20 - последние цены)
	std::vector<_one_stat> base_sale_r; // база продажи для чтения (первых 20 - последние цены)
	static constexpr i64 step_pak_cc = 100; // период ключевых цен
	_prices2 read_cc{}; // последние прочитанные цены
	i64 read_n = -666; // номер последних прочитанных цен
	_info_pak ip_last, ip_n; // дополнительная информация

	bool add0(const _prices2& c); // не дельта!
	bool add1(const _prices2& c); // дельта
	bool add12(const _one_stat* v1, std::vector<_one_stat>& v0, i64 izm);
	bool coding_delta_number(i64 a, i64 b);
	i64  decoding_delta_number(i64 a);
	bool read0(_prices2& c);
	bool read1(_prices2& c);
	bool read12(_one_stat* v1, std::vector<_one_stat>& v0);
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct _basic_statistics // базовая разреженная статистика с нулями
{
	std::vector<i64> data;
	i64 start = 0;

	void push(i64 x); // добавить число в статистику
	i64 number() const noexcept;
	i64 number(i64 be, i64 en) const noexcept; // количество в интервале [be,en)
	i64 operator[](i64 x) const noexcept;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct _statistics // сжатая статистика
{
	std::vector<_one_stat> data;
	typedef std::vector<_one_stat>::iterator _it;
	typedef std::vector<_one_stat>::const_iterator _cit;

	_statistics() = default;
	_statistics(const _basic_statistics& a) { *this = a; }

	void clear() { data.clear(); }
	i64 min_value() const noexcept { return (data.empty()) ? 0 : data.front().value; }
	i64 max_value() const noexcept { return (data.empty()) ? 0 : data.back().value; }
	i64 number(_cit be, _cit en) const noexcept; // количество в интервале [be,en)
	i64 number(i64 be, i64 en) const noexcept; // количество в интервале [be,en)
	i64 number() const noexcept { return number(data.begin(), data.end()); } // общее количество

	i64 first_zero(); // номер первого нулевого элемента начиная со start (-1 если не нашлось)
	i64 number_not_zero() { return data.size(); } // количество значений с ненулевым количеством
	double arithmetic_size(_it be, _it en); // арифметический размер в битах
	double arithmetic_size() { return arithmetic_size(data.begin(), data.end()); }

	void operator=(const _basic_statistics& a);
	void operator+=(const _statistics& a);
	bool operator==(const _statistics& a) const noexcept;

	i64 operator[](i64 n) const noexcept; // value -> number

	_matrix to_matrix(); // два столбца - x и y
	_matrix to_matrix(i64 mi, i64 ma); // два столбца - x и y
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
