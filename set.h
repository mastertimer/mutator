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

struct _offer2  // предложение с удобными типами, хранится всё равно в сжатом виде
{
	i64 price;  // цена
	i64 number; // количество
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct _date_time // дата и время
{
	uchar month, day, hour, minute, second; // (месяц+год), день, час, мин, сек

	operator int(); // преобразовать в целое число (в секундах)
	int to_minute(); // преобразовать в целое число без секунд (в секундах)
	void operator =(int a); // присвоить число
	void now(); // присвоить текущее время
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct _prices // массив спроса предложения
{
	_offer pok[roffer]; // предложение покупки (порядок с самого выгодного)
	_offer pro[roffer]; // предложение продажи (порядок с самого выгодного)
	_date_time time; // время

	void clear()       noexcept { time.second = 200; } // метка пустого прайса
	bool empty() const noexcept { return (time.second == 200); } // проверка на пустоту
	bool operator==(const _prices& p) const noexcept; // время не учитывается при сравнении
};

struct _prices2 // массив спроса предложения с удобными типами
{
	_offer2 buy[roffer];  // предложение покупки (порядок с самого выгодного)
	_offer2 sale[roffer]; // предложение продажи (порядок с самого выгодного)
	time_t time;         // время

	void clear()       noexcept { time = 0; } // метка пустого прайса
	bool empty() const noexcept { return (time == 0); } // проверка на пустоту 
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
	_prices last_cc; // последние цены
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

constexpr _prices2 cena_zero2 = { {}, {}, 0 };

struct _statistics // статистика цен, сжатая
{
	i64 size = 0; // количество записей
	_prices2 last_cc = cena_zero2; // последние цены

	void add(_prices2& c); // добавить цены (сжать)

private:
	std::vector<uchar> data;
	std::vector<i64> udata; // указатель на место сжатых данных кратных step_pak_cc
	std::vector<i64> baza; // база, от которой считается дельта (+ покупка, - продажа)
	i64 offer0 = 0; // какой цене соответствует baza[0]
	static constexpr i64 step_pak_cc = 100; // период ключевых цен
	uchar byte = 0; // текущий байт
	uchar bit = 0;

	void add0(_prices2& c); // не дельта!
	void add1(_prices2& c); // дельта
	void push1(uchar a); // добавить 1 байт
	void pushn(u64 a, uchar n); // добавить n байт
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
	virtual void draw(i64 n, _area area, _bitmap* bm) = 0; // нарисовать 1 элемент
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
	void draw(i64 n, _area area, _bitmap* bm) override; // нарисовать 1 элемент
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
	void draw(i64 n, _area area, _bitmap* bm) override; // нарисовать 1 элемент
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
	void draw(i64 n, _area area, _bitmap* bm) override; // нарисовать 1 элемент
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
	void draw(i64 n, _area area, _bitmap* bm) override; // нарисовать 1 элемент
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
	void draw(i64 n, _area area, _bitmap* bm) override; // нарисовать 1 элемент
	void recovery() override; // выполнить
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct _set_graph : public _t_go
{
	std::vector<std::unique_ptr<_basic_curve>> curve; // кривая
	bool obn = true; // обновить картинку
	i64 size_el = 6; // размер элемента

	_set_graph();

	uchar type()                     override { return 9; }
	int get_froglif()                override { return 0x71; }

	void ris2(_trans tr, bool final) override;
	void run(_tetron* tt0, _tetron* tt, u64 flags) override;
	bool mouse_down_left2(_xy r) override; // начало перетаскивания
	void mouse_move_left2(_xy r) override; // процесс перетаскивания

private:
	_bitmap bm;
	i64 x_tani = 0; // предыдущая координата x при перетаскивании
	int v_vib = 0; // диапазон полосы прокрутки

	void draw(_isize size);
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
	_matrix a; // данные - 1 или 2 стобца
	std::string caption; // подпись линии
	double width = 1.5; // толщина линии
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct _g_graph : public _t_go
{
	std::vector<_curve> curve;

	_g_graph() { local_area = { {0, 100}, {0, 100} }; }

	uchar type() { return 13; }
	int get_froglif() { return 0xFC; }
	void ris2(_trans tr, bool final) override;

	void add(const _matrix& b, std::string_view s = "");
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void calc_all_prediction(_basic_curve &o, i64& nn, double& kk);
void test_ss(i64 f, std::vector<i64>& k);
i64 test_ss2();
