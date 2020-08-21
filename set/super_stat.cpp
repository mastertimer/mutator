#define NOMINMAX
#include <windows.h>

#include "super_stat.h"

_super_stat ss; // сжатые цены

constexpr _prices cena_zero_ = { {}, {}, { 1,1,1,1,1 } };

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void _date_time::now()
{
#if TARGET_SYSTEM_WINDOWS
	SYSTEMTIME t;
	GetLocalTime(&t);
	month = t.wMonth + (t.wYear - 2017) * 12;
	day = (uchar)t.wDay;
	hour = (uchar)t.wHour;
	minute = (uchar)t.wMinute;
	second = (uchar)t.wSecond;
#elif TARGET_SYSTEM_LINUX
	using namespace std::chrono;
	auto t_ = system_clock::to_time_t(system_clock::now());
	std::tm t;
	t = *localtime(&t_);
	month = t.tm_mon + (t.tm_year - 117) * 12;
	day = (uchar)t.tm_mday;
	hour = (uchar)t.tm_hour;
	minute = (uchar)t.tm_min;
	second = (uchar)t.tm_sec;
#endif
}

void _date_time::operator =(int a)
{
	month = (uchar)(a / 2764800L);
	day = (uchar)((a -= month * 2764800L) / 86400L);
	hour = (uchar)((a -= day * 86400L) / 3600L);
	minute = (uchar)((a -= hour * 3600L) / 60L);
	second = (uchar)(a - minute * 60L);
}

int _date_time::to_minute()
{
	return (((month * 32L + day) * 24L + hour) * 60L + minute) * 60L;
}

