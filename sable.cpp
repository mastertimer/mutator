/*//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

          | результат | итераций | 1 итерация
---------------------------------------------
0.999     |   0.941        61       0.99900
min(rnd)  |   0.873        55       0.99753
max(rnd)  |   1.058        58       1.00097
было(err) |   0.925        61       0.99872
было      |   0.955        21       0.99779

~ коэфиициент - насколько числа красивые? у кого красивее - у покупки или у продажи? (у всех и у 10-15)

/*//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <array>
#include <algorithm>
#include <sstream>

#include "sable.h"

constexpr wchar_t ss_file[]  = L"..\\..\\baza.cen";
constexpr wchar_t mmm_file[] = L"..\\..\\mmm.txt";
constexpr _prices cena_zero_ = { {}, {}, { 1,1,1,1,1 } };

_super_stat      ss;               // сжатые цены
_sable_stat      sss;              // сжатые цены
_sable_graph    *graph  = nullptr; // график

_nervous_oracle *oracle = nullptr; // оракул
_mctds_candle   *sv     = nullptr;
_oracle3        *o3     = nullptr;
_view_stat      *o_test = nullptr; // тестовый график

_recognize       recognize;

std::wstring mmm1 = L"1";
std::wstring mmm2 = L"2";
std::wstring mmm3 = L"3";

int kkk2 = 13; // количество продаваемых акций

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void load_mmm(wstr file_name)
{
	_rjson fs(file_name);
	fs.read("mmm1", mmm1);
	fs.read("mmm2", mmm2);
	fs.read("mmm3", mmm3);
}

void fun13(_tetron* tt0, _tetron* tt, u64 flags)
{
	static bool first = true; if (!first) return; first = false;
	ss.load_from_file((exe_path + ss_file).c_str());
	if (!graph) return;
	if (!graph->find1<_g_scrollbar>(flag_part))
	{
		_g_scrollbar* sb = new _g_scrollbar;
		sb->vid = 2;
		graph->add_flags(sb, flag_sub_go + flag_part + (flag_run << 32));
	}
	sv = new _mctds_candle;
	oracle = new _nervous_oracle;
	o3 = new _oracle3;
	o_test = new _view_stat;
	graph->curve.push_back(std::unique_ptr<_basic_curve>(sv));
//	graph->curve.push_back(std::unique_ptr<_basic_curve>(oracle));
	if (o_test) graph->curve.push_back(std::unique_ptr<_basic_curve>(o_test));
	if (o3) graph->curve.push_back(std::unique_ptr<_basic_curve>(o3));
	sv->recovery();
	oracle->recovery();
	if (o3) o3->recovery();
	if (o_test) o_test->recovery();
	graph->cha_area();
	load_mmm((exe_path + mmm_file).c_str());
}

void fun15(_tetron* tt0, _tetron* tt, u64 flags)
{
	for (_frozen i(n_timer1000, flag_run); i; i++)
	{
		_t_function* f = *i;
		if (!f) continue;
		if (f->a == 16) delete f;
	}
	_g_button* b = *tt0;
	if (b->checked) n_timer1000->add_flags(new _t_function(16), flag_run);
}

bool can_trade     = false; // разрешение на торговлю
bool zamok_pokupki = false; // простой блокиратор
int popitok_prodaz = 2;     // сколько раз можно купить/продать
int gotovo_prodaz  = 0;     // сколько сделок
int vrema_prodat   = 0;     // время когда нужно продать

void fun16(_tetron* tt0, _tetron* tt, u64 flags)
{
	if (zamok_pokupki) return;
	_prices a;
	int ok = recognize.read_prices_from_screen(&a);
	if (ok != 0)
	{
		/*#ifdef DEBUG_MMM
				wstring fn = exe_path + L"errorscr.bmp";
				if (!FileExists(fn.c_str()))
				{
					recognize.image_.SaveToFile(fn.c_str());
					ofstream file(exe_path + L"errorscr.txt");
					file << ok;
					file.close();
				}
		#endif // DEBUG_MMM*/
		return;
	}
	ss.add(a);
	sv->recovery();
	oracle->recovery();
	if (o3) o3->recovery();

	graph->run(nullptr, graph, flag_run);
	// всякие проверки на начало покупки !!!!

	if (!can_trade) return;
	if (oracle->zn.size() < 10) return;
	if (oracle->zn.back().time + 60 != a.time.to_minute()) return;


	if (gotovo_prodaz & 1) // была покупка, но небыло продажи
	{
		if ((a.time >= vrema_prodat) || ((a.time.hour == 18) && (a.time.minute > 30)) || oracle->get_latest_events(oracle->zn.size() - 1).stop())
		{
			/*			int b;
						recognize.ReadTablicaZayavok(0, b);
						if (b != gotovo_prodaz)// лажа, не все купилось, прекратить
						{
							gotovo_prodaz = 0;
							popitok_prodaz = 0;
							return;
						}*/
			zamok_pokupki = true;
			gotovo_prodaz++;
			_t_function* fu = new _t_function(36);
			fu->run(0, fu, flag_run);
		}
		return;
	}

	if (popitok_prodaz < 1) return;
	if (a.time.hour >= 18) return; // слишком поздно
	int ti = oracle->get_latest_events(oracle->zn.size() - 1).start();


	if (ti == 0) return;
	// купить акции
/*	int b;
	recognize.ReadTablicaZayavok(0, b);
	if (b != gotovo_prodaz)// лажа, не все купилось, прекратить
	{
		gotovo_prodaz = 0;
		popitok_prodaz = 0;
		return;
	}*/
	zamok_pokupki = true;
	vrema_prodat = a.time + ti * 60;
	popitok_prodaz--;
	gotovo_prodaz++;
	_t_function* fu = new _t_function(35);
	fu->run(0, fu, flag_run);
}

void fun19(_tetron* tt0, _tetron* tt, u64 flags)
{
	_g_button* b = *tt0;
	can_trade = b->checked;
}

void fun20(_tetron* tt0, _tetron* tt, u64 flags)
{
	if (zamok_pokupki) return;
	if (mmm1 == L"1") load_mmm((exe_path + mmm_file).c_str());
	zamok_pokupki = true;
	_t_function* fu = new _t_function(35);
	fu->run(0, fu, flag_run);
}

void fun21(_tetron* tt0, _tetron* tt, u64 flags)
{
	if (zamok_pokupki) return;
	if (mmm1 == L"1") load_mmm((exe_path + mmm_file).c_str());
	zamok_pokupki = true;
	_t_function* fu = new _t_function(36);
	fu->run(0, fu, flag_run);
}

void fun22(_tetron* tt0, _tetron* tt, u64 flags)
{
	graph->size_el++;
	graph->cha_area();
}

void fun30(_tetron* tt0, _tetron* tt, u64 flags)
{
	if (graph->size_el > 1) graph->size_el--;
	graph->cha_area();
}

void mouse_move_click(i64 x, i64 y)
{
	SetCursorPos((int)x, (int)y);
	mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0);
	mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
}

void mouse_click()
{
	mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0);
	mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
}

int bad_string_to_int(std::wstring& s)
{
	int r = 0;
	int ii;
	int l = (int)s.size();
	wstr ss_ = s.data();
	for (ii = 0; ii < l; ii++) if ((ss_[ii] >= L'0') && (ss_[ii] <= L'9')) break;
	for (int i = ii; i < l; i++)
		if ((ss_[i] >= L'0') && (ss_[i] <= L'9'))
			r = r * 10 + (ss_[i] - L'0');
		else
			break;
	return r;
}

void buy_stock(_tetron* tt, bool buy)
{
	int otst_20 = 1; // 20 >= x >= 1, 1 - лучшая цена
	static int KKK;
	static bool win8 = false;
	static __int64 n = 0;
	// шаг 01: внедрение и инициализация
	if (tt->link.empty())
	{
		n_timer250->add_flags(tt, flag_run);
		return;
	}
	// пауза
	if (n < 1)
	{
		n = n + 1;
		return;
	}
	// шаг 01: двойной клик по цене
	if (n == 1)
	{
		n = 1000;
		RECT rr;
		if (!recognize.find_window_prices(&rr)) return;
		if (buy)
			mouse_move_click(rr.left + 20ll, 6ll + rr.top + 15 * (4ll - otst_20));
		else
			mouse_move_click(rr.left + 20ll, 6ll + rr.top + 15 * (37ll + otst_20));
		mouse_click();
		n = 2;
		return;
	}
	// шаг 02: поиск окна и нажатие на код клиента
	if (n < 13)
	{
		int er = recognize.read_vvod_zaya();
		if (er)
		{
			n = n + 1;
			if (n == 13)
			{
				n = 1000;
				//        ra2.kar_->bitmap_->SaveToFile("xxx.bmp");
				//				MessageBox(0, (L"не найдено окно, ошибка " + to_wstring(er)).data(), L"упс..", MB_OK | MB_TASKMODAL);
			}
			return;
		}
		n = 1000;
		int n_pok = recognize.find_elem(L"Покупка");
		int n_pro = recognize.find_elem(L"Продажа");
		if ((n_pok < 0) || (n_pro < 0))
		{
			//			MessageBox(0, L"не найдена покупка/продажа", L"упс..", MB_OK | MB_TASKMODAL);
			return;
		}
		uint c = recognize.image.sl(recognize.elem[n_pok].area.y.min)[recognize.elem[n_pok].area.x.min - 1];
		uchar* cc = (uchar*)& c;
		bool cfpok = (cc[0] != cc[1]) || (cc[0] != cc[2]); // не серый цвет
		c = recognize.image.sl(recognize.elem[n_pro].area.y.min)[recognize.elem[n_pro].area.x.min - 1];
		cc = (uchar*)& c;
		bool cfpro = (cc[0] != cc[1]) || (cc[0] != cc[2]); // не серый цвет
//		bool cfpok = (recognize.image.sl(recognize.elem[n_pok].area.y.min)[recognize.elem[n_pok].area.x.min - 1] != recognize.image.data[0]);
//		bool cfpro = (recognize.image.sl(recognize.elem[n_pro].area.y.min)[recognize.elem[n_pro].area.x.min - 1] != recognize.image.data[0]);
		if (cfpok == cfpro)
		{
			//			MessageBox(0, L"одинаковые цвета покупка/продажа", L"упс..", MB_OK | MB_TASKMODAL);
			return;
		}
		if (buy)
		{
			if (!cfpok)
			{
				//				MessageBox(0, L"НЕ покупка", L"упс..", MB_OK | MB_TASKMODAL);
				return;
			}
		}
		else
		{
			if (!cfpro)
			{
				//				MessageBox(0, L"НЕ продажа", L"упс..", MB_OK | MB_TASKMODAL);
				return;
			}

		}
		int kk = recognize.find_elem(L"КодКлиента");
		int kk2 = recognize.find_elem(L"Поручение");
		if ((kk < 0) || (kk2 < 0))
		{
			//			MessageBox(0, L"странно нет кода клиента", L"упс..", MB_OK | MB_TASKMODAL);
			return;
		}
		if (abs(recognize.elem[kk].area.x.min - recognize.elem[kk2].area.x.min) < 5) win8 = true;
		if (win8)
			mouse_move_click(recognize.elem[kk].area.x.min + 260 + recognize.offset.x, recognize.elem[kk].area.y.min + 3 + recognize.offset.y);
		else
			mouse_move_click(recognize.elem[kk].area.x.min + 110 + recognize.offset.x, recognize.elem[kk].area.y.min + 30 + recognize.offset.y);
		n = 13;
		return;
	}
	// шаг 03: выбор кода клиента 
	if (n == 13)
	{
		int kk = recognize.find_elem(L"КодКлиента");
		if (kk < 0)
		{
			n = 1000;
			return;
		}
		if (win8)
			mouse_move_click(recognize.elem[kk].area.x.min + 220 + recognize.offset.x, recognize.elem[kk].area.y.min + 20 + recognize.offset.y);
		else
			mouse_move_click(recognize.elem[kk].area.x.min + 70 + recognize.offset.x, recognize.elem[kk].area.y.min + 47 + recognize.offset.y);
		n = 14;
		return;
	}
	// шаг 04: щелканье кода клиента
	if (n == 14)
	{
		int kk = recognize.find_elem(L"КодКлиента");
		if (kk < 0)
		{
			n = 1000;
			return;
		}
		if (win8)
			mouse_move_click(recognize.elem[kk].area.x.min + 220 + recognize.offset.x, recognize.elem[kk].area.y.min + 3 + recognize.offset.y);
		else
			mouse_move_click(recognize.elem[kk].area.x.min + 70 + recognize.offset.x, recognize.elem[kk].area.y.min + 30 + recognize.offset.y);
		n = 15;
		return;
	}
	// шаг 05: поиск кода клиента, щелчок по количеству
	if (n == 15)
	{
		int er = recognize.read_vvod_zaya();
		if (er)
		{
			n = 1000;
			return;
		}
		if (recognize.find_elem(mmm1) < 0)
		{
			n = 1000;
			return;
		}
		int kk = recognize.find_elem_kusok(L"Кол-во");
		if (kk < 0)
		{
			n = 1000;
			return;
		}
		if (win8)
			mouse_move_click(recognize.elem[kk].area.x.min + 100 + recognize.offset.x, recognize.elem[kk].area.y.min + 3 + recognize.offset.y);
		else
			mouse_move_click(recognize.elem[kk].area.x.min + 70 + recognize.offset.x, recognize.elem[kk].area.y.min + 30 + recognize.offset.y);
		n = 16;
		return;
	}
	// шаг 06: ввод количества
	if (n == 16)
	{
		// выделение
		keybd_event(0x11, 0, 0, 0);
		keybd_event('A', 0, 0, 0);
		keybd_event('A', 0, KEYEVENTF_KEYUP, 0);
		keybd_event(0x11, 0, KEYEVENTF_KEYUP, 0);
		// удаление
		keybd_event(0x2E, 0, 0, 0);
		keybd_event(0x2E, 0, KEYEVENTF_KEYUP, 0);
		if (!win8)
		{
			// определение количества
			int kmax = recognize.find_elem_kusok(L"mах:");
			int klot = recognize.find_elem_kusok(L"Кол-во(лот");
			if ((kmax < 0) || (klot < 0))
			{
				n = 1000;
				return;
			}
			kmax = bad_string_to_int(recognize.elem[kmax].s);
			klot = bad_string_to_int(recognize.elem[klot].s);
			if (klot == 0)
			{ // на всякий случай
				n = 1000;
				return;
			}
		}
		//		KKK = KKK2 / klot;
		KKK = kkk2;
		if (KKK == 0) /*||(KKK > kmax)*/
		{ // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
			n = 1000;
			return;
		}
		// ввод количества
		std::string s = std::to_string(KKK);
		for (int i = 0; i < s.size(); i++)
		{
			keybd_event(s[i], 0, 0, 0);
			keybd_event(s[i], 0, KEYEVENTF_KEYUP, 0);
		}
		n = 17;
		return;
	}
	// шаг 07: проверка и нажатие на Да
	if (n == 17)
	{
		int er = recognize.read_vvod_zaya();
		if (er)
		{
			n = 1000;
			return;
		}
		if ((recognize.find_elem(mmm1) < 0) || (recognize.find_elem(std::to_wstring(KKK).c_str()) < 0))
		{
			n = 1000;
			return;
		}
		int kk = recognize.find_elem(L"Да");
		if (kk < 0)
		{
			n = 1000;
			return;
		}
		SetCursorPos((int)(recognize.elem[kk].area.x.min + 20 + recognize.offset.x), (int)(recognize.elem[kk].area.y.min + 5 + recognize.offset.y));


		mouse_click(); // НЕЛЬЗЯ
		n = 18;
		return;
	}
	if (n < 29)
	{
		int er = (buy) ? recognize.read_vnimanie_pokupka() : recognize.read_vnimanie_prodaza();
		if (er)
		{
			n = n + 1;
			if (n == 29)
			{
				n = 1000;
				//        ra2.kar_->bitmap_->SaveToFile("xxx.bmp");
				//				MessageBox(0, (L"не найдено окно внимание, ошибка " + to_wstring(er)).data(), L"упс..", MB_OK | MB_TASKMODAL);
			}
			return;
		}
		int kk = recognize.find_elem(L"ОК");
		if (kk < 0)
		{
			n = 1000;
			return;
		}
		SetCursorPos((int)(recognize.elem[kk].area.x.min + 20 + recognize.offset.x), (int)(recognize.elem[kk].area.y.min + 5 + recognize.offset.y));
		mouse_click();                                                     // НЕЛЬЗЯ
		n = 29;
		return;
	}
	if (n < 40)
	{
		int er = recognize.read_okno_soobsenii();
		if (er)
		{
			n = n + 1;
			if (n == 40)
			{
				n = 1000;
				//        ra2.kar_->bitmap_->SaveToFile("xxx.bmp");
				//				MessageBox(0, (L"не найдено окно сообщений, ошибка " + to_wstring(er)).data(), L"упс..", MB_OK | MB_TASKMODAL);
			}
			return;
		}
		SetCursorPos((int)(recognize.offset.x + recognize.image.size.x - 23), (int)(recognize.offset.y - 15));
		mouse_click(); // НЕЛЬЗЯ
		n = 40;
		return;
	}
	delete tt;
	n = 0;
	zamok_pokupki = false;
}

