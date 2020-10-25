#pragma once

#include <deque>

#include "tetron.h"

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

struct _prices2 // массив спроса предложения с удобными типами
{
	_one_stat buy[roffer];  // предложение покупки (порядок с самого выгодного)
	_one_stat sale[roffer]; // предложение продажи (порядок с самого выгодного)
	time_t time;          // время

	_prices2() = default;
	_prices2(const _prices& a);

	void clear()       noexcept { time = 0; } // метка пустого прайса
	bool empty() const noexcept { return (time == 0); } // проверка на пустоту 
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct _bit_stream // для побитовой записи в вектор
{
	_bit_stream(std::vector<uchar>& da) : data(da) {}
	~_bit_stream();

	void push1(uchar a); // добавить 1 бит
	void pushn(u64 a, uchar n); // добавить n бит
	void pushn1(u64 a); // добавить ограниченное количество бит, 1?????

private:
	std::vector<uchar>& data;
	uchar byte = 0; // текущий байт
	uchar bit = 0;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct _super_stat // супер статистика цен
{
	struct _info_pak // дополнительная информация для упаковки
	{
		bool ok;   // есть данные
		int r;     // общий размер
		int r_pro; // размер продаж
		int r_pok; // размер покупок
	};

	i64 size = 0; // количество записей
	_prices last_cc{}; // последние цены
	static constexpr double c_unpak = 0.01; // распаковка цен

	_super_stat();

	bool operator==(const _super_stat& b) const noexcept { return (size == b.size) && (data == b.data); }

	void add(_prices& c); // добавить цены (сжать)
	void read(i64 n, _prices& c, _info_pak* inf = 0); // прочитать цены (расжать)
	void save_to_file(wstr fn);
	void load_from_file(wstr fn);
	void clear(); // удалить все данные

private:
	_stack data; // сжатые данные
	std::vector<int> u_dd2; // указатель на место сжатых данных кратных StepPakCC

	static const int step_pak_cc = 100;
	_prices read_cc; // последние прочитанные цены
	i64 read_n = -666; // номер последних прочитанных цен
	_info_pak ip_last, ip_n; // дополнительная информация

	void otgruzka(int rez, int Vrez, int* deko); // вспомогательная Pak()
};

struct _sable_stat // статистика цен, сжатая
{
	i64 size = 0; // количество записей
	_prices2 last_cc{}; // последние цены
	std::vector<uchar> data;
	static constexpr time_t old_dtime = 160; // разность времени, после которого цены считаются устаревшими

	bool add(const _prices2& c); // добавить цены (сжать)

private:
	std::vector<i64> udata; // указатель на место сжатых данных кратных step_pak_cc
	std::vector<_one_stat> base_buy; // база покупки
	std::vector<_one_stat> base_sale; // база продажи
	static constexpr i64 step_pak_cc = 100; // период ключевых цен

	bool add0(const _prices2& c, _bit_stream& bs); // не дельта!
	bool add1(const _prices2& c, _bit_stream& bs); // дельта
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct _element_chart // краткая информация элемента графика
{
	int n;         // порядковый номер элемента !!! -1 = нет элемента
	int time;      // время
	double min;    // минимальное значение
	double max;    // максимальное значение
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct _basic_curve // база для кривых и оракулов
{
	virtual     ~_basic_curve() {}

	virtual i64  get_n()                              = 0; // количество элементов
	virtual void get_n_info(i64 n, _element_chart* e) = 0; // получить краткую информацию n-го элемента
	virtual void get_t_info(int t, _element_chart* e) = 0; // получить краткую информацию элемента со временем >= t
	virtual void draw(i64 n, _area area)              = 0; // нарисовать 1 элемент
	virtual void recovery()                           = 0; // восстановить

	virtual i64  prediction() { return 0; }                // отрезок времени роста
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct _mctds_candle : public _basic_curve // источник данных для временного графика - классические свечи
{
	struct _cen_pak // свечка
	{
		ushort min   = 0; // минимальная цена
		ushort max   = 0; // макимальная цена
		ushort first = 0; // первая цена
		ushort last  = 0; // последняя цена
		double cc    = 0; // средняя цена
		_iinterval ncc;   // диапазон цен
		int time     = 0; // общее время
	};

	std::vector<_cen_pak> cen1m; // упакованные цены по минутам

	i64 get_n()                               override { return cen1m.size(); } // количество элементов
	void get_n_info(i64 n, _element_chart* e) override; // получить краткую информацию n-го элемента
	void get_t_info(int t, _element_chart* e) override; // получить краткую информацию элемента со временем >= t
	void draw(i64 n, _area area)              override; // нарисовать 1 элемент
	void recovery()                           override; // обновить
	void push(_stack* mem);
	void pop(_stack* mem);
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct _view_stat : public _basic_curve // тестовое отображение статистических данных
{
	struct _cen_pak // данные по минуте
	{
		int time = 0;   // общее время
		_iinterval ncc; // диапазон цен
		ushort min = 0; // минимальная цена
		ushort max = 0; // макимальная цена
		int k = 0; // количество совпадений

		bool operator < (int a) const noexcept { return (time < a); } // для алгоритма поиска по времени
	};

	std::vector<_cen_pak> cen1m; // упакованные цены по минутам

	i64 get_n()                               override { return cen1m.size(); } // количество элементов
	void get_n_info(i64 n, _element_chart* e) override; // получить краткую информацию n-го элемента
	void get_t_info(int t, _element_chart* e) override; // получить краткую информацию элемента со временем >= t
	void draw(i64 n, _area area)              override; // нарисовать 1 элемент
	void recovery()                           override; // обновить
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct _latest_events // последние события
{
	char event[4]; // [0] - последнее событие
	int minute[4]; // на какой минуте случилось
	double x[4]; // значения

	int start(); // ща будет рост в X минут
	bool stop(); // ща будет падение
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct _nervous_oracle : public _basic_curve // нервозный предсказатель старый
{
	struct _element_nervous // краткая информация элемента графика
	{
		int time       = 0; // время
		_iinterval ncc;     // диапазон цен

		ushort min_pok = 0; // минимальная покупка
		ushort max_pok = 0; // максимальная покупка
		ushort min_pro = 0; // минимальная продажа
		ushort max_pro = 0; // максимальная продажа

		int    v_r     = 0; // количество слагаемых
		double r       = 0; // средний размер
		double r_pok   = 0; // средний размер покупки
		double r_pro   = 0; // средний размер продажи

		bool operator < (int a) const noexcept { return (time < a); } // для алгоритма поиска по времени
	};

	std::vector<_element_nervous> zn; // данные

	i64 get_n() override { return zn.size(); } // количество элементов
	void get_n_info(i64 n, _element_chart* e) override; // получить краткую информацию n-го элемента
	void get_t_info(int t, _element_chart* e) override; // получить краткую информацию элемента со временем >= t
	void draw(i64 n, _area area)              override; // нарисовать 1 элемент
	void recovery() override; // выполнить
	void push(_stack* mem);
	void pop(_stack* mem);
	_latest_events get_latest_events(i64 nn); // получить последние события
	i64 prediction() override;                // отрезок времени роста
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct _nervous_oracle2 : public _basic_curve // нервозный предсказатель новый
{
	struct _element_nervous // краткая информация элемента графика
	{
		int time = 0;       // время
		_iinterval ncc;     // диапазон цен

		ushort min_pok = 0; // минимальная покупка
		ushort max_pok = 0; // максимальная покупка
		ushort min_pro = 0; // минимальная продажа
		ushort max_pro = 0; // максимальная продажа

		int    v_r = 0;     // количество слагаемых
		double r = 0;       // средний размер
		double r_pok = 0;   // средний размер покупки
		double r_pro = 0;   // средний размер продажи

		bool operator < (int a) const noexcept { return (time < a); } // для алгоритма поиска по времени
	};

	std::vector<_element_nervous> zn; // данные

	i64 get_n() override { return zn.size(); } // количество элементов
	void get_n_info(i64 n, _element_chart* e) override; // получить краткую информацию n-го элемента
	void get_t_info(int t, _element_chart* e) override; // получить краткую информацию элемента со временем >= t
	void draw(i64 n, _area area)              override; // нарисовать 1 элемент
	void recovery() override; // выполнить
	void push(_stack* mem);
	void pop(_stack* mem);
	_latest_events get_latest_events(i64 nn); // получить последние события
	i64 prediction() override;                // отрезок времени роста
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct _oracle3 : public _basic_curve // оракул 3-я версия
{
	struct _element_oracle
	{
		int time = 0; // время
		_iinterval ncc; // диапазон цен

		ushort min = 0, max = 0; // разброс по y

		bool operator < (int a) const noexcept { return (time < a); } // для алгоритма поиска по времени
	};
	static const int max_part = 22000; // максимально количество элементов ss

	std::deque<_prices> part_ss; // часть супер-статистики
	i64 begin_ss = 0; // начало куска супер-статистики
	std::vector<_element_oracle> zn; // данные

	i64 get_n() override { return zn.size(); } // количество элементов
	void get_n_info(i64 n, _element_chart* e) override; // получить краткую информацию n-го элемента
	void get_t_info(int t, _element_chart* e) override; // получить краткую информацию элемента со временем >= t
	void draw(i64 n, _area area)              override; // нарисовать 1 элемент
	void recovery() override; // выполнить
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct _sable_graph : public _t_go
{
	std::vector<std::unique_ptr<_basic_curve>> curve; // кривая
	i64 size_el = 6; // размер элемента

	_sable_graph();

	uchar type()                     override { return 9; }
	int get_froglif()                override { return 0x71; }

	void ris2(_trans tr, bool final) override;
	void run(_tetron* tt0, _tetron* tt, u64 flags) override;
	bool mouse_down_left2(_xy r) override; // начало перетаскивания
	void mouse_move_left2(_xy r) override; // процесс перетаскивания

private:
	i64 x_tani = 0; // предыдущая координата x при перетаскивании
	int v_vib = 0; // диапазон полосы прокрутки
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct _area_string // область со строкой
{
	_iarea area;    // область
	std::wstring s; // строка
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct _kusok_bukva // узел дерева кодирования символов
{
	static const int rc = 4; // максимально количество коллизий
	ushort mask = 0; // битовая маска
	std::vector<_kusok_bukva> dalee; // следующий столбец !!!! СЛОЖНО заменить, т.т. использует сортировку
	wchar_t c[rc]{}; // возможные символы
	char f[rc]{}; // соответствующие шрифты
	i64 nbit[rc]{}; // количество бит в символе
	int vc = 0; // количество соответствий

	void cod(ushort* aa, int vaa, wchar_t cc, char nf, i64 nbitt);   // кодирование
	bool operator!=(ushort a) const noexcept { return (mask != a); } // сравнить
	bool operator==(ushort a) const noexcept { return (mask == a); } // сравнить
	bool operator< (ushort a) const noexcept { return (mask <  a); } // сравнить
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct _recognize // распознавание с экрана
{
	_bitmap image; // картинка для распознавания
	POINT offset = { 0,0 }; // смещение картинки
	std::vector<_area_string> elem; // элементы

	_recognize(); // конструктор
	int read_prices_from_screen(_prices* pr); // прочитать цены с экрана (0 - без ошибок)
	int read_vvod_zaya(); // найти и распознать окно ввода заявки. возращает ошибку
	int read_vnimanie_prodaza(); // найти и распознать окно подтвержения продажи. возращает ошибку
	int read_vnimanie_pokupka(); // найти и распознать окно подтвержения продажи. возращает ошибку
	int read_tablica_zayavok(int a, int& b); // количество чисел в таблице заявок
	int read_okno_soobsenii(); // найти и распознать окно сообщений. возращает ошибку
	void find_text13(uint c); // найти текст высотой 13 с нужным цветом
	void find_text13(uint c, int err); // err - погрешность !!! тест скорости, возможно удалить 2 другие !!!
	void find_red_text13(uint err); // найти красный текст высотой 13
	bool find_window_prices(RECT* rr); // координаты окна цен
	int find_elem(std::wstring_view s); // найти текст среди элементов
	int find_elem_kusok(wstr s); // найти кусок текста среди элементов
	int test_image(_prices* pr); // тестовый поиск на готовой картинке

private:
	_kusok_bukva bu; // дерево алфавита для распознавания

	std::wstring rasp_text(ushort* aa, i64 vaa); // распознать текст
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct _curve
{
	_matrix     a;             // данные - 1 или 2 стобца
	std::string caption;       // подпись линии
	bool        bar   = false; // гистограмма
	double      width = 1.5;   // толщина линии
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct _g_graph : public _t_go
{
	std::vector<_curve> curve;

	_g_graph() { local_area = { {0, 100}, {0, 100} }; }

	uchar type() { return 13; }
	int get_froglif() { return 0xFC; }
	void ris2(_trans tr, bool final) override;

	void add(const _matrix& b, std::string_view s = "", bool bar = false);
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

	void operator=(const _basic_statistics &a);
	void operator+=(const _statistics& a);
	bool operator==(const _statistics& a) const noexcept;

	i64 operator[](i64 n) const noexcept; // value -> number

	_matrix to_matrix(); // два столбца - x и y
	_matrix to_matrix(i64 mi, i64 ma); // два столбца - x и y
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct _up_statistics // для удобства поиска количества по значению при обходе по возрастанию
{
	_statistics* st;

	_up_statistics(_statistics& s) : st(&s), li(s.data.begin()), last_value(s.min_value()) {}
	i64 operator[](i64 n) noexcept;
	i64 number_from(i64 start = 0) noexcept; // общее количество (начиная со start)
	double arithmetic_size(_iinterval o); // арифметический размер в битах

private:
	std::vector<_one_stat>::iterator li;
	i64 last_value;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct _sable_statistics
{
	_statistics buyn_number[roffer];  // статистика для каждого расстояния
	_statistics salen_number[roffer]; // статистика для каждого расстояния
	_statistics buy_number;           // общая статистика для покупки
	_statistics sale_number;          // общая статистика для продажи
	_statistics number;               // общая статистика
	_statistics delta;                // разница цен на границе раздела
	_statistics delta_all;            // вся разница цен в списках (кроме границы раздела)

	void calc();                      // вычислить всю статистику
	void clear();                     // очистить все данные
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

	std::vector<_frequency> fr; // распредление интервалов

	_cdf() = default;
	_cdf(const std::vector<_frequency>& a);

	void clear() { fr.clear(); }
	bool coding(i64 a, _bit_stream& bs) const noexcept; // закодировать число в битовый поток (return false если ошибка)
	void calc(const _statistics& st, i64 n, i64 min_value, i64 max_value); // n - количество интервалов
	void to_clipboard(); // скопировать в буффер обмена
//	double calc_size1(const _statistics& st); // сколько в битах (в среднем) будет весить одно число
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct _cdf2 // структура частот для сжатия чисел с переменным количеством бит
{
	struct _frequency
	{
		i64   first  = 0; // первое число кодируемое
		uchar bit    = 0; // количество дополнительных бит
		uchar bit0   = 0; // количество обязательных бит
		u64   prefix = 0; // обязательные биты
	};

	std::vector<_frequency> fr; // распредление интервалов

	_cdf2() = default;
	_cdf2(const std::vector<_frequency> & a);

	void clear() { fr.clear(); }
	bool coding(i64 a, _bit_stream& bs) const noexcept; // закодировать число в битовый поток (return false если ошибка)
	void calc(const _statistics& st, i64 n, i64 min_value, i64 max_value); // n - количество интервалов
	void to_clipboard(); // скопировать в буффер обмена
	double calc_size1(const _statistics& st); // сколько в битах (в среднем) будет весить одно число
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct _cdf3 // структура частот для сжатия малого количества чисел с переменным количеством бит
{
	i64 start = 0;
	std::vector<u64> prefix; // нужный префикс

	_cdf3() = default;
	_cdf3(i64 start_, const std::vector<u64> & a) : start(start_), prefix(a) {}

	void clear() { prefix.clear(); }
	bool coding(i64 a, _bit_stream & bs) const noexcept; // закодировать число в битовый поток (return false если ошибка)
	void calc(const _statistics & st, i64 min_value, i64 max_value); // построить дерево хаффмана
	void to_clipboard(); // скопировать в буффер обмена
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

inline _basic_statistics research1; // для исследования статистики
inline _basic_statistics research2; // для исследования статистики

void calc_all_prediction(_basic_curve &o, i64& nn, double& kk);
double test_ss4();
void unpak_cen();

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