_date_time::operator int()
{
	return (((month * 32L + day) * 24L + hour) * 60L + minute) * 60L + second;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void _super_stat::save_to_file(wstr fn)
{
	_stack mem;
	mem << data;
	mem << size;
	mem.push_data(&last_cc, sizeof(last_cc));
	mem << u_dd2;
	mem.save_to_file(fn);
}

void _super_stat::load_from_file(wstr fn)
{
	_stack mem;
	if (!mem.load_from_file(fn)) return;
	mem >> data;
	mem >> size;
	mem >> last_cc;
	mem >> u_dd2;
	read_cc = cena_zero_;
	read_n = -1;
	ip_last.ok = false;
	ip_n.ok = false;
}

void _super_stat::clear()
{
	data.clear();
	u_dd2.clear();
	last_cc = cena_zero_;
	read_cc = cena_zero_;
	size = 0;
	read_n = -1;
	ip_last.ok = false;
	ip_n.ok = false;
}

void _super_stat::otgruzka(int rez, int Vrez, int* deko)
{
	if (Vrez == 0) return;
	uchar a = (rez << 6) + Vrez - 1;
	data << a;
	if (rez == 1)
		for (int i = 0; i < Vrez; i++)
		{
			char b = deko[i];
			data << b;
		}
	if (rez == 2)
		for (int i = 0; i < Vrez; i++)
		{
			short b = deko[i];
			data << b;
		}
	if (rez == 3)
		for (int i = 0; i < Vrez; i++)
			data.push_int24(deko[i]);
}

void _super_stat::read(int n, _prices& c, _info_pak* inf)
{
	if (inf) inf->ok = false;
	if ((n < 0) || (n >= size)) return;
	if (n == read_n)
	{
		c = read_cc;
		if (inf) *inf = ip_n;
		return;
	}
	if (n == size - 1)
	{
		if (inf) *inf = ip_last;
		c = last_cc;
		return;
	}
	if (read_n + 1 != n)
	{
		int k = n / step_pak_cc;
		if ((read_n > n) || (read_n <= k * step_pak_cc - 1))
		{
			data.adata = u_dd2[k];
			read_n = k * step_pak_cc - 1;
		}
		while (read_n < n) read(read_n + 1, c, inf);
		return;
	}
	if (n % step_pak_cc == 0)
	{
		read_cc = cena_zero_;
		ip_n.ok = false;
	}
	else
		ip_n.ok = true;
	size_t aa0 = data.adata;
	uchar dt2;
	data >> dt2;
	if (dt2 < 255)
		c.time = (int)read_cc.time + dt2;
	else
		data >> c.time;
	uchar a;
	data >> a;
	int delta = a & 7;
	int delta2 = a >> 3;
	if (delta2 == 31)
		data >> c.pok[0].c;
	else
		c.pok[0].c = read_cc.pok[0].c + delta2 - 15;
	if (delta == 7)
	{
		data >> a;
		delta = a;
	}
	c.pro[0].c = c.pok[0].c + delta + 1;
	size_t aa1 = data.adata;
	// декодирование продажи
	for (int j = c.pro[0].c, n2 = 0, n = 0; n2 < rceni;)
	{
		data >> a;
		int rez = a >> 6;
		int Vrez = (a & 63) + 1;
		for (int i = 1; i <= Vrez; i++)
		{
			int kk;
			if (rez == 0)
			{
				kk = 0;
			}
			else if (rez == 1)
			{
				char x;
				data >> x;
				kk = x;
			}
			else if (rez == 2)
			{
				short x;
				data >> x;
				kk = x;
			}
			else
			{
				data.pop_int24(kk);
			}
			while ((j > read_cc.pro[n].c) && (n < rceni - 1)) n++;//
			int kk3 = (j == read_cc.pro[n].c) ? read_cc.pro[n].k : 0;//
			int kk2 = kk + kk3;//
			if (kk2 > 0)
			{
				if (n2 >= rceni)
				{
					throw 1;
				}
				c.pro[n2].c = j;
				c.pro[n2].k = kk2;
				n2++;
			}
			j++;
		}
	}
	size_t aa2 = data.adata;
	// декодирование покупки
	for (int j = c.pok[0].c, n2 = 0, n = 0; n2 < rceni;)
	{
		data >> a;
		int rez = a >> 6;
		int Vrez = (a & 63) + 1;
		for (int i = 1; i <= Vrez; i++)
		{
			int kk;
			if (rez == 0)
			{
				kk = 0;
			}
			else if (rez == 1)
			{
				char x;
				data >> x;
				kk = x;
			}
			else if (rez == 2)
			{
				short x;
				data >> x;
				kk = x;
			}
			else
			{
				data.pop_int24(kk);
			}
			while ((j < read_cc.pok[n].c) && (n < rceni - 1)) n++;//
			int kk3 = (j == read_cc.pok[n].c) ? read_cc.pok[n].k : 0;//
			int kk2 = kk + kk3;//
			if (kk2 > 0)
			{
				if (n2 >= rceni)
				{
					throw 1;
				}
				c.pok[n2].c = j;
				c.pok[n2].k = kk2;
				n2++;
			}
			j--;
		}
	}
	read_cc = c;
	read_n = n;

	ip_n.r = int(data.adata - aa0);
	ip_n.r_pro = int(aa2 - aa1);
	ip_n.r_pok = int(data.adata - aa2);
	if (inf)*inf = ip_n;
}

void _super_stat::add(_prices& c)
{
	// настройка, чтобы наверняка не совпало 
	int delta = c.pro[0].c - c.pok[0].c - 1;
	if (delta > 255) return; // !!!!!!!!!!!!!!!!!!!!
	//  *TEMP.Add() = c;
	if (size % step_pak_cc == 0)
	{
		ip_last.ok = false;
		last_cc = cena_zero_;
		u_dd2.push_back((int)data.size);
	}
	else
		ip_last.ok = true;
	size++;
	// кодирование времени
	size_t aa0 = data.size;
	int dt = (int)c.time - (int)last_cc.time;
	if (dt < 0) dt = 0; // время может идти назад!
	if (dt >= 255)
	{
		uchar dt2 = 255;
		data << dt2;
		data << c.time;
	}
	else
	{
		uchar dt2 = dt;
		data << dt2;
	}
	// кодирование прослойки
	uchar a = (delta <= 6) ? delta : 7;
	int delta2 = c.pok[0].c - last_cc.pok[0].c;
	// 0..30   31 +2
	if ((delta2 >= -15) && (delta2 <= 15))
	{
		a += (delta2 + 15) << 3;
		data << a;
	}
	else
	{
		a += (31) << 3;
		data << a;
		data << c.pok[0].c;
	}
	if (delta > 6)
	{
		a = delta;
		data << a;
	}
	size_t aa1 = data.size;
	int deko[64]; // дельта кодируемая
	int reko[64]; // режим кодирования
	// кодирование продажи
	int rez = 0; // количество байт на дельту
	int Vrez = 0; // количество отсчетов
	for (int j = c.pro[0].c, n2 = 0, n = 0; j <= c.pro[rceni - 1].c; j++)
	{
		if (Vrez == 64)
		{
			otgruzka(rez, Vrez, deko);
			Vrez = 0;
			rez = 0;
		}
		if (j > c.pro[n2].c) n2++;
		while ((j > last_cc.pro[n].c) && (n < rceni - 1)) n++; //
		int kk2 = (j == c.pro[n2].c) ? c.pro[n2].k : 0;
		int kk3 = (j == last_cc.pro[n].c) ? last_cc.pro[n].k : 0; //
		int kk = kk2 - kk3; //
		deko[Vrez] = kk;
		if (kk == 0)
		{
			reko[Vrez] = 0;
			if (rez == 0)
			{
				Vrez++;
				continue;
			}
			if (rez == 1)
			{
				if (Vrez < 2)
				{
					Vrez++;
					continue;
				}
				if ((reko[Vrez - 1] == 1) || (reko[Vrez - 2] == 1))
				{
					Vrez++;
					continue;
				}
				otgruzka(rez, Vrez - 2, deko);
				reko[0] = reko[1] = reko[2] = 0;
				deko[0] = deko[1] = deko[2] = 0;
				Vrez = 3;
				rez = 0;
				continue;
			}
			if (rez == 2)
			{
				if (reko[Vrez - 1] == 2)
				{
					Vrez++;
					continue;
				}
				if (reko[Vrez - 1] == 0)
				{
					otgruzka(rez, Vrez - 1, deko);
					reko[0] = reko[1] = 0;
					deko[0] = deko[1] = 0;
					Vrez = 2;
					rez = 0;
					continue;
				}
				if (Vrez < 2)
				{
					Vrez++;
					continue;
				}
				if (reko[Vrez - 2] == 2)
				{
					Vrez++;
					continue;
				}
				otgruzka(rez, Vrez - 2, deko);
				reko[0] = reko[Vrez - 2];
				reko[1] = reko[Vrez - 1];
				reko[2] = reko[Vrez];
				deko[0] = deko[Vrez - 2];
				deko[1] = deko[Vrez - 1];
				deko[2] = deko[Vrez];
				Vrez = 3;
				rez = 1;
				continue;
			}
			if (rez == 3)
			{
				otgruzka(rez, Vrez, deko);
				reko[0] = 0;
				deko[0] = 0;
				Vrez = 1;
				rez = 0;
				continue;
			}
			continue;
		}
		if ((kk >= -128) && (kk <= 127))
		{
			reko[Vrez] = 1;
			if (rez == 0)
			{
				otgruzka(rez, Vrez, deko);
				reko[0] = reko[Vrez];
				deko[0] = deko[Vrez];
				Vrez = 1;
				rez = 1;
				continue;
			}
			if (rez == 1)
			{
				Vrez++;
				continue;
			}
			if (rez == 2)
			{
				if (Vrez < 2)
				{
					Vrez++;
					continue;
				}
				if ((reko[Vrez - 1] == 2) || (reko[Vrez - 2] == 2))
				{
					Vrez++;
					continue;
				}
				otgruzka(rez, Vrez - 2, deko);
				reko[0] = reko[Vrez - 2];
				reko[1] = reko[Vrez - 1];
				reko[2] = reko[Vrez];
				deko[0] = deko[Vrez - 2];
				deko[1] = deko[Vrez - 1];
				deko[2] = deko[Vrez];
				Vrez = 3;
				rez = 1;
				continue;
			}
			if (rez == 3)
			{
				if (reko[Vrez - 1] == 3)
				{
					Vrez++;
					continue;
				}
				if (reko[Vrez - 1] < 2)
				{
					otgruzka(rez, Vrez - 1, deko);
					reko[0] = reko[Vrez - 1];
					reko[1] = reko[Vrez];
					deko[0] = deko[Vrez - 1];
					deko[1] = deko[Vrez];
					Vrez = 2;
					rez = 1;
					continue;
				}
				if (Vrez < 2)
				{
					Vrez++;
					continue;
				}
				if (reko[Vrez - 2] == 3)
				{
					Vrez++;
					continue;
				}
				otgruzka(rez, Vrez - 2, deko);
				reko[0] = reko[Vrez - 2];
				reko[1] = reko[Vrez - 1];
				reko[2] = reko[Vrez];
				deko[0] = deko[Vrez - 2];
				deko[1] = deko[Vrez - 1];
				deko[2] = deko[Vrez];
				Vrez = 3;
				rez = 2;
				continue;
			}
			continue;
		}
		if ((kk >= -32768) && (kk <= 32767))
		{
			reko[Vrez] = 2;
			if (rez < 2)
			{
				otgruzka(rez, Vrez, deko);
				reko[0] = reko[Vrez];
				deko[0] = deko[Vrez];
				Vrez = 1;
				rez = 2;
				continue;
			}
			if (rez == 2)
			{
				Vrez++;
				continue;
			}
			if (rez == 3)
			{
				if (Vrez < 2)
				{
					Vrez++;
					continue;
				}
				if ((reko[Vrez - 1] == 3) || (reko[Vrez - 2] == 3))
				{
					Vrez++;
					continue;
				}
				otgruzka(rez, Vrez - 2, deko);
				reko[0] = reko[Vrez - 2];
				reko[1] = reko[Vrez - 1];
				reko[2] = reko[Vrez];
				deko[0] = deko[Vrez - 2];
				deko[1] = deko[Vrez - 1];
				deko[2] = deko[Vrez];
				Vrez = 3;
				rez = 2;
				continue;
			}
			continue;
		}
		if ((kk < -32768) || (kk > 32767))
		{
			reko[Vrez] = 3;
			if (rez < 3)
			{
				otgruzka(rez, Vrez, deko);
				reko[0] = reko[Vrez];
				deko[0] = deko[Vrez];
				Vrez = 1;
				rez = 3;
				continue;
			}
			if (rez == 3)
			{
				Vrez++;
				continue;
			}
			continue;
		}
	}
	otgruzka(rez, Vrez, deko);
	// кодирование покупки
	size_t aa2 = data.size;
	rez = 0; // количество байт на дельту
	Vrez = 0; // количество отсчетов
	for (int j = c.pok[0].c, n2 = 0, n = 0; j >= c.pok[rceni - 1].c; j--)
	{
		if (Vrez == 64)
		{
			otgruzka(rez, Vrez, deko);
			Vrez = 0;
			rez = 0;
		}
		if (j < c.pok[n2].c) n2++;
		while ((j < last_cc.pok[n].c) && (n < rceni - 1)) n++; //
		int kk2 = (j == c.pok[n2].c) ? c.pok[n2].k : 0;
		int kk3 = (j == last_cc.pok[n].c) ? last_cc.pok[n].k : 0; //
		int kk = kk2 - kk3; //
		deko[Vrez] = kk;
		if (kk == 0)
		{
			reko[Vrez] = 0;
			if (rez == 0)
			{
				Vrez++;
				continue;
			}
			if (rez == 1)
			{
				if (Vrez < 2)
				{
					Vrez++;
					continue;
				}
				if ((reko[Vrez - 1] == 1) || (reko[Vrez - 2] == 1))
				{
					Vrez++;
					continue;
				}
				otgruzka(rez, Vrez - 2, deko);
				reko[0] = reko[1] = reko[2] = 0;
				deko[0] = deko[1] = deko[2] = 0;
				Vrez = 3;
				rez = 0;
				continue;
			}
			if (rez == 2) {
				if (reko[Vrez - 1] == 2)
				{
					Vrez++;
					continue;
				}
				if (reko[Vrez - 1] == 0)
				{
					otgruzka(rez, Vrez - 1, deko);
					reko[0] = reko[1] = 0;
					deko[0] = deko[1] = 0;
					Vrez = 2;
					rez = 0;
					continue;
				}
				if (Vrez < 2)
				{
					Vrez++;
					continue;
				}
				if (reko[Vrez - 2] == 2)
				{
					Vrez++;
					continue;
				}
				otgruzka(rez, Vrez - 2, deko);
				reko[0] = reko[Vrez - 2];
				reko[1] = reko[Vrez - 1];
				reko[2] = reko[Vrez];
				deko[0] = deko[Vrez - 2];
				deko[1] = deko[Vrez - 1];
				deko[2] = deko[Vrez];
				Vrez = 3;
				rez = 1;
				continue;
			}
			if (rez == 3)
			{
				otgruzka(rez, Vrez, deko);
				reko[0] = 0;
				deko[0] = 0;
				Vrez = 1;
				rez = 0;
				continue;
			}
			continue;
		}
		if ((kk >= -128) && (kk <= 127))
		{
			reko[Vrez] = 1;
			if (rez == 0)
			{
				otgruzka(rez, Vrez, deko);
				reko[0] = reko[Vrez];
				deko[0] = deko[Vrez];
				Vrez = 1;
				rez = 1;
				continue;
			}
			if (rez == 1)
			{
				Vrez++;
				continue;
			}
			if (rez == 2)
			{
				if (Vrez < 2)
				{
					Vrez++;
					continue;
				}
				if ((reko[Vrez - 1] == 2) || (reko[Vrez - 2] == 2))
				{
					Vrez++;
					continue;
				}
				otgruzka(rez, Vrez - 2, deko);
				reko[0] = reko[Vrez - 2];
				reko[1] = reko[Vrez - 1];
				reko[2] = reko[Vrez];
				deko[0] = deko[Vrez - 2];
				deko[1] = deko[Vrez - 1];
				deko[2] = deko[Vrez];
				Vrez = 3;
				rez = 1;
				continue;
			}
			if (rez == 3)
			{
				if (reko[Vrez - 1] == 3)
				{
					Vrez++;
					continue;
				}
				if (reko[Vrez - 1] < 2)
				{
					otgruzka(rez, Vrez - 1, deko);
					reko[0] = reko[Vrez - 1];
					reko[1] = reko[Vrez];
					deko[0] = deko[Vrez - 1];
					deko[1] = deko[Vrez];
					Vrez = 2;
					rez = 1;
					continue;
				}
				if (Vrez < 2)
				{
					Vrez++;
					continue;
				}
				if (reko[Vrez - 2] == 3)
				{
					Vrez++;
					continue;
				}
				otgruzka(rez, Vrez - 2, deko);
				reko[0] = reko[Vrez - 2];
				reko[1] = reko[Vrez - 1];
				reko[2] = reko[Vrez];
				deko[0] = deko[Vrez - 2];
				deko[1] = deko[Vrez - 1];
				deko[2] = deko[Vrez];
				Vrez = 3;
				rez = 2;
				continue;
			}
			continue;
		}
		if ((kk >= -32768) && (kk <= 32767))
		{
			reko[Vrez] = 2;
			if (rez < 2)
			{
				otgruzka(rez, Vrez, deko);
				reko[0] = reko[Vrez];
				deko[0] = deko[Vrez];
				Vrez = 1;
				rez = 2;
				continue;
			}
			if (rez == 2)
			{
				Vrez++;
				continue;
			}
			if (rez == 3)
			{
				if (Vrez < 2)
				{
					Vrez++;
					continue;
				}
				if ((reko[Vrez - 1] == 3) || (reko[Vrez - 2] == 3))
				{
					Vrez++;
					continue;
				}
				otgruzka(rez, Vrez - 2, deko);
				reko[0] = reko[Vrez - 2];
				reko[1] = reko[Vrez - 1];
				reko[2] = reko[Vrez];
				deko[0] = deko[Vrez - 2];
				deko[1] = deko[Vrez - 1];
				deko[2] = deko[Vrez];
				Vrez = 3;
				rez = 2;
				continue;
			}
			continue;
		}
		if ((kk < -32768) || (kk > 32767))
		{
			reko[Vrez] = 3;
			if (rez < 3)
			{
				otgruzka(rez, Vrez, deko);
				reko[0] = reko[Vrez];
				deko[0] = deko[Vrez];
				Vrez = 1;
				rez = 3;
				continue;
			}
			if (rez == 3)
			{
				Vrez++;
				continue;
			}
			continue;
		}
	}
	otgruzka(rez, Vrez, deko);
	last_cc = c;
	ip_last.r = int(data.size - aa0);
	ip_last.r_pro = int(aa2 - aa1);
	ip_last.r_pok = int(data.size - aa2);
}

_super_stat::_super_stat()
{
	last_cc = cena_zero_;
	read_cc = cena_zero_;
	size = 0;
	read_n = -666;
	ip_last.ok = false;
	ip_n.ok = false;
	load_from_file(L"baza.cen");
}

