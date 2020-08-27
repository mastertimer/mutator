#include "set.h"

_super_stat ss; // сжатые цены

constexpr _prices cena_zero_ = { {}, {}, { 1,1,1,1,1 } };

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void _date_time::now()
{
	SYSTEMTIME t;
	GetLocalTime(&t);
	month = t.wMonth + (t.wYear - 2017) * 12;
	day = (uchar)t.wDay;
	hour = (uchar)t.wHour;
	minute = (uchar)t.wMinute;
	second = (uchar)t.wSecond;
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
	mem << (int)size;
	mem.push_data(&last_cc, sizeof(last_cc));
	mem << u_dd2;
	mem.save_to_file(fn);
}

void _super_stat::load_from_file(wstr fn)
{
	_stack mem;
	if (!mem.load_from_file(fn)) return;
	mem >> data;
	int sisi;
	mem >> sisi; // !!! пересохранить на 64
	size = sisi;
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

void _super_stat::read(int64 n, _prices& c, _info_pak* inf)
{
	if (inf) inf->ok = false;
	if ((n < 0) || (n >= size)) return;
	if (n == read_n)
	{
		c = read_cc;
		if (inf)* inf = ip_n;
		return;
	}
	if (n == size - 1)
	{
		if (inf)* inf = ip_last;
		c = last_cc;
		return;
	}
	if (read_n + 1 != n)
	{
		int64 k = n / step_pak_cc;
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
	if (inf)* inf = ip_n;
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

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void _g_graph::ris2(_trans tr, bool final)
{
	_area2i a = tr(local_area);
	draw();
	master_bm.draw(a.x.min, a.y.min, bm.size.x, bm.size.y, &bm);
}

// вынести в общий модуль?
void OSpordis(double min, double max, int64 maxN, double& mi, double& step)
{
	int64 n;
	double step2;
	if (maxN < 2) maxN = 2;
	step = exp(round(log((max - min) / maxN) / log(10)) * log(10));
	do
	{
		{
			mi = (int64(min / step)) * step;
			if (mi < min) mi += step;
			n = (int64((max - mi) / step)) + 1;
		}
		if (n < maxN) step = step * 0.1; else break;
	} while (true);
	while (n > maxN)
	{
		step = step * 10;
		{
			mi = (int64(min / step)) * step;
			if (mi < min) mi += step;
			n = (int64((max - mi) / step)) + 1;
		}
	}
	step2 = step;
	step = step2 * 0.2;
	{
		mi = (int64(min / step)) * step;
		if (mi < min) mi += step;
		n = (int64((max - mi) / step)) + 1;
	}
	if (n <= maxN) return;
	step = step2 * 0.5;
	{
		mi = (int64(min / step)) * step;
		if (mi < min) mi += step;
		n = (int64((max - mi) / step)) + 1;
	}
	if (n <= maxN) return;
	step = step2;
	{
		mi = (int64(min / step)) * step;
		if (mi < min) mi += step;
		n = (int64((max - mi) / step)) + 1;
	}
}

std::string date_to_ansi_string(int time)
{
	std::string res = "22.12.20";
	_date_time a;
	a = time;
	res[0] = (a.day / 10) + '0';
	res[1] = (a.day % 10) + '0';
	int m = ((a.month - 1) % 12) + 1;
	res[3] = (m / 10) + '0';
	res[4] = (m % 10) + '0';
	int g = ((a.month - 1) / 12) + 17;
	res[6] = (g / 10) + '0';
	res[7] = (g % 10) + '0';
	return res;
}

void _g_graph::draw()
{
	if (!obn) return;
	obn = false;
	bm.clear();

	_area y_; // диапазон у (grid)
	static std::vector<int> time_; // отсчеты времени (grid)

	double polzi_ = 0; // !! ползунок

	_g_scrollbar* sb = find1<_g_scrollbar>(flag_part);
	if (sb)	polzi_ = sb->position;

	int ll = (int)curve.size();
	if (ll == 0) return;

	int64 k_el = bm.size.x / size_el;
	if (k_el < 1) return;
	double r_el = (double)bm.size.x / k_el;

	int n = curve[0]->get_n();
	if (n == 0) return;
	//	v_vib_ = n - k_el;
	v_vib = n - 1;
	if (v_vib < 0) v_vib = 0;
	int vib = (int)(polzi_ * v_vib + 0.5); // !! ползунок

	int period = 60;
	int pause_max = 3;
	_element_chart* al = new _element_chart[ll]; // элементы линий
	// 1-й проход - вычисление zmin, zmax
	double zmin = 1E100;
	double zmax = -1E100;
	curve[0]->get_n_info(vib, &al[0]);
	int timelast = al[0].time;
	for (int i = 1; i < ll; i++) curve[i]->get_t_info(timelast, &al[i]);
	timelast -= period;
	int ke = 0; // количество построенных элементов
	while (ke < k_el)
	{
		int timenext = 2000000000; // следующее время
		for (int i = 0; i < ll; i++)
			if (al[i].n >= 0)
				if (al[i].time < timenext) timenext = al[i].time;
		if (timenext == 2000000000) break;

		//int dt = (timenext - timelast) / period;
		//ke += (dt <= (pause_max + 1)) ? dt : 2;
		//if (ke > k_el) break;
		ke++;
		timelast = timenext;

		for (int i = 0; i < ll; i++)
		{
			if (al[i].n < 0) continue;
			if (al[i].time == timelast)
			{ // сработало
				if (al[i].min < zmin) zmin = al[i].min;
				if (al[i].max > zmax) zmax = al[i].max;
				curve[i]->get_n_info(al[i].n + 1, &al[i]);
			}
		}
	}
	if (zmin == zmax) zmax = zmin + 1.0;
	y_ = { zmin, zmax };
	time_.clear();
	// 2-й проход - рисование
	curve[0]->get_n_info(vib, &al[0]);
	timelast = al[0].time;
	for (int i = 1; i < ll; i++) curve[i]->get_t_info(timelast, &al[i]);
	timelast -= period;
	ke = 0; // количество построенных элементов
	while (ke < k_el)
	{
		int timenext = 2000000000; // следующее время
		for (int i = 0; i < ll; i++)
			if (al[i].n >= 0)
				if (al[i].time < timenext) timenext = al[i].time;
		if (timenext == 2000000000) break;

		//int dt = (timenext - timelast) / period;
		//ke += (dt <= (pause_max + 1)) ? dt : 2;
		//if (ke > k_el) break;
		ke++;
		timelast = timenext;
		time_.push_back(timelast);

		for (int i = 0; i < ll; i++)
		{
			if (al[i].n < 0) continue;
			if (al[i].time == timelast)
			{ // сработало
				double ymi = bm.size.y - (al[i].min - zmin) * bm.size.y / (zmax - zmin);
				double yma = bm.size.y - (al[i].max - zmin) * bm.size.y / (zmax - zmin);
				double x = r_el * (ke - 1i64);
				curve[i]->draw(al[i].n, { {x, x + r_el}, {yma, ymi} }, &bm);
				curve[i]->get_n_info(al[i].n + 1, &al[i]);
			}
		}
	}
	delete[] al;

	// рисование сетки

	uint col_setka = c_max - 0xE0000000; // цвет сетки
	uint col_setka_font = c_def; // цвет подписи сетки
	if (time_.size() < 1)
	{
		bm.line({ 0, 0 }, { bm.size.x - 1, bm.size.y - 1 }, 0xFF800000);
		bm.line({ 0, bm.size.y - 1 }, { bm.size.x - 1 , 0 }, 0xFF800000);
		return;
	}
	// рисование горизонтальных линий сетки с подписями
	int64 dex = 35; // длина подписи
	int64 maxN = bm.size.y / 15;
	if (maxN > 1)
	{
		double mi, step;
		OSpordis(y_.min, y_.max, maxN, mi, step);
		for (double y = mi; y < y_.max; y += step)
		{
			double yy = bm.size.y - 0.5 - (y - y_.min) * bm.size.y / (y_.max - y_.min);
			bm.line({ dex, (int)(yy) }, { bm.size.x - dex, (int)(yy) }, col_setka);
			bm.text16(0, (int)(yy - 7), double_to_astring(y, 2).c_str(), col_setka_font);
			bm.text16(bm.size.x - dex, (int)(yy - 7), double_to_astring(y, 2).data(), col_setka_font);
		}
	}
	// рисование вертикальных линий сетки с подписями
	static int g_delta_time[] = {
		1, 2, 3, 5, 10, 15, 20, 30, // секунды
		60, 120, 180, 300, 600, 900, 1200, 1800, // минуты
		3600, 7200, 10800, 14400, 21600, 28800, 43200, // часы
		86400, 172800, 345600, 691200, 1382400, // дни
		2764800, 5529600, 8294400, 11059200, 16588800, // месяца
		33177600 }; // год

	double rel = r_el;
	int stept = (((int)(dex / rel)) + 1) * period;
	int ks = sizeof(g_delta_time) / sizeof(g_delta_time[0]);
	for (int i = 0; i < ks; i++)
		if (g_delta_time[i] >= stept)
		{
			stept = g_delta_time[i];
			break;
		}
	int dele[] = { 1, 60, 3600, 86400, 2764800, 33177600 };
	int ost[] = { 60, 60, 24, 32, 12, 1000 };
	int ido = 0;
	if (stept % 33177600) ido = 4;
	if (stept % 2764800) ido = 3;
	if (stept % 86400) ido = 2;
	if (stept % 3600) ido = 1;
	if (stept % 60) ido = 0;
	std::string s = "00:00";
	int mintime = 0;
	int pr_time = 0;
	for (uint i = 0; i < time_.size(); i++)
	{
		if (time_[i] == 0) continue;
		if (mintime == 0) mintime = time_[i];
		bool sca = ((pr_time > 0) && (time_[i] - pr_time > 36000));
		pr_time = time_[i];
		if (time_[i] % stept == 0)
		{ // вертикальная линия с подписью
			double x = rel * i;
			if ((x <= dex) || (x >= (int64)bm.size.x - dex)) continue;
			uint cl = (sca) ? (0x80FF0000) : col_setka;
			bm.line({ (int)x, 0 }, { (int)x, bm.size.y - 1 }, cl);
			if ((x - 13 <= dex) || (x + 13 >= (int64)bm.size.x - dex)) continue;
			int ii = (time_[i] / dele[ido]) % ost[ido];
			s[4] = '0' + (ii % 10);
			s[3] = '0' + (ii / 10);
			ii = (time_[i] / dele[ido + 1]) % ost[ido + 1];
			s[1] = '0' + (ii % 10);
			s[0] = '0' + (ii / 10);
			bm.text16((int)(x - 13), bm.size.y - 11, s.data(), col_setka_font);
			bm.text16((int)(x - 13), -2, s.data(), col_setka_font);
			continue;
		}
		if (sca)
		{
			double x = rel * i;
			if ((x <= dex) || (x >= (int64)bm.size.x - dex)) continue;
			bm.line({ (int)x, 0 }, { (int)x, bm.size.y - 1 }, 0x80FF0000);
		}
	}
	// рисование даты
	bm.text16n(dex + 10, 10, date_to_ansi_string(mintime).data(), 4, c_max - 0x80000000);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int _mctds_candle::get_n()
{
	return (int)cen1m.size();
}

void _mctds_candle::get_n_info(int n, _element_chart* e)
{
	if (n >= (int)cen1m.size())
	{
		e->n = -1;
		return;
	}
	e->n = n;
	e->time = cen1m[n].time;
	e->min = cen1m[n].min * c_unpak;
	e->max = cen1m[n].max * c_unpak;
	e->middle = cen1m[n].cc * c_unpak;
}

void _mctds_candle::get_t_info(int t, _element_chart* e)
{
	e->n = -1; // !! написать, когда потребуется!!
}

void _mctds_candle::push(_stack* mem)
{
	*mem << cen1m;
}

void _mctds_candle::pop(_stack* mem)
{
	*mem >> cen1m;
}

void _mctds_candle::draw(int n, _area2 area, _bitmap* bm)
{
	double min_ = cen1m[n].min * c_unpak;
	double max_ = cen1m[n].max * c_unpak;
	double first_ = cen1m[n].first * c_unpak;
	double last_ = cen1m[n].last * c_unpak;

	_area2 oo = area;
	int x1 = (int)(oo.x.min + 1);
	int x2 = (int)(oo.x.max - 1);
	if (x2 < x1) return;

	uint col_rost = 0xFF28A050; // цвет ростущей свечки
	uint col_pade = 0xFF186030; // цвет падающей свечки
	double yfi, yla;
	if (min_ < max_)
	{
		yfi = oo.y.max - oo.y.length() * (first_ - min_) / (max_ - min_);
		yla = oo.y.max - oo.y.length() * (last_ - min_) / (max_ - min_);
	}
	else
	{
		yfi = yla = oo.y.min;
	}
	if (first_ <= last_)
	{
		bm->fill_rectangle({ {x1, x2}, {(int)yla, (int)yfi} }, col_rost);
		bm->line({ (x1 + x2) >> 1, (int)oo.y.max }, { (x1 + x2) >> 1, (int)oo.y.min }, col_rost);
	}
	else
	{
		bm->fill_rectangle({ {x1, x2}, {(int)yfi, (int)yla} }, col_pade);
		bm->line({ (x1 + x2) >> 1, (int)oo.y.max }, { (x1 + x2) >> 1, (int)oo.y.min }, col_pade);
	}

}

void _mctds_candle::recovery()
{
	int64 vcc = 0;
	if (cen1m.size()) vcc = cen1m.back().ncc.max;
	int ssvcc = (int)ss->size;
	if (ssvcc == vcc) return; // ничего не изменилось
	if (vcc < ssvcc) // добавились несколько цен
	{
		_prices cc;
		int t = 0;
		_cen_pak* cp = 0;
		if (cen1m.size())
		{
			cp = &cen1m.back();
			cp->cc *= ((double)cp->ncc.max - cp->ncc.min);
			t = cp->time;
		}
		for (int64 i = vcc; i < ssvcc; i++)
		{
			if (i > 572737) // i > 572738
			{
				if (cp == 0) continue;
			}
			ss->read(i, cc);
			int t2 = cc.time.to_minute();
			if (t2 != t)
			{
				t = t2;
				if (cp)	cp->cc /= ((double)cp->ncc.max - cp->ncc.min);
				_cen_pak we;
				we.time = t;
				we.ncc.min = i;
				we.ncc.max = i + 1;
				we.first = ((int)cc.pok[0].c + (int)cc.pro[0].c) / 2;
				we.last = we.first;
				we.min = we.first;
				we.max = we.first;
				we.cc = we.first;
				cen1m.push_back(we);
				cp = &cen1m.back();
			}
			else
			{
				if (cp == 0) continue; // для паранойи компилятора
				int aa = ((int)cc.pok[0].c + (int)cc.pro[0].c) / 2;
				cp->cc += aa;
				if (aa < cp->min) cp->min = aa;
				if (aa > cp->max) cp->max = aa;
				cp->ncc.max++;
				cp->last = aa;
			}
		}
		if (cp)	cp->cc /= ((double)cp->ncc.max - cp->ncc.min);
		return;
	}
	_prices cc; // уменьшились цены, полный пересчет
	cen1m.clear();
	int t = 0;
	_cen_pak* cp = 0;
	for (int64 i = 0; i < ssvcc; i++)
	{
		ss->read(i, cc);
		int t2 = cc.time.to_minute();
		if (t2 != t)
		{
			t = t2;
			if (cp)	cp->cc /= ((double)cp->ncc.max - cp->ncc.min);
			_cen_pak we;
			we.time = t;
			we.ncc.min = i;
			we.ncc.max = i + 1;
			we.first = ((int)cc.pok[0].c + (int)cc.pro[0].c) / 2;
			we.last = we.first;
			we.min = we.first;
			we.max = we.first;
			we.cc = we.first;
			cen1m.push_back(we);
			cp = &cen1m.back();
		}
		else
		{
			if (cp == 0) continue; // для паранойи компилятора
			int aa = ((int)cc.pok[0].c + (int)cc.pro[0].c) / 2;
			cp->cc += aa;
			if (aa < cp->min) cp->min = aa;
			if (aa > cp->max) cp->max = aa;
			cp->ncc.max++;
			cp->last = aa;
		}
	}
	if (cp)	cp->cc /= ((double)cp->ncc.max - cp->ncc.min);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int _latest_events::start()
{
	if ((minute[2] == 2) && (event[0] == event[1]) && (event[0] == event[2])) // триплет
	{
		if (event[0] == 1) return 70;
		if (event[0] == 2) // фиолетовый
			if ((x[0] > x[1]) && (x[1] > x[2]))	return 13;
		if (event[0] == 3)
		{
			if ((x[0] > x[1]) && (x[1] > x[2]))	return 40;
			if ((x[0] < x[1]) && (x[1] < x[2]))	return 90;
		}
		//		if (event_[0] == 4) return 120;  //голубой
		if (event[0] == 6) return 60;   //зеленый
		return 0;
	}
	if ((minute[1] == 1) && (event[0] == event[1])) // дуплет
	{
		if (event[0] == 2) // фиолетовый
		{
			if (event[2] == 5) return 100; //песочный
			if ((event[2] == 4) && (event[3] == 4)) return 40;
		}
		return 0;
	}
	return 0;
}

bool _latest_events::stop()
{
	if ((event[0] == 5) && (event[1] == 5) && (minute[1] == 1)) return true; // песочный
	return false;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void _nervous_oracle::push(_stack* mem)
{
	*mem << zn;
}

void _nervous_oracle::pop(_stack* mem)
{
	*mem >> zn;
}

int _nervous_oracle::get_n()
{
	return (int)zn.size();
}

void _nervous_oracle::get_n_info(int n, _element_chart* e)
{
	if (n >= (int)zn.size())
	{
		e->n = -1;
		return;
	}
	e->n = n;
	e->time = zn[n].time;
	e->middle = ((double)zn[n].max_pro + (double)zn[n].min_pok) * 0.5 * c_unpak;
	e->min = e->middle;
	e->max = e->middle;
}

void _nervous_oracle::get_t_info(int t, _element_chart* e)
{
	auto x = lower_bound(zn.begin(), zn.end(), t);
	if (x == zn.end())
	{
		e->n = -1;
		return;
	}
	int xx = (int)(x - zn.begin());
	e->n = xx;
	e->time = zn[xx].time;
	e->middle = ((double)zn[xx].max_pro + (double)zn[xx].min_pok) * 0.5 * c_unpak;
	e->min = e->middle;
	e->max = e->middle;
}

_latest_events _nervous_oracle::get_latest_events(int64 nn)
{
	const int64 k = 4;
	_latest_events e;
	e.event[0] = e.event[1] = e.event[2] = e.event[3] = 0;
	e.minute[0] = e.minute[1] = e.minute[2] = e.minute[3] = 0;
	e.x[0] = e.x[1] = e.x[2] = e.x[3] = 0.0;
	if (nn < 10) return e;
	int64 ii = std::max(k, nn - 40);
	int ee = 0;
	for (int64 n = nn; n >= ii; n--)
	{
		if ((int64)zn[n].time - zn[n - k].time != k * 60) continue;
		bool rost_pro = true;
		bool rost_pok = true;
		bool pade_pro = true;
		bool pade_pok = true;
		for (int64 i = n - k; i < n; i++)
		{
			if (zn[i].r_pro >= zn[i + 1].r_pro) rost_pro = false;
			if (zn[i].r_pok >= zn[i + 1].r_pok) rost_pok = false;
			if (zn[i].r_pro <= zn[i + 1].r_pro) pade_pro = false;
			if (zn[i].r_pok <= zn[i + 1].r_pok) pade_pok = false;
		}
		char a = 0;
		if ((rost_pro || rost_pok) && (pade_pro || pade_pok))
			a = 7;
		else
		{
			if (rost_pok) a += 1;
			if (rost_pro) a += 2;
			if (pade_pro || pade_pok) a += 3;
			if (pade_pok) a += 1;
			if (pade_pro) a += 2;
		}
		if (a == 0) continue;
		e.event[ee] = a;
		e.minute[ee] = (int)(nn - n);
		e.x[ee] = ((double)zn[n].max_pro + (double)zn[n].min_pok) * 0.5 * c_unpak;
		ee++;
		if (ee == 4) break;
	}
	return e;
}

void _nervous_oracle::draw(int n, _area2 area, _bitmap* bm)
{
	const int64 k = 4;
	if (n < k) return;
	if ((int64)zn[n].time - zn[n - k].time != k * 60) return;
	bool rost_pro = true;
	bool rost_pok = true;
	bool pade_pro = true;
	bool pade_pok = true;
	for (int64 i = n - k; i < n; i++)
	{
		if (zn[i].r_pro >= zn[i + 1].r_pro) rost_pro = false;
		if (zn[i].r_pok >= zn[i + 1].r_pok) rost_pok = false;
		if (zn[i].r_pro <= zn[i + 1].r_pro) pade_pro = false;
		if (zn[i].r_pok <= zn[i + 1].r_pok) pade_pok = false;
	}
	_element_nervous& a = zn[n];
	uint c = 0xFF808080;

	if (rost_pro) c += 0x70;
	if (rost_pok) c += 0x700000;
	if (pade_pro) c -= 0x70;
	if (pade_pok) c -= 0x700000;

	double r = area.x.length() * 0.5 * 2;
	if (get_latest_events(n).start())
	{
		c = 0xFFFF0000;
		r *= 2;
	}

	if (c == 0xFF808080) return;

	bm->fill_ring(area.center(), r, r * 0.1, c, c);
}

void _nervous_oracle::recovery()
{
	int64 vcc = 0;
	if (zn.size()) vcc = zn.back().ncc.max;
	int64 ssvcc = ss->size;
	if (ssvcc == vcc) return; // ничего не изменилось
	if (vcc < ssvcc) // добавились несколько цен
	{
		_prices cc;
		_super_stat::_info_pak inf;
		int t = 0;
		_element_nervous* cp = 0;
		if (zn.size())
		{
			cp = &zn.back();
			if (cp->v_r)
			{
				cp->r *= cp->v_r;
				cp->r_pok *= cp->v_r;
				cp->r_pro *= cp->v_r;
			}
			t = cp->time;
		}
		for (int64 i = vcc; i < ssvcc; i++)
		{
			ss->read(i, cc, &inf);
			int t2 = cc.time.to_minute();
			if (t2 != t)
			{
				t = t2;
				if (cp)
				{
					if (cp->v_r)
					{
						cp->r /= cp->v_r;
						cp->r_pok /= cp->v_r;
						cp->r_pro /= cp->v_r;
					}
				}
				_element_nervous we;
				we.time = t;
				we.ncc.min = i;
				we.ncc.max = i + 1;
				we.max_pok = we.min_pok = cc.pok[0].c;
				we.max_pro = we.min_pro = cc.pro[0].c;
				if (inf.ok)
				{
					we.v_r = 1;
					we.r = inf.r;
					we.r_pok = inf.r_pok;
					we.r_pro = inf.r_pro;
				}
				else
					we.v_r = 0;
				zn.push_back(we);
				cp = &zn.back();
			}
			else
			{
				if (cp == 0) continue; // для паранойи компилятора
				if (cc.pok[0].c < cp->min_pok) cp->min_pok = cc.pok[0].c;
				if (cc.pok[0].c > cp->max_pok) cp->max_pok = cc.pok[0].c;
				if (cc.pro[0].c < cp->min_pro) cp->min_pro = cc.pro[0].c;
				if (cc.pro[0].c > cp->max_pro) cp->max_pro = cc.pro[0].c;
				cp->ncc.max++;
				if (inf.ok)
				{
					if (cp->v_r)
					{
						cp->v_r++;
						cp->r += inf.r;
						cp->r_pok += inf.r_pok;
						cp->r_pro += inf.r_pro;
					}
					else
					{
						cp->v_r = 1;
						cp->r = inf.r;
						cp->r_pok = inf.r_pok;
						cp->r_pro = inf.r_pro;
					}
				}
			}
		}
		if (cp)
		{
			if (cp->v_r)
			{
				cp->r /= cp->v_r;
				cp->r_pok /= cp->v_r;
				cp->r_pro /= cp->v_r;
			}
		}
		return;
	}
	_prices cc; // уменьшились цены, полный пересчет
	_super_stat::_info_pak inf;
	zn.clear();
	int t = 0;
	_element_nervous* cp = 0;
	for (int64 i = 0; i < ssvcc; i++)
	{
		ss->read(i, cc, &inf);
		int t2 = cc.time.to_minute();
		if (t2 != t)
		{
			t = t2;
			if (cp)
			{
				if (cp->v_r)
				{
					cp->r /= cp->v_r;
					cp->r_pok /= cp->v_r;
					cp->r_pro /= cp->v_r;
				}
			}
			_element_nervous we;
			we.time = t;
			we.ncc.min = i;
			we.ncc.max = i + 1;
			we.max_pok = we.min_pok = cc.pok[0].c;
			we.max_pro = we.min_pro = cc.pro[0].c;
			if (inf.ok)
			{
				we.v_r = 1;
				we.r = inf.r;
				we.r_pok = inf.r_pok;
				we.r_pro = inf.r_pro;
			}
			else
				we.v_r = 0;
			zn.push_back(we);
			cp = &zn.back();
		}
		else
		{
			if (cp == 0) continue; // для паранойи компилятора
			if (cc.pok[0].c < cp->min_pok) cp->min_pok = cc.pok[0].c;
			if (cc.pok[0].c > cp->max_pok) cp->max_pok = cc.pok[0].c;
			if (cc.pro[0].c < cp->min_pro) cp->min_pro = cc.pro[0].c;
			if (cc.pro[0].c > cp->max_pro) cp->max_pro = cc.pro[0].c;
			cp->ncc.max++;
			if (inf.ok)
			{
				if (cp->v_r)
				{
					cp->v_r++;
					cp->r += inf.r;
					cp->r_pok += inf.r_pok;
					cp->r_pro += inf.r_pro;
				}
				else
				{
					cp->v_r = 1;
					cp->r = inf.r;
					cp->r_pok = inf.r_pok;
					cp->r_pro = inf.r_pro;
				}
			}
		}
	}
	if (cp)
	{
		if (cp->v_r)
		{
			cp->r /= cp->v_r;
			cp->r_pok /= cp->v_r;
			cp->r_pro /= cp->v_r;
		}
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void _oracle3::get_n_info(int n, _element_chart* e)
{
	if (n >= (int)zn.size())
	{
		e->n = -1;
		return;
	}
	e->n = n;
	e->time = zn[n].time;
	e->min = (zn[n].min - 1) * c_unpak;
	e->max = zn[n].max * c_unpak;
	e->middle = (e->min + e->max) * 0.5;
}

void _oracle3::get_t_info(int t, _element_chart* e)
{
	auto x = lower_bound(zn.begin(), zn.end(), t);
	if (x == zn.end())
	{
		e->n = -1;
		return;
	}
	int xx = (int)(x - zn.begin());
	e->n = xx;
	e->time = zn[xx].time;
	e->min = (zn[xx].min - 1) * c_unpak;
	e->max = zn[xx].max * c_unpak;
	e->middle = (e->min + e->max) * 0.5;
}

int _oracle3::get_n()
{
	return (int)zn.size();
}

void _oracle3::recovery()
{
	int64 vcc = 0;
	if (zn.size()) vcc = zn.back().ncc.max;
	int64 ssvcc = ss->size;
	if (ssvcc == vcc) return; // ничего не изменилось
	if (vcc < ssvcc) // добавились несколько цен
	{
		_prices cc;
		int t = 0;
		_element_oracle* cp = 0;
		if (zn.size())
		{
			cp = &zn.back();
			t = cp->time;
		}
		for (int64 i = vcc; i < ssvcc; i++)
		{
			ss->read(i, cc);
			int t2 = cc.time.to_minute();
			if (t2 != t)
			{
				t = t2;
				_element_oracle we;
				we.time = t;
				we.ncc.min = i;
				we.ncc.max = i + 1;
				we.max = cc.pro[rceni - 1].c;
				we.min = cc.pok[rceni - 1].c;
				zn.push_back(we);
				cp = &zn.back();
			}
			else
			{
				if (cp == 0) continue; // для паранойи компилятора
				if (cc.pok[rceni - 1].c < cp->min) cp->min = cc.pok[rceni - 1].c;
				if (cc.pro[rceni - 1].c > cp->max) cp->max = cc.pro[rceni - 1].c;
				cp->ncc.max++;
			}
		}
		return;
	}
	_prices cc; // уменьшились цены, полный пересчет
	zn.clear();
	int t = 0;
	_element_oracle* cp = 0;
	for (int64 i = 0; i < ssvcc; i++)
	{
		ss->read(i, cc);
		int t2 = cc.time.to_minute();
		if (t2 != t)
		{
			t = t2;
			_element_oracle we;
			we.time = t;
			we.ncc.min = i;
			we.ncc.max = i + 1;
			we.max = cc.pro[rceni - 1].c;
			we.min = cc.pok[rceni - 1].c;
			zn.push_back(we);
			cp = &zn.back();
		}
		else
		{
			if (cp == 0) continue; // для паранойи компилятора
			if (cc.pok[rceni - 1].c < cp->min) cp->min = cc.pok[rceni - 1].c;
			if (cc.pro[rceni - 1].c > cp->max) cp->max = cc.pro[rceni - 1].c;
			cp->ncc.max++;
		}
	}
}

void _oracle3::draw(int n, _area2 area, _bitmap* bm)
{
	static _prices pri[61]; // цены
	static int min, max; // разброс по y
	min = 0;
	max = 1;
	for (auto& i : pri) i.clear();

	for (int64 i = zn[n].ncc.min; i < zn[n].ncc.max; i++)
	{
		if (i < begin_ss)
		{
			int64 delta = begin_ss - i;
			if (delta >= max_part)
				part_ss.clear();
			else
			{
				_prices w;
				w.clear();
				for (int i = 0; i < delta; i++)
				{
					part_ss.push_front(w);
					if (part_ss.size() > max_part) part_ss.pop_back();
				}
			}
			begin_ss = i;
		}
		if (i >= begin_ss + (int64)part_ss.size())
		{
			_prices w;
			w.clear();
			int64 delta = i - (begin_ss + (int)part_ss.size()) + 1;
			if (delta >= max_part)
			{
				part_ss.clear();
				part_ss.push_back(w);
				begin_ss = i;
			}
			else
				for (int i = 0; i < delta; i++)
				{
					part_ss.push_back(w);
					if (part_ss.size() > max_part)
					{
						part_ss.pop_front();
						begin_ss++;
					}
				}
		}
		int64 ii = i - begin_ss;
		if (part_ss[ii].empty()) ss->read(i, part_ss[ii]);
		pri[part_ss[ii].time.second] = part_ss[ii];
		min = zn[n].min - 1;
		max = zn[n].max;
	}

	_area2 oo = area;
	int x1 = (int)oo.x.min;
	int x2 = (int)oo.x.max;
	int dx = x2 - x1;
	if (dx < 2) return;
	int step = 60;
	if (dx >= 4) step = 30;
	if (dx >= 6) step = 20;
	if (dx >= 8) step = 15;
	if (dx >= 10) step = 12;
	if (dx >= 12) step = 10;
	if (dx >= 20) step = 6;
	if (dx >= 24) step = 5;
	if (dx >= 30) step = 4;
	if (dx >= 40) step = 3;
	if (dx >= 60) step = 2;
	if (dx >= 120) step = 1;
	int kol = 60 / step;
	int dd = max - min;
	double ddy = oo.y.max - oo.y.min;
	for (int i = 0; i < kol; i++)
	{
		int ss = i * step;
		while (pri[ss].empty())
		{
			if (ss + 1 >= (i + 1) * step) break;
			ss++;
		}
		if (pri[ss].empty()) continue;
		int xx1 = x1 + (x2 - x1) * i / kol;
		int xx2 = x1 + (x2 - x1) * (i + 1) / kol - 1;
		for (int j = rceni - 1; j >= 0; j--)
		{
			int64 ce = pri[ss].pro[j].c;
			int yy1 = (int)(oo.y.min + (max - ce) * ddy / dd);
			int yy2 = (int)(oo.y.min + (max - ce + 1) * ddy / dd) - 1;
			if (yy2 < yy1) continue;
			uint q = (uint)sqrt(pri[ss].pro[j].k) + 32;
			if (q > 255) q = 255;
			uint cc = (q << 8) + (q << 16) + 0xA0000000;
			bm->fill_rectangle({ {xx1, xx2}, {yy1, yy2} }, cc);
		}
		for (int j = 0; j < rceni; j++)
		{
			int64 ce = pri[ss].pok[j].c;
			int yy1 = (int)(oo.y.min + (max - ce) * ddy / dd);
			int yy2 = (int)(oo.y.min + (max - ce + 1) * ddy / dd) - 1;
			if (yy2 < yy1) continue;
			uint q = (uint)sqrt(pri[ss].pok[j].k) + 32;
			if (q > 255) q = 255;
			uint cc = q + (q << 8) + 0xA0000000;
			bm->fill_rectangle({ {xx1, xx2}, {yy1, yy2} }, cc);
		}
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