void fun35(_tetron* tt0, _tetron* tt, u64 flags)
{
	buy_stock(tt, true);
}

void fun36(_tetron* tt0, _tetron* tt, u64 flags)
{
	buy_stock(tt, false);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void _date_time::now()
{
	SYSTEMTIME t;
	GetLocalTime(&t);
	month  = t.wMonth + (t.wYear - 2017) * 12;
	day    = (uchar)t.wDay;
	hour   = (uchar)t.wHour;
	minute = (uchar)t.wMinute;
	second = (uchar)t.wSecond;
}

void _date_time::operator =(int a)
{
	month  = (uchar)(a / 2764800L);
	day    = (uchar)((a -= month * 2764800L) / 86400L);
	hour   = (uchar)((a -= day * 86400L) / 3600L);
	minute = (uchar)((a -= hour * 3600L) / 60L);
	second = (uchar)(a - minute * 60L);
}

int _date_time::to_minute()
{
	return (((month * 32L + day) * 24L + hour) * 60L + minute) * 60L;
}

_date_time::operator int() const noexcept
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

void _super_stat::read(i64 n, _prices& c, _info_pak* inf)
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
		i64 k = n / step_pak_cc;
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
	for (int j = c.pro[0].c, n2 = 0, n_ = 0; n2 < roffer;)
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
			while ((j > read_cc.pro[n_].c) && (n_ < roffer - 1)) n_++;//
			int kk3 = (j == read_cc.pro[n_].c) ? read_cc.pro[n_].k : 0;//
			int kk2 = kk + kk3;//
			if (kk2 > 0)
			{
				if (n2 >= roffer)
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
	for (int j = c.pok[0].c, n2 = 0, n_ = 0; n2 < roffer;)
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
			while ((j < read_cc.pok[n_].c) && (n_ < roffer - 1)) n_++;//
			int kk3 = (j == read_cc.pok[n_].c) ? read_cc.pok[n_].k : 0;//
			int kk2 = kk + kk3;//
			if (kk2 > 0)
			{
				if (n2 >= roffer)
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
	for (int j = c.pro[0].c, n2 = 0, n = 0; j <= c.pro[roffer - 1].c; j++)
	{
		if (Vrez == 64)
		{
			otgruzka(rez, Vrez, deko);
			Vrez = 0;
			rez = 0;
		}
		if (j > c.pro[n2].c) n2++;
		while ((j > last_cc.pro[n].c) && (n < roffer - 1)) n++; //
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
	for (int j = c.pok[0].c, n2 = 0, n = 0; j >= c.pok[roffer - 1].c; j--)
	{
		if (Vrez == 64)
		{
			otgruzka(rez, Vrez, deko);
			Vrez = 0;
			rez = 0;
		}
		if (j < c.pok[n2].c) n2++;
		while ((j < last_cc.pok[n].c) && (n < roffer - 1)) n++; //
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
	ip_last.ok = false;
	ip_n.ok = false;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void _bit_vector::resize(i64 v)
{
	i64 rp = (i64)data.size();
	i64 r = v / 64;
	bit = v - r * 64;
	if (rp == r)
	{
		byte &= (1ui64 << bit) - 1;
		return;
	}
	if (r > rp)
	{
		data.resize(r);
		data[rp] = byte;
		byte = 0;
		return;
	}
	byte = data[r] & ((1ui64 << bit) - 1);
	data.resize(r);
}

void _bit_vector::push1(u64 a) noexcept
{
	byte |= ((a & 1) << bit);
	if (bit < 63) { bit++; return; }
	data.push_back(byte);
	byte = bit = 0;
}

void _bit_vector::pushn(u64 a, uchar n) noexcept
{
	for (; n; n--)
	{
		byte |= ((a & 1) << bit);
		if (bit < 63) bit++;
		else
		{
			data.push_back(byte);
			byte = bit = 0;
		}
		a >>= 1;
	}
}

void _bit_vector::pushn1(u64 a) noexcept
{
	while (a > 1)
	{
		byte |= ((a & 1) << bit);
		if (bit < 63) bit++;
		else
		{
			data.push_back(byte);
			byte = bit = 0;
		}
		a >>= 1;
	}
}

/*_bit_stream::~_bit_stream()
{
	pushn(0, (8 - bit) % 8); // заполнить последний байт
}*/

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace
{
	_cdf f_number({ {1, 0, 44}, {2, 0, 187}, {3, 1, 186}, {5, 0, 207}, {6, 2, 130}, {10, 0, 112}, {11, 2, 291},
		{15, 3, 237}, {21, 2, 695}, {25, 0, 91}, {26, 2, 478}, {30, 0, 65}, {31, 4, 229}, {40, 3, 139}, {48, 1, 401},
		{50, 0, 46}, {51, 0, 92}, {52, 1, 147}, {54, 1, 131}, {56, 0, 221}, {57, 1, 244}, {59, 1, 88}, {61, 2, 64},
		{65, 2, 251}, {69, 1, 311}, {71, 2, 154}, {75, 1, 184}, {77, 3, 239}, {85, 1, 309}, {87, 3, 175}, {95, 3, 343},
		{100, 0, 22}, {101, 1, 307}, {103, 0, 170}, {104, 3, 151}, {112, 3, 195}, {120, 2, 319}, {124, 1, 143},
		{126, 4, 126}, {141, 3, 447}, {149, 1, 487}, {151, 2, 248}, {155, 3, 189}, {163, 4, 223}, {179, 3, 160},
		{187, 4, 227}, {200, 0, 40}, {201, 2, 158}, {205, 0, 273}, {206, 2, 194}, {210, 0, 437}, {211, 1, 287},
		{213, 2, 165}, {217, 2, 247}, {221, 2, 415}, {225, 0, 180}, {226, 1, 471}, {228, 3, 84}, {236, 3, 199},
		{244, 3, 253}, {250, 0, 25}, {251, 2, 135}, {255, 0, 299}, {256, 2, 419}, {260, 0, 100}, {261, 4, 113},
		{276, 3, 218}, {284, 4, 82}, {300, 0, 178}, {301, 3, 173}, {309, 4, 69}, {325, 4, 68}, {341, 3, 359},
		{349, 2, 427}, {353, 3, 234}, {361, 4, 242}, {372, 3, 224}, {380, 5, 77}, {410, 4, 157}, {426, 5, 80},
		{454, 4, 250}, {470, 5, 74}, {502, 5, 167}, {534, 3, 1975}, {542, 5, 243}, {574, 5, 245}, {606, 4, 161},
		{622, 4, 435}, {638, 5, 235}, {670, 6, 98}, {734, 7, 124}, {832, 6, 149}, {896, 7, 255}, {1024, 7, 213},
		{1152, 8, 211}, {1408, 9, 203}, {1920, 10, 225}, {2944, 12, 209}, {7040, 14, 350}, {23424, 24, 1463},
		{10000001, 0, 1} }); // 1...670 000

	static const _cdf f_delta({ {1, 0, 3}, {2, 0, 6}, {3, 0, 12}, {4, 0, 16}, {5, 0, 40}, {6, 0, 88}, {7, 0, 440},
		{8, 3, 248}, {16, 10, 312}, {1001, 0, 1} }); // 1...345
}

bool _sable_stat::add0(const _prices2& c)
{
	data.pushn(c.buy[roffer - 1].value, 16);

	static const _cdf nnd({ {1, 0, 27}, {2, 0, 23}, {3, 0, 10}, {4, 0, 9}, {5, 0, 14}, {6, 0, 31}, {7, 1, 13},
		{9, 1, 8}, {11, 1, 19}, {13, 2, 52}, {17, 4, 28}, {33, 10, 36}, {1001, 0, 1} }); // 1...210

	for (i64 i = roffer - 1; i >= 0; i--)
	{
		if (i != roffer - 1)
		{
			if (!f_delta.coding(c.buy[i].value - c.buy[i + 1].value, data)) return false;
		}
		if (!f_number.coding(c.buy[i].number, data)) return false;
	}
	if (!nnd.coding(c.sale[0].value - c.buy[0].value, data)) return false;
	for (i64 i = 0; i < roffer; i++)
	{
		if (i != 0)
		{
			if (!f_delta.coding(c.sale[i].value - c.sale[i - 1].value, data)) return false;
		}
		if (!f_number.coding(c.sale[i].number, data)) return false;
	}
	base_buy.resize(roffer);
	base_sale.resize(roffer);
	for (i64 i = 0; i < roffer; i++)
	{
		base_buy[i] = c.buy[i];
		base_sale[i] = c.sale[i];
	}
	return true;
}

i64 calc_delta_del_add(const _one_stat* c, const std::vector<_one_stat> &v)
{
	if (c[0].value == v[0].value) return 0;
	if (c[1].value > c[0].value)
	{
		if (c[0].value < v[0].value)
		{
			for (i64 i = 1; i < roffer; i++) if (c[i].value >= v[0].value) return i;
			return roffer;
		}
		for (i64 i = 1; i < (i64)v.size(); i++) if (v[i].value >= c[0].value) return -i;
		return -((i64)v.size());
	}
	if (c[0].value > v[0].value)
	{
		for (i64 i = 1; i < roffer; i++) if (c[i].value <= v[0].value) return i;
		return roffer;
	}
	for (i64 i = 1; i < (i64)v.size(); i++) if (v[i].value <= c[0].value) return -i;
	return -((i64)v.size());
}

i64 calc_delta_del_add1(const _one_stat* c, const std::vector<_one_stat>& v, i64 n)
{
	if (c[n].value == v[n].value) return 0;
	if (c[1].value > c[0].value)
	{
		if (c[n].value < v[n].value) return 1;
		return -1;
	}
	if (c[n].value > v[n].value) return 1;
	return -1;
}

i64 calc_series_value(const _one_stat* c, const std::vector<_one_stat>& v, i64 n)
{
	i64 k = std::min(roffer, (i64)v.size());
	for (i64 i = n; i < k; i++) if (c[i].value != v[i].value) return i - n;
	return k - n;
}

i64 calc_series_number(const _one_stat* c, const std::vector<_one_stat>& v, i64 n, i64 k)
{
	for (i64 i = n; i < k; i++) if (c[i].number != v[i].number) return i - n;
	return k - n;
}

bool _sable_stat::delta_number(i64 a, i64 b)
{
	static const _cdf ttrr1({ {-65, 4, 251}, {-51, 1, 141}, {-49, 4, 447}, {-38, 3, 443}, {-30, 0, 205}, {-29, 2, 476},
		{-25, 0, 43}, {-24, 3, 153}, {-16, 3, 196}, {-10, 0, 228}, {-9, 2, 209}, {-5, 0, 147}, {-4, 1, 129},
		{-2, 0, 137}, {-1, 0, 115}, {0, 0, 2124}, {1, 0, 32}, {2, 0, 143}, {3, 1, 245}, {5, 0, 199}, {6, 2, 180},
		{10, 0, 197}, {11, 2, 361}, {15, 3, 263}, {20, 0, 489}, {21, 2, 348}, {25, 0, 14}, {26, 2, 184}, {30, 0, 113},
		{31, 3, 252}, {38, 2, 295}, {42, 3, 157}, {50, 0, 91}, {51, 0, 244}, {52, 2, 221}, {56, 0, 248}, {57, 2, 401},
		{60, 0, 200}, {61, 1, 291}, {63, 3, 159}, {71, 2, 831}, {75, 0, 133}, {76, 3, 211}, {83, 2, 188}, {87, 2, 193},
		{91, 2, 479}, {95, 0, 647}, {96, 2, 232}, {100, 0, 10}, {101, 1, 408}, {103, 3, 229}, {111, 3, 136},
		{119, 3, 169}, {125, 0, 97}, {126, 4, 80}, {142, 3, 429}, {150, 0, 460}, {151, 2, 575}, {155, 3, 511},
		{163, 4, 227}, {179, 4, 253}, {191, 3, 156}, {199, 0, 844}, {200, 0, 55}, {201, 2, 240}, {205, 3, 84},
		{213, 2, 168}, {217, 3, 239}, {225, 0, 189}, {226, 1, 280}, {228, 3, 237}, {236, 4, 108}, {250, 0, 419},
		{251, 1, 903}, {253, 3, 273}, {260, 0, 85}, {261, 4, 249}, {277, 4, 181}, {293, 1, 1612}, {295, 3, 351},
		{303, 4, 201}, {319, 4, 140}, {335, 5, 217}, {356, 4, 176}, {372, 5, 131}, {402, 4, 301}, {418, 5, 164},
		{445, 4, 315}, {461, 5, 303}, {485, 4, 431}, {501, 6, 231}, {565, 6, 185}, {609, 5, 195}, {641, 7, 207},
		{755, 6, 423}, {819, 6, 165}, {883, 7, 132}, {1011, 8, 383}, {1267, 11, 216}, {3315, 24, 3148},
		{10000001, 0, 1} });

	static const _cdf ttrr2({ {-199, 6, 204}, {-150, 1, 276}, {-148, 5, 133}, {-125, 0, 205}, {-124, 3, 186},
		{-116, 4, 234}, {-104, 2, 399}, {-100, 0, 23}, {-99, 3, 171}, {-92, 2, 477}, {-88, 1, 361}, {-86, 2, 188},
		{-82, 3, 141}, {-75, 0, 253}, {-74, 3, 187}, {-66, 3, 146}, {-60, 2, 201}, {-56, 0, 271}, {-55, 2, 218},
		{-51, 0, 1007}, {-50, 0, 98}, {-49, 0, 1547}, {-48, 3, 149}, {-40, 3, 138}, {-34, 2, 473}, {-30, 0, 81},
		{-29, 2, 501}, {-25, 0, 14}, {-24, 2, 341}, {-20, 1, 497}, {-18, 3, 189}, {-10, 0, 395}, {-9, 2, 367},
		{-5, 0, 212}, {-4, 1, 489}, {-2, 0, 345}, {-1, 0, 100}, {0, 0, 2059}, {1, 0, 114}, {2, 0, 373}, {3, 1, 378},
		{5, 0, 319}, {6, 2, 369}, {10, 0, 301}, {11, 3, 156}, {19, 3, 383}, {25, 0, 8}, {26, 2, 469}, {30, 0, 116},
		{31, 2, 404}, {35, 4, 251}, {50, 0, 97}, {51, 0, 508}, {52, 2, 177}, {56, 2, 210}, {60, 1, 140}, {62, 4, 219},
		{75, 0, 223}, {76, 3, 194}, {84, 3, 185}, {92, 2, 429}, {96, 2, 349}, {100, 0, 19}, {101, 0, 892},
		{102, 4, 203}, {117, 3, 170}, {125, 0, 237}, {126, 4, 169}, {140, 3, 357}, {148, 4, 159},
		{164, 4, 485}, {175, 0, 449}, {176, 5, 207}, {200, 0, 108}, {201, 1, 636}, {203, 3, 220}, {211, 4, 157},
		{225, 0, 751}, {226, 2, 779}, {230, 4, 165}, {246, 4, 202}, {260, 0, 175}, {261, 5, 197}, {287, 4, 491},
		{303, 5, 137}, {334, 4, 506}, {350, 5, 154}, {382, 5, 196}, {414, 5, 363}, {442, 5, 511}, {474, 5, 153},
		{506, 6, 181}, {570, 6, 132}, {622, 5, 321}, {654, 7, 129}, {762, 6, 447}, {826, 6, 249}, {890, 8, 155},
		{1146, 10, 130}, {2170, 24, 3083}, {10000001, 0, 1} });

	static const _cdf ttrr3({ {-317, 5, 84}, {-287, 3, 421}, {-279, 4, 461}, {-268, 3, 273}, {-260, 0, 105},
		{-259, 4, 295}, {-250, 0, 473}, {-249, 4, 189}, {-235, 3, 375}, {-228, 2, 204}, {-224, 3, 134}, {-216, 2, 417},
		{-212, 3, 225}, {-204, 2, 401}, {-200, 0, 99}, {-199, 4, 237}, {-183, 3, 414}, {-175, 0, 599}, {-174, 4, 201},
		{-158, 3, 413}, {-150, 0, 278}, {-149, 4, 133}, {-133, 3, 196}, {-125, 0, 102}, {-124, 2, 293}, {-120, 0, 855},
		{-119, 4, 220}, {-108, 3, 198}, {-100, 0, 10}, {-99, 1, 286}, {-97, 2, 349}, {-93, 3, 129}, {-85, 4, 193},
		{-75, 0, 213}, {-74, 4, 195}, {-60, 2, 259}, {-56, 1, 214}, {-54, 2, 289}, {-50, 0, 108}, {-49, 4, 132},
		{-40, 3, 140}, {-32, 1, 1613}, {-30, 0, 118}, {-29, 2, 406}, {-25, 0, 8}, {-24, 3, 222}, {-16, 3, 467},
		{-10, 0, 369}, {-9, 2, 373}, {-5, 2, 183}, {-1, 0, 100}, {0, 0, 2125}, {1, 0, 121}, {2, 0, 285}, {3, 1, 430},
		{5, 0, 407}, {6, 2, 510}, {10, 0, 403}, {11, 4, 243}, {25, 0, 31}, {26, 2, 302}, {30, 0, 116}, {31, 2, 531},
		{35, 4, 181}, {50, 0, 78}, {51, 2, 382}, {54, 1, 345}, {56, 2, 455}, {60, 1, 423}, {62, 4, 149}, {75, 0, 141},
		{76, 4, 387}, {85, 3, 279}, {93, 3, 238}, {100, 0, 27}, {101, 4, 177}, {117, 3, 503}, {125, 0, 179},
		{126, 5, 124}, {158, 5, 153}, {184, 4, 501}, {200, 0, 197}, {201, 4, 156}, {217, 5, 253}, {244, 4, 263},
		{260, 0, 173}, {261, 6, 231}, {325, 6, 137}, {389, 4, 845}, {405, 6, 190}, {468, 5, 327}, {500, 0, 787},
		{501, 5, 477}, {533, 7, 229}, {661, 7, 471}, {772, 6, 497}, {836, 6, 391}, {900, 8, 209}, {1156, 12, 339},
		{5252, 24, 3149}, {10000001, 0, 1} });

	static const _cdf ttrr4({ {-10000000, 24, 2717}, {-6096, 12, 1181}, {-2214, 10, 153}, {-1192, 7, 297},
		{-1064, 7, 238}, {-945, 6, 475}, {-887, 5, 194}, {-855, 6, 112}, {-791, 5, 959}, {-767, 5, 425}, {-735, 6, 178},
		{-671, 6, 230}, {-625, 5, 134}, {-593, 6, 237}, {-539, 5, 225}, {-507, 3, 210}, {-499, 5, 207}, {-467, 5, 141},
		{-438, 3, 958}, {-430, 5, 155}, {-398, 5, 166}, {-375, 4, 154}, {-359, 5, 255}, {-327, 5, 129}, {-308, 3, 402},
		{-300, 4, 221}, {-284, 5, 159}, {-260, 0, 81}, {-259, 3, 863}, {-253, 2, 319}, {-249, 4, 193}, {-235, 3, 271},
		{-227, 2, 347}, {-223, 4, 253}, {-208, 3, 399}, {-200, 0, 98}, {-199, 5, 217}, {-178, 4, 202}, {-162, 4, 251},
		{-146, 3, 394}, {-138, 4, 198}, {-125, 0, 106}, {-124, 2, 318}, {-120, 4, 161}, {-108, 3, 431}, {-100, 0, 12},
		{-99, 3, 185}, {-91, 2, 456}, {-87, 4, 241}, {-75, 0, 254}, {-74, 4, 201}, {-60, 2, 374}, {-56, 1, 479},
		{-54, 2, 328}, {-50, 0, 239}, {-49, 5, 96}, {-30, 0, 80}, {-29, 2, 607}, {-25, 0, 23}, {-24, 3, 130},
		{-16, 3, 206}, {-9, 2, 346}, {-5, 2, 205}, {-1, 1, 235}, {1, 0, 94}, {2, 2, 88}, {6, 2, 274}, {10, 0, 285},
		{11, 4, 249}, {25, 0, 19}, {26, 2, 703}, {30, 0, 203}, {31, 2, 702}, {35, 4, 242}, {50, 0, 64}, {51, 3, 137},
		{57, 2, 474}, {61, 4, 173}, {75, 0, 174}, {76, 4, 142}, {92, 3, 232}, {100, 0, 21}, {101, 5, 187},
		{125, 0, 177}, {126, 5, 189}, {158, 5, 168}, {184, 4, 266}, {200, 0, 303}, {201, 5, 171}, {233, 5, 150},
		{260, 0, 214}, {261, 6, 233}, {325, 6, 136}, {389, 7, 120}, {500, 0, 925}, {501, 7, 139}, {629, 8, 122},
		{885, 8, 182}, {1141, 10, 502}, {2165, 24, 3741}, {10000001, 0, 1} });

	if (a <= 66) return ttrr1.coding(b - a, data); // 25 %
	if (a <= 200) return ttrr2.coding(b - a, data); // 25 %
	if (a <= 318) return ttrr3.coding(b - a, data); // 25 %
	return ttrr4.coding(b - a, data); // 25 %
}

bool _sable_stat::add12(const _one_stat* v1, std::vector<_one_stat>& v0, i64 izm)
{
	static const _cdf3 nnds(-12, { 2484, 4020, 2004, 688, 724, 304, 308, 148, 80, 32, 24, 10, 3, 14, 28, 36, 244, 240,
		340, 692, 944, 1492, 3508, 7092, 5044}); // -20...20

	static const _cdf3 nnse0(1, {14, 12, 26, 16, 34, 114, 104, 146, 152, 200, 136, 338, 440, 472, 344, 312, 376, 504, 466, 3});

	static const _cdf3 nnse200[21] = { _cdf3() , // исправить
		_cdf3(0, {3, 2}), // 1
		_cdf3(0, {3, 4, 6}), // 2
		_cdf3(0, {3, 12, 8, 6}), // 3
		_cdf3(0, {3, 10, 30, 22, 4}), // 4
		_cdf3(0, {3, 10, 22, 62, 46, 4}), // 5
		_cdf3(0, {3, 10, 8, 20, 60, 44, 14}), // 6
		_cdf3(0, {3, 12, 26, 24, 16, 34, 50, 14}), // 7
		_cdf3(0, {3, 10, 30, 24, 54, 48, 32, 38, 12}), // 8
		_cdf3(0, {3, 10, 30, 28, 38, 52, 118, 36, 86, 8}), // 9
		_cdf3(0, {3, 10, 30, 28, 38, 52, 118, 36, 150, 214, 8}), // 10
		_cdf3(0, {3, 10, 8, 28, 46, 52, 126, 36, 478, 350, 158, 22}), // 11
		_cdf3(0, {3, 14, 8, 28, 42, 52, 122, 36, 474, 410, 282, 346, 18}), // 12
		_cdf3(0, {3, 14, 8, 28, 42, 36, 90, 122, 180, 212, 500, 372, 148, 18}), // 13
		_cdf3(0, {3, 14, 8, 28, 42, 36, 122, 90, 212, 148, 372, 436, 308, 500, 18}), // 14
		_cdf3(0, {3, 14, 8, 20, 60, 44, 90, 122, 202, 490, 362, 298, 394, 266, 426, 18}), // 15
		_cdf3(0, {3, 14, 8, 18, 52, 36, 90, 106, 202, 506, 442, 314, 266, 890, 634, 394, 28}), // 16
		_cdf3(0, {3, 14, 8, 18, 42, 44, 90, 92, 252, 506, 442, 444, 316, 634, 826, 570, 890, 20}), // 17
		_cdf3(0, {2, 11, 9, 29, 47, 39, 95, 87, 247, 511, 447, 439, 895, 639, 575, 823, 567, 831, 21}), // 18
		_cdf3(0, {7, 14, 12, 29, 26, 24, 53, 50, 48, 69, 98, 96, 229, 194, 130, 192, 421, 293, 128, 9}), // 19
		_cdf3(0, {2, 19, 25, 63, 47, 39, 43, 33, 87, 91, 81, 183, 187, 177, 503, 375, 507, 379, 369, 497, 13}) // 20
	};

	static const _cdf3 nnsegg[21] = { _cdf3() , // исправить
		_cdf3(0, {2, 3}), // 1
		_cdf3(0, {6, 4, 3}), // 2
		_cdf3(0, {6, 12, 8, 3}), // 3
		_cdf3(0, {4, 10, 30, 22, 3}), // 4
		_cdf3(0, {5, 11, 23, 63, 47, 2}), // 5
		_cdf3(0, {5, 31, 27, 19, 55, 39, 2}), // 6
		_cdf3(0, {5, 31, 27, 55, 39, 51, 35, 2}), // 7
		_cdf3(0, {5, 31, 27, 39, 51, 35, 87, 119, 2}), // 8
		_cdf3(0, {15, 13, 19, 59, 43, 57, 49, 41, 33, 2}), // 9
		_cdf3(0, {15, 13, 19, 59, 57, 41, 33, 49, 107, 75, 2}), // 10
		_cdf3(0, {15, 13, 19, 59, 41, 49, 107, 33, 75, 121, 89, 2}), // 11
		_cdf3(0, {15, 9, 19, 59, 53, 37, 75, 107, 125, 93, 77, 109, 2}), // 12
		_cdf3(0, {15, 13, 19, 43, 41, 123, 121, 91, 89, 81, 65, 113, 97, 2}), // 13
		_cdf3(0, {15, 13, 19, 43, 41, 123, 89, 121, 113, 97, 219, 81, 155, 65, 2}), // 14
		_cdf3(0, {15, 13, 19, 59, 49, 107, 89, 105, 73, 65, 249, 203, 185, 139, 97, 2}), // 15
		_cdf3(0, {15, 13, 19, 59, 49, 107, 105, 73, 97, 139, 217, 249, 153, 185, 203, 65, 2}), // 16
		_cdf3(0, {5, 31, 27, 39, 119, 99, 151, 211, 147, 131, 343, 371, 307, 435, 499, 471, 195, 2}), // 17
		_cdf3(0, {5, 11, 63, 39, 119, 175, 215, 367, 399, 271, 1007, 751, 591, 719, 847, 975, 279, 407, 2}), // 18
		_cdf3(0, {5, 11, 63, 55, 79, 239, 199, 303, 295, 263, 943, 999, 871, 679, 935, 615, 743, 687, 391, 2}), // 19
		_cdf3(0, {5, 4, 10, 62, 54, 78, 174, 134, 486, 454, 326, 750, 614, 806, 550, 678, 934, 870, 1006, 366, 7}) // 20
	};

	i64 kk = (v1[1].value > v1[0].value) ? -1 : 1;

	if (!nnds.coding(izm, data)) return false;

	i64 n = 0;
	i64 tip = izm;
	if (izm == 0)
	{
		n = calc_series_value(v1, v0, 0);
		if (!nnse0.coding(n, data)) return false;
		for (i64 i = 0; i < n;)
		{
			i64 ser = calc_series_number(v1, v0, i, n);
			if (!nnse200[n - i].coding(ser, data)) return false;
			i += ser;
			if (i >= n) break;
			if (!delta_number(v0[i].number, v1[i].number)) return false;
			v0[i].number = v1[i].number;
			i++;
		}
	}
	if (izm < 0)
	{
		v0.erase(v0.begin(), v0.begin() - izm);
		i64 n2 = calc_series_value(v1, v0, 0);
		if (!nnsegg[std::min((i64)v0.size(), roffer)].coding(n2, data)) return false;
		if (n2 == 0)
		{
			v0.insert(v0.begin(), v1[0]);
			if (!f_delta.coding((v1[0].value - v1[1].value) * kk, data)) return false;
			if (!f_number.coding(v1[0].number, data)) return false;
			n = 1;
		}
		else
		{
			n = n2;
			for (i64 i = 0; i < n;)
			{
				i64 ser = calc_series_number(v1, v0, i, n);
				if (!nnse200[n - i].coding(ser, data)) return false;
				i += ser;
				if (i >= n) break;
				if (!delta_number(v0[i].number, v1[i].number)) return false;
				v0[i].number = v1[i].number;
				i++;
			}
			tip = 0;
		}
	}
	if (izm > 0)
	{
		v0.insert(v0.begin(), izm, {});
		for (i64 i = izm - 1; i >= 0; i--) // кодируется как c add0
		{
			v0[i] = v1[i];
			if (!f_delta.coding((v1[i].value - v1[i + 1].value) * kk, data)) return false;
			if (!f_number.coding(v1[i].number, data)) return false;
		}
		n = izm;
		i64 n2 = calc_series_value(v1, v0, n);
		if (!nnsegg[std::min((i64)v0.size(), roffer) - n].coding(n2, data)) return false;
		if (n2 == 0)
		{
			v0.erase(v0.begin() + n);
		}
		else
		{
			n2 += n;
			for (i64 i = n; i < n2;)
			{
				i64 ser = calc_series_number(v1, v0, i, n2);
				if (!nnse200[n2 - i].coding(ser, data)) return false;
				i += ser;
				if (i >= n2) break;
				if (!delta_number(v0[i].number, v1[i].number)) return false;
				v0[i].number = v1[i].number;
				i++;
			}
			n = n2;
			tip = 0;
		}
	}
	while (n < roffer)
	{
		if (n >= (i64)v0.size())
		{
			v0.resize(roffer);
			for (; n < roffer; n++)
			{
				v0[n] = v1[n];
				if (!f_delta.coding((v1[n - 1].value - v1[n].value) * kk, data)) return false;
				if (!f_number.coding(v1[n].number, data)) return false;
			}
			break;
		}
		if (tip == 0)
		{
			i64 buy_izm2 = calc_delta_del_add1(v1, v0, n);
			data.push1(buy_izm2 > 0);
			if (buy_izm2 < 0)
				v0.erase(v0.begin() + n);
			else
			{
				v0.insert(v0.begin() + n, v1[n]);
				if (!f_delta.coding((v1[n - 1].value - v1[n].value) * kk, data)) return false;
				if (!f_number.coding(v1[n].number, data)) return false;
				n++;
			}
			tip = 1;
			continue;
		}
		i64 n2 = calc_series_value(v1, v0, n);
		if (!nnsegg[std::min((i64)v0.size(), roffer) - n].coding(n2, data)) return false;
		if (n2 > 0)
		{
			n2 += n;
			for (i64 i = n; i < n2;)
			{
				i64 ser = calc_series_number(v1, v0, i, n2);
				if (!nnse200[n2 - i].coding(ser, data)) return false;
				i += ser;
				if (i >= n2) break;
				if (!delta_number(v0[i].number, v1[i].number)) return false;
				v0[i].number = v1[i].number;
				i++;
			}
			n = n2;
		}
		tip = 0;
	}
	return true;
}

bool _sable_stat::add1(const _prices2& c)
{
	i64 buy_izm = calc_delta_del_add(c.buy, base_buy);
	i64 sale_izm = calc_delta_del_add(c.sale, base_sale);
	if (std::max(abs(buy_izm), abs(sale_izm)) > 12)
	{
		data.push1(0);
		return add0(c);
	}
	data.push1(1);

	std::vector<_one_stat> bbuy = base_buy;
	std::vector<_one_stat> bsale = base_sale;

	if (!add12(c.buy, bbuy, buy_izm)) return false;
	if (!add12(c.sale, bsale, sale_izm)) return false;

	base_buy = std::move(bbuy);
	base_sale = std::move(bsale);
	return true;
}

bool _sable_stat::add(const _prices2& c)
{
//	if (c == last_cc) return false; // с большой вероятностью данные устарелые
	auto s_data = data.size();
	if (size % step_pak_cc == 0)
	{
		data.pushn(c.time, 31);
		udata.push_back(data.size());
		if (!add0(c))
		{
			udata.pop_back();
			goto err;
		}
	}
	else
	{
		time_t dt = c.time - last_cc.time;
		if (dt < 0) dt = 0; // время может идти назад!
		if (dt == 1) data.push1(0); else { data.push1(1); data.pushn(dt, 31); }
		if (dt > old_dtime)
		{
			if (!add0(c)) goto err;
		}
		else
		{
			if (!add1(c)) goto err;
		}
	}
	size++;
	last_cc = c;
	return true;
err:
	data.resize(s_data);
	return false;
}

void _sable_stat::read0()
{

}

void _sable_stat::read1()
{

}

bool _sable_stat::read(i64 n, _prices2& c)
{
	if ((n < 0) || (n >= size)) return false;
	if (n == read_n)
	{
		c = read_cc;
		return true;
	}
	if (n == size - 1)
	{
		c = last_cc;
		return true;
	}
	if (read_n + 1 != n)
	{
		i64 k = n / step_pak_cc;
		if ((read_n > n) || (read_n <= k * step_pak_cc - 1))
		{
			adata = udata[k];
			read_n = k * step_pak_cc - 1;
		}
		bool r;
		while (read_n < n) r = read(read_n + 1, c);
		return r;
	}
	if (n % step_pak_cc == 0)
		read0();
	else
		read1();
	read_cc = c;
	read_n = n;
	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool _sable_graph::mouse_down_left2(_xy r)
{
	x_tani = (i64)r.x;
	return true;
}

void _sable_graph::mouse_move_left2(_xy r)
{
	i64 dx = ((i64)r.x - x_tani) / size_el;
	if (dx == 0) return;
	x_tani += dx * size_el;

	_g_scrollbar* polz = find1<_g_scrollbar>(flag_part); 
	if (!polz) return;
	double* ii = &polz->position;
	if (!ii) return;
	*ii -= double(dx) / v_vib;
	if (*ii < 0) *ii = 0;
	if (*ii > 1) *ii = 1;
	polz->cha_area();
	polz->run(0, polz, flag_run);
}

void _sable_graph::run(_tetron* tt0, _tetron* tt, u64 flags)
{
	cha_area();
}

_sable_graph::_sable_graph()
{
	graph = this;
	local_area = { {0, 200}, {0, 100} };
}

void os_pordis(double min, double max, i64 maxN, double& mi, double& step, double min_step = 0.0)
{
	i64 n;
	double step2;
	if (maxN < 2) maxN = 2;
	step = exp(round(log((max - min) / maxN) / log(10)) * log(10));
	auto fun = [&]()
	{
		mi = (i64(min / step)) * step;
		if (mi < min) mi += step;
		n = (i64((max - mi) / step)) + 1;
	};
	do
	{
		fun();
		if (n < maxN) step = step * 0.1; else break;
	} while (true);
	while (n > maxN)
	{
		step = step * 10;
		fun();
	}
	step2 = step;
	step = step2 * 0.2;
	fun();
	if (n <= maxN) goto end;
	step = step2 * 0.5;
	fun();
	if (n <= maxN) goto end;
	step = step2;
	fun();
end:
	if (step < min_step)
	{
		step = min_step;
		fun();
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

void _sable_graph::ris2(_trans tr, bool final)
{
	_area a = tr(local_area);
	_interval y_; // диапазон у (grid)
	static std::vector<int> time_; // отсчеты времени (grid)

	double polzi_ = 0; // !! ползунок

	_g_scrollbar* sb = find1<_g_scrollbar>(flag_part);
	if (sb)	polzi_ = sb->position;

	int ll = (int)curve.size();
	if (ll == 0) return;

	i64 k_el = local_area.x.length() / size_el;
	if (k_el < 1) return;
	double r_el = a.x.length() / k_el;

	int n = curve[0]->get_n();
	if (n == 0) return;
	//	v_vib_ = n - k_el;
	v_vib = n - 1;
	if (v_vib < 0) v_vib = 0;
	int vib = (int)(polzi_ * v_vib + 0.5); // !! ползунок

	int period = 60;
	//	int pause_max = 3;
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
				curve[i]->get_n_info(al[i].n + 1i64, &al[i]);
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
				double ymi = a.y.max - (al[i].min - zmin) * a.y.length() / (zmax - zmin);
				double yma = a.y.max - (al[i].max - zmin) * a.y.length() / (zmax - zmin);
				double x = r_el * (ke - 1i64) + a.x.min;
				curve[i]->draw(al[i].n, { {x, x + r_el}, {yma, ymi} });
				curve[i]->get_n_info(al[i].n + 1i64, &al[i]);
			}
		}
	}
	delete[] al;
	// рисование сетки
	uint col_setka      = c_max - 0xE0000000; // цвет сетки
	uint col_setka_font = c_def;              // цвет подписи сетки
	if (time_.size() < 1)
	{
		master_bm.line({ a.x.min, a.y.min }, { a.x.max, a.y.max }, 0xFF800000);
		master_bm.line({ a.x.min, a.y.max }, { a.x.max, a.y.min }, 0xFF800000);
		return;
	}
	// рисование горизонтальных линий сетки с подписями
	i64 dex = 33; // длина подписи
	i64 maxN = a.y.length() / 15;
	if (maxN > 1)
	{
		double mi, step;
		os_pordis(y_.min, y_.max, maxN, mi, step, ss.c_unpak);
		for (double y = mi; y < y_.max; y += step)
		{
			double yy = a.y.max - (y - y_.min) * a.y.length() / (y_.max - y_.min);
			master_bm.line({ a.x.min + dex, yy }, { a.x.max - dex, yy }, col_setka);
			master_bm.text16(std::max(a.x.min, 0.0) + 2, (i64)(yy - 6), double_to_astring(y, 2), col_setka_font);
			master_bm.text16(std::min((i64)a.x.max, master_bm.size.x) - dex, (i64)(yy - 6), double_to_astring(y, 2),
				col_setka_font);
		}
	}
	// рисование вертикальных линий сетки с подписями
	static int g_delta_time[] = {
		1, 2, 3, 5, 10, 15, 20, 30,                    // секунды
		60, 120, 180, 300, 600, 900, 1200, 1800,       // минуты
		3600, 7200, 10800, 14400, 21600, 28800, 43200, // часы
		86400, 172800, 345600, 691200, 1382400,        // дни
		2764800, 5529600, 8294400, 11059200, 16588800, // месяца
		33177600 };                                    // год

	double rel = r_el;
	dex = 26;
	int stept = (((int)(dex / rel)) + 1) * period;
	int ks = sizeof(g_delta_time) / sizeof(g_delta_time[0]);
	for (int i = 0; i < ks; i++)
		if (g_delta_time[i] >= stept)
		{
			stept = g_delta_time[i];
			break;
		}
	int dele[] = { 1, 60, 3600, 86400, 2764800, 33177600 };
	int ost[]  = { 60, 60, 24, 32, 12, 1000 };
	int ido = 0;
	if (stept % 33177600) ido = 4;
	if (stept %  2764800) ido = 3;
	if (stept %    86400) ido = 2;
	if (stept %     3600) ido = 1;
	if (stept %       60) ido = 0;
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
			if ((x <= dex) || (x >= a.x.length() - dex)) continue;
			uint cl = (sca) ? (0x80FF0000) : col_setka;
			master_bm.line({ i64(a.x.min + x), a.y.min }, { a.x.min + x, a.y.max }, cl);
			if ((x - 13 <= dex) || (x + 13 >= a.x.length() - dex)) continue;
			int ii = (time_[i] / dele[ido]) % ost[ido];
			s[4] = '0' + (ii % 10);
			s[3] = '0' + (ii / 10);
			ii = (time_[i] / dele[ido + 1]) % ost[ido + 1];
			s[1] = '0' + (ii % 10);
			s[0] = '0' + (ii / 10);
			master_bm.text16(x - 11 + a.x.min, std::min((i64)a.y.max, master_bm.size.y) - 13, s, col_setka_font);
			master_bm.text16(x - 11 + a.x.min, std::max(a.y.min, 0.0), s, col_setka_font);
			continue;
		}
		if (sca)
		{
			double x = rel * i;
			if ((x <= dex) || (x >= a.x.length() - dex)) continue;
			master_bm.line({ a.x.min + x, a.y.min }, { a.x.min + x, a.y.max }, 0x80FF0000);
		}
	}
	// рисование даты
	master_bm.text16n(std::max(a.x.min, 0.0) + dex + 10, std::max(a.y.min, 0.0) + 10,
		date_to_ansi_string(mintime).data(), 4, c_max - 0x80000000);
	// рисование количества элементов
	master_bm.text16n(std::max(a.x.min, 0.0) + dex + 10, std::max(a.y.min, 0.0) + 60,
		std::to_string(ss.size).data(), 2, 0x60ff0000);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void _view_stat::get_n_info(i64 n, _element_chart* e)
{
	if (n >= (i64)cen1m.size())
	{
		e->n = -1;
		return;
	}
	e->n = n;
	e->time = cen1m[n].time;
	e->min = cen1m[n].min * ss.c_unpak;
	e->max = cen1m[n].max * ss.c_unpak;
}

void _view_stat::get_t_info(int t, _element_chart* e)
{
	auto x = lower_bound(cen1m.begin(), cen1m.end(), t);
	if (x == cen1m.end())
	{
		e->n = -1;
		return;
	}
	i64 xx = (x - cen1m.begin());
	e->n = xx;
	e->time = cen1m[xx].time;
	e->min = cen1m[xx].min * ss.c_unpak;
	e->max = cen1m[xx].max * ss.c_unpak;
}

void _view_stat::draw(i64 n, _area area)
{
	double r = cen1m[n].k * area.x.length() * 0.15;
	uint c = 0x80ff0000;
	master_bm.fill_ring(area.center(), r, r * 0.1, c, c);
}

void _view_stat::recovery()
{
	i64 vcc = 0;
	if (cen1m.size()) vcc = cen1m.back().ncc.max;
	i64 ssvcc = ss.size;
	if (ssvcc == vcc) return; // ничего не изменилось
	if (vcc < ssvcc) // добавились несколько цен
	{
		_prices cc;
		int t = 0;
		_cen_pak* cp = 0;
		if (cen1m.size())
		{
			cp = &cen1m.back();
			t = cp->time;
		}
		for (i64 i = vcc; i < ssvcc; i++)
		{
			ss.read(i, cc);
			int t2 = cc.time.to_minute();
			if (t2 != t)
			{
				t = t2;
				_cen_pak we;
				we.time = t;
				we.ncc.min = i;
				we.ncc.max = i + 1;
				we.min = ((int)cc.pok[0].c + (int)cc.pro[0].c) / 2;
				we.max = we.min;
				we.k = (cc.pro[1].k == 570);
				cen1m.push_back(we);
				cp = &cen1m.back();
			}
			else
			{
				if (cp == 0) continue; // для паранойи компилятора
				int aa = ((int)cc.pok[0].c + (int)cc.pro[0].c) / 2;
				if (aa < cp->min) cp->min = aa;
				if (aa > cp->max) cp->max = aa;
				if (cc.pro[1].k == 570) cp->k++;
				cp->ncc.max++;
			}
		}
		return;
	}
	_prices cc; // уменьшились цены, полный пересчет
	cen1m.clear();
	int t = 0;
	_cen_pak* cp = 0;
	for (i64 i = 0; i < ssvcc; i++)
	{
		ss.read(i, cc);
		int t2 = cc.time.to_minute();
		if (t2 != t)
		{
			t = t2;
			_cen_pak we;
			we.time = t;
			we.ncc.min = i;
			we.ncc.max = i + 1;
			we.min = ((int)cc.pok[0].c + (int)cc.pro[0].c) / 2;
			we.max = we.min;
			we.k = (cc.pro[1].k == 570);
			cen1m.push_back(we);
			cp = &cen1m.back();
		}
		else
		{
			if (cp == 0) continue; // для паранойи компилятора
			int aa = ((int)cc.pok[0].c + (int)cc.pro[0].c) / 2;
			if (aa < cp->min) cp->min = aa;
			if (aa > cp->max) cp->max = aa;
			if (cc.pro[1].k == 570) cp->k++;
			cp->ncc.max++;
		}
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void _mctds_candle::get_n_info(i64 n, _element_chart* e)
{
	if (n >= (i64)cen1m.size())
	{
		e->n = -1;
		return;
	}
	e->n = n;
	e->time = cen1m[n].time;
	e->min = cen1m[n].min * ss.c_unpak;
	e->max = cen1m[n].max * ss.c_unpak;
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

void _mctds_candle::draw(i64 n, _area area)
{
	double min_   = cen1m[n].min   * ss.c_unpak;
	double max_   = cen1m[n].max   * ss.c_unpak;
	double first_ = cen1m[n].first * ss.c_unpak;
	double last_  = cen1m[n].last  * ss.c_unpak;

	_iinterval xx = area.x;
	xx.min++;
	xx.max--;
	if (xx.empty()) return;

	constexpr uint col_rost = 0xFF28A050; // цвет ростущей свечки
	constexpr uint col_pade = 0xFF186030; // цвет падающей свечки
	double yfi, yla;
	if (min_ < max_)
	{
		yfi = area.y.max - area.y.length() * (first_ - min_) / (max_ - min_);
		yla = area.y.max - area.y.length() * (last_ - min_) / (max_ - min_);
	}
	else
	{
		yfi = yla = area.y.min;
	}
	if (first_ <= last_)
	{
		master_bm.fill_rectangle({ xx, {yla, yfi} }, col_rost);
		master_bm.line({ xx.center(), area.y.max }, { xx.center(), area.y.min }, col_rost);
	}
	else
	{
		master_bm.fill_rectangle({ xx, {yfi, yla} }, col_pade);
		master_bm.line({ xx.center(), area.y.max }, { xx.center(), area.y.min }, col_pade);
	}
}

void _mctds_candle::recovery()
{
	i64 vcc = 0;
	if (cen1m.size()) vcc = cen1m.back().ncc.max;
	i64 ssvcc = ss.size;
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
		for (i64 i = vcc; i < ssvcc; i++)
		{
			ss.read(i, cc);
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
	for (i64 i = 0; i < ssvcc; i++)
	{
		ss.read(i, cc);
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

i64 _nervous_oracle::prediction()
{
	if (zn.size() < 10) return 0;
	if (zn.back().time + 60 != ss.last_cc.time.to_minute()) return 0;
	return get_latest_events(zn.size() - 1).start();
}

void _nervous_oracle::push(_stack* mem)
{
	*mem << zn;
}

void _nervous_oracle::pop(_stack* mem)
{
	*mem >> zn;
}

void _nervous_oracle::get_n_info(i64 n, _element_chart* e)
{
	if (n >= (i64)zn.size())
	{
		e->n = -1;
		return;
	}
	e->n = n;
	e->time = zn[n].time;
	e->min = e->max = ((double)zn[n].max_pro + (double)zn[n].min_pok) * 0.5 * ss.c_unpak;
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
	e->min = e->max = ((double)zn[xx].max_pro + (double)zn[xx].min_pok) * 0.5 * ss.c_unpak;
}

_latest_events _nervous_oracle::get_latest_events(i64 nn)
{
	const i64 k = 4;
	_latest_events e;
	e.event[0] = e.event[1] = e.event[2] = e.event[3] = 0;
	e.minute[0] = e.minute[1] = e.minute[2] = e.minute[3] = 0;
	e.x[0] = e.x[1] = e.x[2] = e.x[3] = 0.0;
	if (nn < 10) return e;
	i64 ii = std::max(k, nn - 40);
	int ee = 0;
	for (i64 n = nn; n >= ii; n--)
	{
		if ((i64)zn[n].time - zn[n - k].time != k * 60) continue;
		bool rost_pro = true;
		bool rost_pok = true;
		bool pade_pro = true;
		bool pade_pok = true;
		for (i64 i = n - k; i < n; i++)
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
		e.x[ee] = ((double)zn[n].max_pro + (double)zn[n].min_pok) * 0.5 * ss.c_unpak;
		ee++;
		if (ee == 4) break;
	}
	return e;
}

void _nervous_oracle::draw(i64 n, _area area)
{
	const i64 k = 4;
	if (n < k) return;
	if ((i64)zn[n].time - zn[n - k].time != k * 60) return;
	bool rost_pro = true;
	bool rost_pok = true;
	bool pade_pro = true;
	bool pade_pok = true;
	for (i64 i = n - k; i < n; i++)
	{
		if (zn[i].r_pro >= zn[i + 1].r_pro) rost_pro = false;
		if (zn[i].r_pok >= zn[i + 1].r_pok) rost_pok = false;
		if (zn[i].r_pro <= zn[i + 1].r_pro) pade_pro = false;
		if (zn[i].r_pok <= zn[i + 1].r_pok) pade_pok = false;
	}
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

	master_bm.fill_ring(area.center(), r, r * 0.1, c, c);
}

void _nervous_oracle::recovery()
{
	i64 vcc = 0;
	if (zn.size()) vcc = zn.back().ncc.max;
	i64 ssvcc = ss.size;
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
		for (i64 i = vcc; i < ssvcc; i++)
		{
			ss.read(i, cc, &inf);
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
	for (i64 i = 0; i < ssvcc; i++)
	{
		ss.read(i, cc, &inf);
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

i64 _nervous_oracle2::prediction()
{
	// return i64(rnd(15000) == 13) * 60; // случайный
	if (zn.size() < 10) return 0;
	if (zn.back().time + 60 != ss.last_cc.time.to_minute()) return 0;
	return get_latest_events(zn.size() - 1).start();
}

void _nervous_oracle2::push(_stack* mem)
{
	*mem << zn;
}

void _nervous_oracle2::pop(_stack* mem)
{
	*mem >> zn;
}

void _nervous_oracle2::get_n_info(i64 n, _element_chart* e)
{
	if (n >= (i64)zn.size())
	{
		e->n = -1;
		return;
	}
	e->n = n;
	e->time = zn[n].time;
	e->min = e->max = ((double)zn[n].max_pro + (double)zn[n].min_pok) * 0.5 * ss.c_unpak;
}

void _nervous_oracle2::get_t_info(int t, _element_chart* e)
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
	e->min = e->max = ((double)zn[xx].max_pro + (double)zn[xx].min_pok) * 0.5 * ss.c_unpak;
}

_latest_events _nervous_oracle2::get_latest_events(i64 nn)
{
	const i64 k = 4;
	_latest_events e;
	e.event[0] = e.event[1] = e.event[2] = e.event[3] = 0;
	e.minute[0] = e.minute[1] = e.minute[2] = e.minute[3] = 0;
	e.x[0] = e.x[1] = e.x[2] = e.x[3] = 0.0;
	if (nn < 10) return e;
	i64 ii = std::max(k, nn - 40);
	int ee = 0;
	for (i64 n = nn; n >= ii; n--)
	{
		if ((i64)zn[n].time - zn[n - k].time != k * 60) continue;
		bool rost_pro = true;
		bool rost_pok = true;
		bool pade_pro = true;
		bool pade_pok = true;
		for (i64 i = n - k; i < n; i++)
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
		e.x[ee] = ((double)zn[n].max_pro + (double)zn[n].min_pok) * 0.5 * ss.c_unpak;
		ee++;
		if (ee == 4) break;
	}
	return e;
}

void _nervous_oracle2::draw(i64 n, _area area)
{
	const i64 k = 4;
	if (n < k) return;
	if ((i64)zn[n].time - zn[n - k].time != k * 60) return;
	bool rost_pro = true;
	bool rost_pok = true;
	bool pade_pro = true;
	bool pade_pok = true;
	for (i64 i = n - k; i < n; i++)
	{
		if (zn[i].r_pro >= zn[i + 1].r_pro) rost_pro = false;
		if (zn[i].r_pok >= zn[i + 1].r_pok) rost_pok = false;
		if (zn[i].r_pro <= zn[i + 1].r_pro) pade_pro = false;
		if (zn[i].r_pok <= zn[i + 1].r_pok) pade_pok = false;
	}
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

	master_bm.fill_ring(area.center(), r, r * 0.1, c, c);
}

void _nervous_oracle2::recovery()
{
	i64 vcc = 0;
	if (zn.size()) vcc = zn.back().ncc.max;
	i64 ssvcc = ss.size;
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
		for (i64 i = vcc; i < ssvcc; i++)
		{
			ss.read(i, cc, &inf);
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
	for (i64 i = 0; i < ssvcc; i++)
	{
		ss.read(i, cc, &inf);
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

void _oracle3::get_n_info(i64 n, _element_chart* e)
{
	if (n >= (i64)zn.size())
	{
		e->n = -1;
		return;
	}
	e->n = n;
	e->time = zn[n].time;
	e->min = (zn[n].min - 1) * ss.c_unpak;
	e->max = zn[n].max * ss.c_unpak;
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
	e->min = (zn[xx].min - 1) * ss.c_unpak;
	e->max = zn[xx].max * ss.c_unpak;
}

void _oracle3::recovery()
{
	i64 vcc = 0;
	if (zn.size()) vcc = zn.back().ncc.max;
	i64 ssvcc = ss.size;
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
		for (i64 i = vcc; i < ssvcc; i++)
		{
			ss.read(i, cc);
			int t2 = cc.time.to_minute();
			if (t2 != t)
			{
				t = t2;
				_element_oracle we;
				we.time = t;
				we.ncc.min = i;
				we.ncc.max = i + 1;
				we.max = cc.pro[roffer - 1].c;
				we.min = cc.pok[roffer - 1].c;
				zn.push_back(we);
				cp = &zn.back();
			}
			else
			{
				if (cp == 0) continue; // для паранойи компилятора
				if (cc.pok[roffer - 1].c < cp->min) cp->min = cc.pok[roffer - 1].c;
				if (cc.pro[roffer - 1].c > cp->max) cp->max = cc.pro[roffer - 1].c;
				cp->ncc.max++;
			}
		}
		return;
	}
	_prices cc; // уменьшились цены, полный пересчет
	zn.clear();
	int t = 0;
	_element_oracle* cp = 0;
	for (i64 i = 0; i < ssvcc; i++)
	{
		ss.read(i, cc);
		int t2 = cc.time.to_minute();
		if (t2 != t)
		{
			t = t2;
			_element_oracle we;
			we.time = t;
			we.ncc.min = i;
			we.ncc.max = i + 1;
			we.max = cc.pro[roffer - 1].c;
			we.min = cc.pok[roffer - 1].c;
			zn.push_back(we);
			cp = &zn.back();
		}
		else
		{
			if (cp == 0) continue; // для паранойи компилятора
			if (cc.pok[roffer - 1].c < cp->min) cp->min = cc.pok[roffer - 1].c;
			if (cc.pro[roffer - 1].c > cp->max) cp->max = cc.pro[roffer - 1].c;
			cp->ncc.max++;
		}
	}
}

void _oracle3::draw(i64 n, _area area)
{
	static _prices pri[61]; // цены
	static i64 min, max; // разброс по y
	min = 0;
	max = 1;
	for (auto& i : pri) i.clear();

	for (i64 i = zn[n].ncc.min; i < zn[n].ncc.max; i++)
	{
		if (i < begin_ss)
		{
			i64 delta = begin_ss - i;
			if (delta >= max_part)
				part_ss.clear();
			else
			{
				_prices w;
				w.clear();
				for (int i_ = 0; i_ < delta; i_++)
				{
					part_ss.push_front(w);
					if (part_ss.size() > max_part) part_ss.pop_back();
				}
			}
			begin_ss = i;
		}
		if (i >= begin_ss + (i64)part_ss.size())
		{
			_prices w;
			w.clear();
			i64 delta = i - (begin_ss + (int)part_ss.size()) + 1;
			if (delta >= max_part)
			{
				part_ss.clear();
				part_ss.push_back(w);
				begin_ss = i;
			}
			else
				for (int i_ = 0; i_ < delta; i_++)
				{
					part_ss.push_back(w);
					if (part_ss.size() > max_part)
					{
						part_ss.pop_front();
						begin_ss++;
					}
				}
		}
		i64 ii = i - begin_ss;
		if (part_ss[ii].empty()) ss.read(i, part_ss[ii]);
		pri[part_ss[ii].time.second] = part_ss[ii];
		min = zn[n].min - 1;
		max = zn[n].max;
	}
	_iinterval xx = area.x;
	xx.min++;
	i64 dx = xx.size();
	if (dx < 2) return;
	i64 step = 60;
	if (dx >=   4) step = 30;
	if (dx >=   6) step = 20;
	if (dx >=   8) step = 15;
	if (dx >=  10) step = 12;
	if (dx >=  12) step = 10;
	if (dx >=  20) step = 6;
	if (dx >=  24) step = 5;
	if (dx >=  30) step = 4;
	if (dx >=  40) step = 3;
	if (dx >=  60) step = 2;
	if (dx >= 120) step = 1;
	i64 kol = 60 / step;
	i64 dd = max - min;
	double ddy = area.y.max - area.y.min;
	for (i64 i = 0; i < kol; i++)
	{
		i64 ss_ = i * step;
		while (pri[ss_].empty())
		{
			if (ss_ + 1 >= (i + 1) * step) break;
			ss_++;
		}
		if (pri[ss_].empty()) continue;
		i64 xx1 = xx.min + dx * i / kol;
		i64 xx2 = xx.min + dx * (i + 1) / kol - 1;
		for (int j = roffer - 1; j >= 0; j--)
		{
			i64 ce = pri[ss_].pro[j].c;
			_iinterval yy(area.y.min + (max - ce) * ddy / dd, area.y.min + (max - ce + 1) * ddy / dd);
			yy.min++;
			yy.max--;
			if (yy.empty()) continue;
			uint q = (uint)sqrt(pri[ss_].pro[j].k) + 32;
			if (q > 255) q = 255;
			uint cc = (q << 8) + (q << 16) + 0x60000000;
			if ((pri[ss_].pro[j].k == 250)) cc = 0xffff00ff;
			if ((pri[ss_].pro[j].k == 250) && (j == 0)) cc = 0xffffffff;
			master_bm.fill_rectangle({ {xx1, xx2}, yy}, cc);
		}
		for (int j = 0; j < roffer; j++)
		{
			i64 ce = pri[ss_].pok[j].c;
			_iinterval yy(area.y.min + (max - ce) * ddy / dd, area.y.min + (max - ce + 1) * ddy / dd);
			yy.min++;
			yy.max--;
			if (yy.empty()) continue;
			uint q = (uint)sqrt(pri[ss_].pok[j].k) + 32;
			if (q > 255) q = 255;
			uint cc = q + (q << 8) + 0x60000000;
			master_bm.fill_rectangle({ {xx1, xx2}, yy }, cc);
		}
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

_recognize::_recognize()
{
	static const int vf_ = 2; // количество шрифтов
	_bitmap bm_[vf_]; // холст для рисования
	bm_[0].set_font(L"MS Sans Serif", false);
	bm_[1].set_font(L"MS Sans Serif", true);
	std::wstring nabor = L"0123456789.,:;-()[]><=абвгдежзийклмнопрстуфхцчшщъыьэюяАБВГДЕЖЗИЙКЛМНОПРСТУФХЦЧШЩЪЫЬЭЮЯ"
		L"abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
	int L = (int)nabor.size();
	std::wstring ss_ = L"0";
	ushort aa[20];
	for (int nf = 0; nf < vf_; nf++)
		for (int i = 0; i < L; i++)
		{
			wchar_t c = nabor[i];
			ss_[0] = c;
			_isize size = bm_[nf].size_text(ss_.data(), 8);
			//			if (c == ' ') ShowMessage(IntToStr((int)size.cx));
			//			if (size.cy != 13) ShowMessage(ss_+" "+IntToStr((int)size.cy));
			if (size.x > 20) return;//что-то не то...
			if ((size.x > bm_[nf].size.x) || (size.y > bm_[nf].size.y))
				bm_[nf].resize({ std::max(size.x, bm_[nf].size.x), std::max(size.y, bm_[nf].size.y) });
			bm_[nf].clear(0);
			bm_[nf].text(0, 0, ss_.data(), 8, 0xffffff, 0);
			ZeroMemory(aa, sizeof(ushort) * size.x);
			for (i64 j = size.y - 1; j >= 0; j--)
			{
				uint* sl = bm_[nf].sl(j);
				for (int i_ = 0; i_ < size.x; i_++) aa[i_] = (aa[i_] << 1) + (sl[i_] > 0);
			}
			int na = 0;
			int ko = (int)size.x - 1;
			while ((aa[na] == 0) && (na < ko)) na++;
			while ((aa[ko] == 0) && (ko > na)) ko--;
			i64 nbit = 0;
			for (int j = na; j <= ko; j++) nbit += bit16(aa[j]);
			bu.cod(aa + na, ko - na + 1, c, nf, nbit);
		}
}

int _recognize::read_okno_soobsenii()
{
	HWND w = FindWindow(0, mmm2.c_str());
	if (!w) return 1;
	offset = { 0, 0 };
	ClientToScreen(w, &offset);
	image.clear(0xFFFFFFFF); // т.к. если окно свернуто, то не грабится
	image.grab_ecran_oo2(w);
	find_text13(0xFF000000);
	return 0;
}

int _recognize::read_vnimanie_pokupka()
{
	HWND w = FindWindow(0, L"Внимание");
	if (!w) return 1;
	offset = { 0, 0 };
	ClientToScreen(w, &offset);
	image.clear(0xFFFFFFFF); // т.к. если окно свернуто, то не грабится
	image.grab_ecran_oo2(w);
	find_text13(0xFF000000);
	if ((elem.size() < 17) || (elem.size() > 18)) return 2; // неправильное количество элементов
	if (find_elem(L"Выдействителы-ожелаетевыполнитьтранзакцию") < 0) return 3;
	if (find_elem(L"ЛимитированнаяПокупка") < 0) return 4;
	return 0;
}

int _recognize::read_vnimanie_prodaza()
{
	HWND w = FindWindow(0, L"Внимание");
	if (!w) return 1;
	offset = { 0, 0 };
	ClientToScreen(w, &offset);
	image.clear(0xFFFFFFFF); // т.к. если окно свернуто, то не грабится
	image.grab_ecran_oo2(w);
	find_text13(0xFF000000);
	if ((elem.size() < 17) || (elem.size() > 18)) return 2; // неправильное количество элементов
	if (find_elem(L"Выдействителы-ожелаетевыполнитьтранзакцию") < 0) return 3;
	if (find_elem(L"ЛимитированнаяПродажа") < 0) return 4;
	return 0;
}

int _recognize::read_vvod_zaya()
{
	HWND w = FindWindow(0, L"МБ ФР: Т+ Акции и ДР Ввод заявки");
	if (!w) return 1;
	offset = { 0, 0 };
	ClientToScreen(w, &offset);
	image.clear(0xFFFFFFFF); // т.к. если окно свернуто, то не грабится
	image.grab_ecran_oo2(w);
	find_text13(0xFF000000);
	if ((elem.size() < 20) || (elem.size() > 28)) return 2; // неправильное количество элементов
	return 0;
}

struct FindWnd
{
	std::wstring class_name_; // имя класса
	std::wstring window_name_; // имя окна
	HWND hwnd_{}; // указатель окна
};

BOOL CALLBACK PoiskOkna(HWND hwnd, LPARAM lParam)
{
	FindWnd* aa = (FindWnd*)lParam;
	wchar_t str[255];
	GetWindowText(hwnd, str, 255);
	if (aa->window_name_ != str) return TRUE;
	GetClassName(hwnd, str, 255);
	if (aa->class_name_ != str) return TRUE;
	aa->hwnd_ = hwnd;
	return FALSE;


	//FindWnd* aa = (FindWnd*)lParam;
	//wchar_t str[255];
	//wstring s;
	//GetWindowText(hwnd, str, 255);
	//if (aa->window_name_ != str) return TRUE;
	//s = str;
	//GetClassName(hwnd, str, 255);
	//s = s + L" : " + str;
	//MessageBox(0, s.c_str(), L"упс", MB_OK | MB_TASKMODAL);

	//if (aa->class_name_ != str) return TRUE;
	//aa->hwnd_ = hwnd;
	//return FALSE;

}

HWND FindSubWindow(HWND w, const wchar_t* classname, const wchar_t* windowname)
{
	//		HWND w2 = FindWindowEx(w, 0, L"HostWindow", 0);
	FindWnd aa;
	aa.class_name_ = classname;
	aa.window_name_ = windowname;
	aa.hwnd_ = 0;
	//	EnumWindows(fnEnumWindowProc, 0);
	EnumChildWindows(w, PoiskOkna, (LPARAM)& aa);
	return aa.hwnd_;
}

int _recognize::read_tablica_zayavok(int a, int& b)
{
	b = 0;
	HWND w = FindWindow(0, mmm3.c_str());
	if (!w) return 1;
	HWND w2 = FindSubWindow(w, L"InfoMDITableCommon", L"Таблица заявок Основной рынок"); // InfoPriceTable HostWindow
	if (!w2) return 2;
	RECT rr;
	GetWindowRect(w2, &rr);

	image.clear(0xFFFFFFFF); // т.к. если окно свернуто, то не грабится
	image.grab_ecran_oo2(w2);
	//	image_.SaveToFile(L"err.bmp");
	find_text13(0xFF0040FF, 10); // синим цветом 

	std::wstring ss_ = std::to_wstring(a);
	for (uint i = 0; i < elem.size(); i++)
		if (elem[i].s == ss_) b++;
	return 0;
}

bool _recognize::find_window_prices(RECT* rr)
{
	HWND w = FindWindow(0, mmm3.c_str());
	if (!w) return false;
	HWND w2 = FindSubWindow(w, L"InfoPriceTable", L"Сбербанк [МБ ФР: Т+ Акции и ДР] Котировки"); // InfoPriceTable HostWindow
	if (!w2) return false;
	GetWindowRect(w2, rr);
	return true;
}

i64 to_int(const std::wstring& s) // преобразование в число с игнорированием нечисловых символов
{
	i64 r = 0;
	bool znak = false;
	int l = (int)s.size();
	const wchar_t* data = s.data();
	for (int i = 0; i < l; i++)
	{
		ushort delta = data[i] - L'0';
		if (delta <= 9) r = r * 10 + delta;
		if (data[i] == L'-') znak = !znak;
	}
	if (znak) r = -r;
	return r;
}

int _recognize::test_image(_prices* pr)
{
	find_text13(0xFF0000FF); // синим цветом покупки
	if (elem.size() != roffer * 2) return 3;
	i64 pre = 0;
	for (int i = 0; i < roffer; i++)
	{
		i64 a = to_int(elem[i * 2i64].s);
		if (a <= pre) return 4;
		pre = a;
		if ((a < 1) || (a > 65000)) return 5;
		pr->pok[roffer - 1 - i].c = static_cast<ushort>(a);
		a = to_int(elem[i * 2i64 + 1i64].s);
		if ((a < 1) || (a > 2000000000)) return 6;
		pr->pok[roffer - 1 - i].k = static_cast<int>(a);
	}
	find_red_text13(24); // красным цветом продажи
	if (elem.size() != roffer * 2) return 7;
	for (int i = 0; i < roffer; i++)
	{
		i64 a = to_int(elem[i * 2i64].s);
		if (a <= pre) return 8;
		pre = a;
		if ((a < 1) || (a > 65000)) return 9;
		pr->pro[i].c = static_cast<ushort>(a);
		a = to_int(elem[i * 2i64 + 1i64].s);
		if ((a < 1) || (a > 2000000000)) return 10;
		pr->pro[i].k = static_cast<int>(a);
	}
	return 0;
}

int _recognize::read_prices_from_screen(_prices* pr)
{
	HWND w = FindWindow(0, mmm3.c_str());
	if (!w) return 1;
	HWND w2 = FindSubWindow(w, L"InfoPriceTable", L"Сбербанк [МБ ФР: Т+ Акции и ДР] Котировки"); // InfoPriceTable HostWindow
	if (!w2) return 2;
	image.clear(0xFFFFFFFF); // т.к. если окно свернуто, то не грабится
	pr->time.now();
	image.grab_ecran_oo2(w2);
	find_text13(0xFF0000FF); // синим цветом покупки
	if (elem.size() != roffer * 2) return 3;
	i64 pre = 0;
	for (i64 i = 0; i < roffer; i++)
	{
		std::wstring swe = elem[i * 2].s;
		i64 a = to_int(swe);
		if (a <= pre) return 4;
		pre = a;
		if ((a < 1) || (a > 65000)) return 5;
		pr->pok[roffer - 1 - i].c = static_cast<ushort>(a);
		a = to_int(elem[i * 2 + 1].s);
		if ((a < 1) || (a > 2000000000)) return 6;
		pr->pok[roffer - 1 - i].k = static_cast<int>(a);
	}
	find_red_text13(24); // красным цветом продажи
	if (elem.size() != roffer * 2) return 7;
	for (i64 i = 0; i < roffer; i++)
	{
		i64 a = to_int(elem[i * 2].s);
		if (a <= pre) return 8;
		pre = a;
		if ((a < 1) || (a > 65000)) return 9;
		pr->pro[i].c = static_cast<ushort>(a);
		a = to_int(elem[i * 2 + 1].s);
		if ((a < 1) || (a > 2000000000)) return 10;
		pr->pro[i].k = static_cast<int>(a);
	}
	return 0;
}

void _recognize::find_red_text13(uint err)
{
	elem.clear();
	i64 rx = image.size.x;
	ushort* lin = new ushort[rx];
	ZeroMemory(lin, sizeof(ushort) * rx);
	for (i64 j = image.size.y - 1; j >= 0; j--)
	{
		uint* sl = image.sl(j);
		i64 first = -1;
		i64 last = -100;
		bool norm = true;
		for (int i = 0; i < rx; i++) {
			uint cc = sl[i];
			uint e2 = (255 - ((cc >> 16) & 255)) + ((cc >> 8) & 255) + (cc & 255);
			lin[i] = (lin[i] << 1) + (e2 <= err);
			if (lin[i])
			{
				if (first < 0)
				{
					first = i;
					norm = false;
				}
				last = i;
				if (lin[i] & 1) norm = true;
				continue;
			}
			if (i - last == 8)
			{ // КРИТИЧЕСКАЯ РАЗНИЦА!!!!
				if (!norm)
				{
					_area_string aa;
					ushort s = 0;
					for (i64 k = first; k <= last; k++) s |= lin[k];
					aa.area = { {first, last + 1}, {j + 1, j + bit16(s) + 1} };
					aa.s = rasp_text(lin + first, last - first + 1);
					elem.push_back(aa);
					ZeroMemory(lin + first, sizeof(ushort) * (last + 1i64 - first));
				}
				first = -1;
			}
		}
	}
	delete[] lin;
}

void _recognize::find_text13(uint c, int err)
{
	int c0 = c & 255;
	int c1 = (c >> 8) & 255;
	int c2 = (c >> 16) & 255;

	elem.clear();
	i64 rx = image.size.x;
	ushort* lin = new ushort[rx];
	ZeroMemory(lin, sizeof(ushort) * rx);
	for (i64 j = image.size.y - 1; j >= 0; j--)
	{
		uint* sl = image.sl(j);
		i64 first = -1;
		i64 last = -100;
		bool norm = true;
		for (int i = 0; i < rx; i++)
		{
			uint cc = sl[i];
			int e0 = (int)(cc & 255) - c0;
			int e1 = (int)((cc >> 8) & 255) - c1;
			int e2 = (int)((cc >> 16) & 255) - c2;
			if (e0 < 0) e0 = -e0;
			if (e1 < 0) e1 = -e1;
			if (e2 < 0) e2 = -e2;
			lin[i] = (lin[i] << 1) + ((e0 + e1 + e2) <= err);
			if (lin[i])
			{
				if (first < 0)
				{
					first = i;
					norm = false;
				}
				last = i;
				if (lin[i] & 1) norm = true;
				continue;
			}
			if (i - last == 8)
			{ // КРИТИЧЕСКАЯ РАЗНИЦА!!!!
				if (!norm)
				{
					_area_string aa;
					ushort s = 0;
					for (i64 k = first; k <= last; k++) s |= lin[k];
					aa.area = { {first, last + 1}, {j + 1, j + bit16(s) + 1} };
					aa.s = rasp_text(lin + first, last - first + 1);
					elem.push_back(aa);
					ZeroMemory(lin + first, sizeof(ushort) * (last + 1i64 - first));
				}
				first = -1;
			}
		}
	}
	delete[] lin;
}

void _recognize::find_text13(uint c)
{
	elem.clear();
	i64 rx = image.size.x;
	ushort* lin = new ushort[rx];
	ZeroMemory(lin, sizeof(ushort) * rx);
	for (i64 j = image.size.y - 1; j >= 0; j--)
	{
		uint* sl = image.sl(j);
		i64 first = -1;
		i64 last = -100;
		bool norm = true;
		for (int i = 0; i < rx; i++) {
			lin[i] = (lin[i] << 1) + (sl[i] == c);
			if (lin[i])
			{
				if (first < 0)
				{
					first = i;
					norm = false;
				}
				last = i;
				if (lin[i] & 1) norm = true;
				continue;
			}
			if (i - last == 8)
			{ // КРИТИЧЕСКАЯ РАЗНИЦА!!!!
				if (!norm)
				{
					_area_string aa;
					ushort s = 0;
					for (i64 k = first; k <= last; k++) s |= lin[k];
					aa.area = { {first, last + 1}, {j + 1, j + bit16(s) + 1} };
					aa.s = rasp_text(lin + first, last - first + 1);
					elem.push_back(aa);
					ZeroMemory(lin + first, sizeof(ushort) * (last + 1i64 - first));
				}
				first = -1;
			}
		}
	}
	delete[] lin;
}

std::wstring _recognize::rasp_text(ushort* aa, i64 vaa)
{
	std::wstring s;
	wchar_t* s2_ = new wchar_t[vaa + 1i64];
	ushort b = 0;
	for (int k = 0; k < vaa; k++) b |= aa[k];
	i64 vb = bit16(b);
	i64 bestbit = 0;
	for (i64 ii = 0; ii < 3; ii++)
	{
		i64 sm = 12 - vb - ii;
		if (sm < -1) break;
		int L = 0;
		int i = 0;
		i64 nbit = 0;
		while (i < vaa)
		{
			if (aa[i] == 0)
			{
				i++;
				continue;
			}
			wchar_t c = 0;
			int rc = 0;
			i64 bbb = 0;
			_kusok_bukva* bb = &bu;
			for (int j = i; j < vaa; j++)
			{
				ushort m = (sm >= 0) ? (aa[j] << sm) : ((aa[j] >> (-sm)));

				auto fi = std::lower_bound(bb->dalee.begin(), bb->dalee.end(), m);
				if (fi == bb->dalee.end()) break;
				if (*fi != m) break;
				bb = &*fi;
//				uint n;
//				if (!bb->dalee.find_sort(m, &n)) break;
//				bb = &bb->dalee.data[n];

				if (bb->vc)
				{
					c = bb->c[0];
					bbb = bb->nbit[0];
					rc = j - i + 1;
				}
			}
			if (rc == 0)
			{
				i++;
				continue;
				// break;
			}
			i += rc;
			s2_[L++] = c;
			nbit += bbb;
		}
		//if (L > s.size())
		//{
		//	s2_[L] = 0;
		//	s = s2_;
		//}
		if (nbit > bestbit)
		{
			s2_[L] = 0;
			s = s2_;
			bestbit = nbit;
		}

	}
	delete[] s2_;
	return s;
}

int _recognize::find_elem(std::wstring_view s)
{
	int l = (int)elem.size();
	for (int i = 0; i < l; i++)
		if (elem[i].s == s) return i;
	return -1;
}

int _recognize::find_elem_kusok(wstr s)
{
	int l = (int)elem.size();
	for (int i = 0; i < l; i++)
		if (elem[i].s.find(s) != std::string::npos) return i;
	return -1;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void _kusok_bukva::cod(ushort* aa, int vaa, wchar_t cc, char nf, i64 nbitt)
{
	if (vaa == 0)
	{
		if (vc < rc)
		{
			c[vc] = cc;
			f[vc] = nf;
			nbit[vc] = nbitt;
			vc++;
		}
		return;
	}
	auto fi = std::lower_bound(dalee.begin(), dalee.end(), *aa);
	i64 n = fi - dalee.begin();
	bool nena = false;
	if (fi == dalee.end())
		nena = true;
	else
		if (*fi != *aa)
			nena = true;
	if (nena)
	{
		_kusok_bukva b;
		b.mask = *aa;
		dalee.insert(fi, b);
	}
/*	uint n;
	if (!dalee.find_sort(*aa, &n))
	{
		_kusok_bukva b;
		b.mask = *aa;
		dalee.insert(n, b);
	}*/

	dalee[n].cod(aa + 1, vaa - 1, cc, nf, nbitt);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void _g_graph::add(const _matrix& b, std::string_view s, bool bar)
{
	curve.push_back({b, std::string(s), bar});
}

void _g_graph::ris2(_trans tr, bool final)
{
	constexpr double ot = 0.03; // отступ от каждой стороны
	_area a = tr(local_area);
	master_bm.rectangle(a, c_def);
	
	unsigned int col_setka = c_min - 0xA0000000; // цвет сетки
	unsigned int col_font = c_max - 0x80000000; // цвет шрифта

	// вычисление диапазона
	double minx = 1;
	double maxx = 0;
	double miny = 1;
	double maxy = 0;

	for (auto &i : curve)
	{
		_matrix* c = &i.a;
		if ((c->size.x < 1)|| (c->size.x > 2)) continue; // должно быть 1-2 столбца
		double min, max;
		if (c->size.x == 2)
			c->column(0).min_max(&min, &max);
		else
		{
			min = 0;
			max = c->size.y - 1;
		}
		if (minx > maxx)
		{
			minx = min;
			maxx = max;
		}
		else
		{
			if (min < minx) minx = min;
			if (max > maxx) maxx = max;
		}
		if (c->size.x == 2)
			c->column(1).min_max(&min, &max);
		else
			c->column(0).min_max(&min, &max);
		if (i.bar)
		{
			if (max < 0) max = 0;
			if (min > 0) min = 0;
		}
		if (miny > maxy)
		{
			miny = min;
			maxy = max;
		}
		else
		{
			if (min < miny) miny = min;
			if (max > maxy) maxy = max;
		}
	}
	if ((minx > maxx)||(miny > maxy)) return;
	double dx = maxx - minx;
	if (dx == 0) dx = 1;
	double dy = maxy - miny;
	if (dy == 0) dy = 1;
	minx -= dx * ot;
	maxx += dx * ot;
	miny -= dy * ot;
	maxy += dy * ot;
	// рисование графиков
	double kx = a.x.length() / (maxx - minx);
	double ky = a.y.length() / (maxy - miny);
	int ng = 0;
	double delta_bar = 0;
	for (auto& j : curve)
	{
		_matrix* c = &j.a;
		if ((c->size.x < 1) || (c->size.x > 2)) continue; // должно быть 1-2 столбца
		double xpr = 0;
		double ypr = 0;
		uint cc = (ng < _countof(color_set)) ? color_set[ng] : c_max;
//		cc -= 0x80000000;
		ng++;
		for (int i = 0; i < c->size.y; i++)
		{
			double x, y;
			if (c->size.x == 2)
			{
				x = (*c)[i][0];
				y = (*c)[i][1];
			}
			else
			{
				x = i;
				y = (*c)[i][0];
			}
			double xx = (a.x.min + (x - minx) * kx);
			double yy = (a.y.max - (y - miny) * ky);
			if (j.bar)
				master_bm.lines({ xx + delta_bar, (a.y.max + miny * ky) }, { xx + delta_bar, yy }, j.width, cc);
			else
				if (i > 0) master_bm.lines({ xpr, ypr }, { xx, yy }, j.width, cc);
			xpr = xx;
			ypr = yy;
		}
		if (j.bar) delta_bar += j.width;
	}
	// рисование осей
	if (a.y.length() > 10)
	{
		int maxN = a.x.length() / 9; // 1-е приближение
		if (maxN > 1)
		{
			double mi, step;
			os_pordis(minx, maxx, maxN, mi, step);
			int zn = (int)(-log10(step * 1.1) + 1);
			if (zn < 0) zn = 0;
			i64 dex = std::max(master_bm.size_text16(double_to_astring(minx, zn)).x,
				master_bm.size_text16(double_to_astring(maxx, zn)).x) + 4;
			maxN = a.x.length() / dex; // 2-е приближение
			if (maxN > 1)
			{
				os_pordis(minx, maxx, maxN, mi, step);
				zn = (int)(-log10(step * 1.1) + 1);
				if (zn < 0) zn = 0;
				for (double x = mi; x < maxx; x += step)
				{
					double xx = (x - minx) * kx;
					master_bm.line({ i64(xx + a.x.min), (i64)a.y.min }, { i64(xx + a.x.min), (i64)a.y.max }, col_setka);
					std::string s = double_to_astring(x, zn);
					_isize l = master_bm.size_text16(s);
					if (xx < a.x.length() - 50)	master_bm.text16(a.x.min + xx - l.x / 2, std::max(a.y.min, 0.0),
						s, col_font);
					if (xx > 50) master_bm.text16(a.x.min + xx - l.x / 2, std::min((i64)a.y.max, master_bm.size.y) - 13,
						s, col_font);
				}
			}
		}
	}
	if (a.x.length() > 10)
	{
//		int dex = 35; // длина подписи
		int maxN = a.y.length() / 15;
		if (maxN > 1)
		{
			double mi, step;
			os_pordis(miny, maxy, maxN, mi, step);
			int zn = (int)(-log10(step * 1.1) + 1);
			if (zn < 0) zn = 0;
			for (double y = mi; y < maxy; y += step)
			{
				double yy = (y - miny) * ky;
				master_bm.line({ (i64)a.x.min, i64(a.y.max - yy) }, { (i64)a.x.max, i64(a.y.max - yy) }, col_setka);
				std::string s = double_to_astring(y, zn);
				_isize l = master_bm.size_text16(s);
				if (a.y.length() - yy > 16) master_bm.text16(std::max(a.x.min, 0.0) + 2, a.y.max - yy - 6, s, col_font);
				if (yy > 16) master_bm.text16(std::min((i64)a.x.max, master_bm.size.x) - l.x - 2, a.y.max - yy - 6,
					s, col_font);
			}
		}
	}
	if ((a.x.length() > 50) && (a.y.length() > 50))
	{
		i64 y = std::max(a.y.min, 0.0);
		i64 x = std::min((i64)a.x.max, master_bm.size.x) - 50;
		ng = -1;
		for (auto& j : curve)
		{
			ng++;
			if (j.caption == "") continue;
			y += 16;
			uint cc = (ng < _countof(color_set)) ? color_set[ng] : c_max;
			master_bm.text16(x, y, j.caption, cc);
			master_bm.lines({ x - 20.0, y + 6.0 }, { x - 4.0, y + 6.0 }, j.width, cc);
		}

	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool _prices::operator==(const _prices& p) const noexcept
{
	for (i64 i = 0; i < roffer; i++)
	{
		if ((pok[i] != p.pok[i]) || (pro[i] != p.pro[i])) return false;
	}
	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

_prices2::_prices2(const _prices& a)
{
	time = a.time;
	for (i64 i = 0; i < roffer; i++)
	{
		buy[i] = a.pok[i];
		sale[i] = a.pro[i];
	}
}

bool _prices2::operator==(const _prices2& p) const noexcept
{
	for (i64 i = 0; i < roffer; i++)
		if ((buy[i] != p.buy[i]) || (sale[i] != p.sale[i])) return false;
	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void calc_all_prediction(_basic_curve& o, i64 &nn, double &kk)
{
	_super_stat ss_old = ss;
	ss.clear();
	_prices pr;
	i64 rez = 0; // 0 - ожидание, 1 - покупка, 2 - продажа
	i64 t_start = 0;
	i64 t_end   = 0;
	i64 cena = 0;
	i64 cena2 = 0;
	i64 vv = 0;
	double k = 1;
	for (i64 i = 0; i < ss_old.size; i++)
	{
		o.recovery();
		ss_old.read(i, pr);
		ss.add(pr);
		if (rez == 1)
		{
			if (pr.time < t_start + 2) continue; // 2 секунды пауза между решением и действием
			if (pr.time > t_start + 60) { rez = 0; continue; } // что-то не так
			rez = 2;
			cena = pr.pro[0].c;
			continue;
		}
		if (rez == 2)
		{
			if (pr.time < t_end) continue; // еще не время
			if (pr.time > t_end + 60) { rez = 0; continue; }; // что-то не так
			cena2 = pr.pok[0].c;
			rez = 0;
			vv++;
			k *= (cena2 * 0.999) / cena;
			continue;
		}
		i64 t = o.prediction();
		if (t <= 0) continue;
		t_start = pr.time;
		t_end = pr.time + t * 60;
		rez = 1;
	}
	nn = vv;
	kk = k;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

i64 _statistics::number(i64 be, i64 en) const noexcept
{
	auto li1 = std::lower_bound(data.begin(), data.end(), be, [](_one_stat a, i64 b) { return (a.value < b); });
	auto li2 = std::lower_bound(data.begin(), data.end(), en, [](_one_stat a, i64 b) { return (a.value < b); });
	return number(li1, li2);
}

i64 _statistics::number(_cit be, _cit en) const noexcept
{
	i64 s = 0;
	for (auto i = be; i != en; ++i) s += i->number;
	return s;
}

bool _statistics::operator==(const _statistics& a) const noexcept
{
	if (data.size() != a.data.size()) return false;
	for (i64 i = 0; i < (i64)data.size(); i++)
		if (data[i] != a.data[i]) return false;
	return true;
}

void _statistics::operator+=(const _statistics& a)
{
	_statistics res;
	auto i = data.begin();
	auto ai = a.data.begin();
	while ((i != data.end()) && (ai != a.data.end()))
	{
		if (i->value < ai->value)
		{
			res.data.push_back(*i);
			++i;
			continue;
		}
		if (ai->value < i->value)
		{
			res.data.push_back(*ai);
			++ai;
			continue;
		}
		res.data.push_back({ i->value, i->number + ai->number });
		++i;
		++ai;
	}
	for (; i != data.end(); ++i) res.data.push_back(*i);
	for (; ai != a.data.end(); ++ai) res.data.push_back(*ai);
	*this = res;
}

void _statistics::operator=(const _basic_statistics& a)
{
	i64 k = 0;
	for (auto i : a.data) if (i) k++;
	data.clear();
	data.reserve(k);
	for (i64 i = 0; i < (i64)a.data.size(); i++)
		if (a.data[i]) data.push_back({ a.start + i, a.data[i] });
}

i64 _statistics::first_zero()
{
	for (i64 i = 1; i < (i64)data.size(); i++)
		if (data[i].value != data[i - 1].value + 1)
			return data[i - 1].value + 1;
	return -1;
}

double _statistics::arithmetic_size(_it be, _it en)
{
	double s = 0;
	double n = 1.0 / number(be, en);
	for (auto i = be; i != en; ++i) s += i->number * log(i->number * n);
	return -s / log(2.0);
}

_matrix _statistics::to_matrix()
{
	_matrix res(data.size(), 2);
	for (i64 i = 0; i < (i64)data.size(); i++)
	{
		res[i][0] = data[i].value;
		res[i][1] = data[i].number;
	}
	return res;
}

_matrix _statistics::to_matrix(i64 mi, i64 ma)
{
	i64 n;
	i64 k = data.size() - 1;
	for (n = 0; n <= k; n++) if (data[n].value >= mi) break;
	for (; k >= 0; k--) if (data[k].value <= ma) break;
	if (k < n) return {};
	_matrix res(k - n + 1, 2);
	for (i64 i = n; i <= k; i++)
	{
		res[i - n][0] = data[i].value;
		res[i - n][1] = data[i].number;
	}
	return res;
}

i64 _statistics::operator[](i64 n) const noexcept
{
	auto li = std::lower_bound(data.begin(), data.end(), n, [](_one_stat a, i64 b) { return (a.value < b); });
	if (li == data.end()) return 0;
	return (li->value == n) ? li->number : 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

i64 bit_for_value(u64 k) // k - количество чисел. (1) = 0, (2) = 1, (4) = 2
{
	if (k == 0) return 0;
	k--;
	i64 n = 0;
	while (k)
	{
		n++;
		k >>= 1;
	}
	return n;
}

void _cdf3::to_clipboard()
{
	std::stringstream a;
	a << start << ", {";
	for (auto& i : prefix) a << i << ", ";
	a << "}";
	::to_clipboard(a.str().c_str());
}

bool _cdf3::coding(i64 a, _bit_vector& bs) const noexcept
{
	if ((a < start) || (a - start >= (i64)prefix.size())) return false;
	if (bst) bst->push(a); // для переподбора
	bs.pushn1(prefix[a - start]);
	return true;
}

void _cdf3::calc(const _statistics& st, i64 min_value, i64 max_value)
{
	start = min_value;
	i64 n = max_value - min_value + 1;
	prefix.resize(n);
	for (auto& i : prefix) i = 1;
	// вычисление префикса
	std::multimap<i64, std::vector<i64>> xxx3;
	for (i64 i = 0; i < n; i++) xxx3.insert({ st[start + i], {i} });
	while (xxx3.size() > 1)
	{
		auto p1 = xxx3.begin();
		for (auto i : p1->second) prefix[i] <<= 1;
		i64 s = p1->first;
		std::vector<i64> v = p1->second;
		xxx3.erase(p1);
		p1 = xxx3.begin();
		for (auto i : p1->second) prefix[i] = (prefix[i] << 1) + 1;
		s += p1->first;
		v.insert(v.end(), p1->second.begin(), p1->second.end());
		xxx3.erase(p1);
		xxx3.insert({ s, v });
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void _cdf::to_clipboard()
{
	std::stringstream a;
	for (auto& i : fr)
	{
		a << "{" << i.first << ", " << (int)i.bit << ", " << i.prefix << "}, ";
	}
	::to_clipboard(a.str().c_str());
}

/*double _cdf::calc_size1(const _statistics& st)
{
	i64 s = 0;
	for (auto i : st.data)
	{
		auto n = std::upper_bound(fr.begin(), fr.end(), i.value, [](i64 a, _frequency b) { return (a < b.first); });
		if ((n == fr.begin()) || (n == fr.end())) continue; // return -1;
		n--;
		s += ((i64)n->bit0 + n->bit) * i.number;
	}
	return double(s) / st.number();
}*/

bool _cdf::coding(i64 a, _bit_vector& bs) const noexcept
{
	auto n = std::upper_bound(fr.begin(), fr.end(), a, [](i64 a, _frequency b) { return (a < b.first); });
	if ((n == fr.begin()) || (n == fr.end())) return false;
	if (bst) bst->push(a);
	n--;
	bs.pushn1(n->prefix);
	bs.pushn(a - n->first, n->bit);
	return true;
}

void _cdf::calc(const _statistics& st, i64 n, i64 min_value, i64 max_value)
{
	fr.resize(n + 1);

	struct _uuu
	{
		_iinterval o;
		i64 k = 0;
		uchar bit = 0;
		bool operator==(const _uuu& a) const noexcept { return (o == a.o); }
	};

	auto calc_poteri = [](const _uuu& left, const _uuu& right)
	{
		i64 r0 = right.k * right.bit + left.k * left.bit;
		i64 r = (right.k + left.k) * bit_for_value(right.o.max - left.o.min);
		return r - r0;
	};
	// uu - простые интервалы длиной 1, или с количеством 0
	std::vector<_uuu> ee;
	_uuu a;
	i64 pr;
	if (min_value < st.min_value())
	{
		pr = st.min_value();
		a.o = { min_value, pr };
		a.k = 0;
		a.bit = bit_for_value(a.o.size());
		ee.push_back(a);
	}
	else
		pr = min_value;
	for (auto i : st.data)
	{
		if (i.value > max_value) break;
		if (i.value < min_value) continue;
		if (i.value > pr) // нули
		{
			a.o = { pr, i.value };
			a.k = 0;
			a.bit = bit_for_value(a.o.size());
			ee.push_back(a);
		}
		a.o = i.value;
		a.k = i.number;
		a.bit = 0;
		ee.push_back(a);
		pr = i.value + 1;
	}
	if (max_value >= pr)
	{
		a.o = { pr, max_value + 1 };
		a.k = 0;
		a.bit = bit_for_value(a.o.size());
		ee.push_back(a);
	}

	struct _2uuu
	{
		_uuu u1, u2;
		std::multimap<i64, _2uuu>::iterator left, right;
	};

	std::multimap<i64, _2uuu> xxx;
	// подготовка стартовых пар интервалов отсортированных по минимум потерь при объединении
	std::multimap<i64, _2uuu>::iterator pr_it;
	for (i64 i = 1; i < (i64)ee.size(); i++)
	{
		_2uuu aa;
		aa.u1 = ee[i - 1];
		aa.u2 = ee[i];
		std::multimap<i64, _2uuu>::iterator it = xxx.insert({ calc_poteri(aa.u1, aa.u2), aa });
		if (i == 1)
			it->second.left = it;
		else
		{
			pr_it->second.right = it;
			it->second.left = pr_it;
		}
		pr_it = it;
	}
	pr_it->second.right = pr_it;
	// схлопывание пар интервалов, пока их не останется n-1
	while ((i64)xxx.size() >= n)
	{
		auto a_ = xxx.begin(); // минимальная пара
		auto aa = a_->second;
		auto i = aa.left; // левая пара
		auto j = aa.right; // правая пара
		aa.u1.k += aa.u2.k;
		aa.u1.o.max = aa.u2.o.max;
		aa.u1.bit = bit_for_value(aa.u1.o.size());
		_2uuu ii, jj;
		bool ina = (i != a_);
		bool jna = (j != a_);
		if (ina)
		{
			ii = i->second;
			ii.u2 = aa.u1;
			bool gran = (ii.left == i);
			xxx.erase(i);
			i = xxx.insert({ calc_poteri(ii.u1, ii.u2), ii });
			if (gran)
				i->second.left = i;
			else
				i->second.left->second.right = i;
		}
		if (jna)
		{
			jj = j->second;
			jj.u1 = aa.u1;
			bool gran = (jj.right == j);
			xxx.erase(j);
			j = xxx.insert({ calc_poteri(jj.u1, jj.u2), jj });
			j->second.left = (ina) ? i : j;
			if (gran)
				j->second.right = j;
			else
				j->second.right->second.left = j;
		}
		if (ina)
			i->second.right = (jna) ? j : i;
		xxx.erase(a_);
	}
	// подкотовка отсортированных интервалов
	std::map<i64, _uuu> xxx2;
	for (auto& i : xxx)
	{
		xxx2[i.second.u1.o.min] = i.second.u1;
		xxx2[i.second.u2.o.min] = i.second.u2;
	}
	// заполнение массива частот
	auto ii = xxx2.begin();
	for (i64 i = 0; i < n; i++)
	{
		fr[i].first = ii->second.o.min;
		fr[i].bit = ii->second.bit;
		fr[i].prefix = 1;
		++ii;
	}
	fr[n].first = max_value + 1;
	fr[n].bit = 0;
	fr[n].prefix = 1;
	// коррекция - дозаполнение маленьких интервалов, за счет больших соседей
	for (i64 i = 0; i < n - 1; i++)
	{
		i64 delta = (1ll << fr[i].bit) - (fr[i + 1].first - fr[i].first);
		if (delta == 0) continue;
		if (i > 0)
			if (fr[i].bit < fr[i - 1].bit)
			{
				fr[i].first -= delta;
				fr[i - 1].bit = bit_for_value(fr[i].first - fr[i - 1].first); // вдруг уменьшилось??
				continue;
			}
		if (fr[i].bit <= fr[i + 1].bit)
		{
			fr[i + 1].first += delta;
			fr[i + 1].bit = bit_for_value(fr[i + 2].first - fr[i + 1].first); // вдруг уменьшилось??
			continue;
		}
	}
	// вычисление префикса
	std::multimap<i64, std::vector<i64>> xxx3;
	for (i64 i = 0; i < n; i++) xxx3.insert({ st.number(fr[i].first, fr[i + 1].first), {i} });
	while (xxx3.size() > 1)
	{
		auto p1 = xxx3.begin();
		for (auto i : p1->second) fr[i].prefix <<= 1;
		i64 s = p1->first;
		std::vector<i64> v = p1->second;
		xxx3.erase(p1);
		p1 = xxx3.begin();
		for (auto i : p1->second) fr[i].prefix = (fr[i].prefix << 1) + 1;
		s += p1->first;
		v.insert(v.end(), p1->second.begin(), p1->second.end());
		xxx3.erase(p1);
		xxx3.insert({ s, v });
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

double test_ss4()
{
	_prices pr;
	for (i64 i = 0; i < ss.size; i++)
	{
		ss.read(i, pr);
		sss.add(pr);
	}
	return (double)sss.data.size() / (sss.size * 8);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void _basic_statistics::push(i64 x)
{
	if (x - start >= (i64)data.size()) data.resize(x - start + 1, 0);
	if (x < start) { data.insert(data.begin(), start - x, 0); start = x; }
	data[x - start]++;
}

i64 _basic_statistics::number() const noexcept
{
	i64 s = 0;
	for (auto i : data) s += i;
	return s;
}

i64 _basic_statistics::number(i64 be, i64 en) const noexcept
{
	be -= start;
	en -= start;
	if (be < 0) be = 0;
	if (en > (i64)data.size()) en = (i64)data.size();
	i64 s = 0;
	for (i64 i = be; i < en; i++) s += data[i];
	return s;
}

i64 _basic_statistics::operator[](i64 x) const noexcept
{
	if (x - start >= (i64)data.size()) return 0;
	if (x < start) return 0;
	return data[x - start];
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
