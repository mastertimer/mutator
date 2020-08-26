#pragma once

#include "basic_go.h"

constexpr int rceni = 20; // предложений продажи, предложений покупки ( ВСЕГО = Rceni * 2 );

struct _offer // предложение
{
	ushort c; // цена
	int k; // количество
};

struct _date_time // дата и время
{
	uchar month, day, hour, minute, second; // (месяц+год), день, час, мин, сек

	operator int(); // преобразовать в целое число
	int to_minute(); // преобразовать в целое число без секунд
	void operator =(int a); // присвоить число
	void now(); // присвоить текущее время
};

struct _prices // массив спроса предложения
{
	_offer pok[rceni]; // предложение покупки (порядок с самого выгодного)
	_offer pro[rceni]; // предложение продажи (порядок с самого выгодного)
	_date_time time; // время

	inline void clear() { time.second = 200; } // метка пустого прайса
	inline bool empty() { return (time.second == 200); } // проверка на пустоту 
};

struct _super_stat // супер статистика цен
{
	struct _info_pak // дополнительная информация для упаковки
	{
		bool ok; // есть данные
		int r; // общий размер
		int r_pro; // размер продаж
		int r_pok; // размер покупок
	};
	int size; // количество записей

	_super_stat();
	void add(_prices& c); // добавить цены (сжать)
	void read(int n, _prices& c, _info_pak* inf = 0); // прочитать цены (расжать)
	void save_to_file(wstr fn);
	void load_from_file(wstr fn);
	void clear(); // удалить все данные

private:
	_prices last_cc; // последние цены
	_stack data; // сжатые данные
	std::vector<int> u_dd2; // указатель на место сжатых данных кратных StepPakCC

	static const int step_pak_cc = 100;
	_prices read_cc; // последние прочитанные цены
	int read_n; // номер последних прочитанных цен
	_info_pak ip_last, ip_n; // дополнительная информация

	void otgruzka(int rez, int Vrez, int* deko); // вспомогательная Pak()
};

struct _g_graph : public _t_go
{
	_g_graph() { local_area = { {0, 100}, {0, 100} }; }

	uchar type()                     override { return 9; }
	int get_froglif()                override { return 0xF6; } // !!!!!

	void ris2(_trans tr, bool final) override;

};
