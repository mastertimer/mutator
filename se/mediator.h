#pragma once

#include "exchange_data.h"
#include "graphics.h"

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
	bool operator< (ushort a) const noexcept { return (mask < a); } // сравнить
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct _recognize // распознавание с экрана
{
	_bitmap image; // картинка для распознавания
	POINT offset = { 0,0 }; // смещение картинки
	std::vector<_area_string> elem; // элементы

	_recognize(); // конструктор
	int read_vvod_zaya(); // найти и распознать окно ввода заявки. возращает ошибку
	int read_vnimanie_prodaza(); // найти и распознать окно подтвержения продажи. возращает ошибку
	int read_vnimanie_pokupka(); // найти и распознать окно подтвержения продажи. возращает ошибку
	int read_okno_soobsenii(); // найти и распознать окно сообщений. возращает ошибку
	void find_text13(uint c); // найти текст высотой 13 с нужным цветом
	void find_text13(uint c, int err); // err - погрешность !!! тест скорости, возможно удалить 2 другие !!!
	int find_elem(std::wstring_view s); // найти текст среди элементов
	int find_elem_kusok(std::wstring_view s); // найти кусок текста среди элементов

private:
	_kusok_bukva bu; // дерево алфавита для распознавания

	std::wstring rasp_text(ushort* aa, i64 vaa); // распознать текст
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

inline _recognize recognize;
inline bool zamok_pokupki = false; // простой блокиратор

void buy_stock(struct _ui_element* tt, bool buy);

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

