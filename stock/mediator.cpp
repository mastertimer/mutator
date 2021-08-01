#include <chrono>

#include "mediator.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

constexpr wchar_t mmm_file[] = L"..\\..\\data\\mmm.txt";

std::wstring mmm1 = L"1";
std::wstring mmm2 = L"2";
std::wstring mmm3 = L"3";

int kkk2 = 1; // количество продаваемых акций

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void load_mmm()
{
	static bool first = true;
	if (!first) return;
	first = false;
	_rjson fs((exe_path + mmm_file).c_str());
	fs.read("mmm1", mmm1);
	fs.read("mmm2", mmm2);
	fs.read("mmm3", mmm3);
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
			bm_[nf].text({ 0LL, 0LL }, ss_.data(), 8, 0xffffff, 0);
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
	load_mmm();
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
	EnumChildWindows(w, PoiskOkna, (LPARAM)&aa);
	return aa.hwnd_;
}

int _recognize::read_tablica_zayavok(int a, int& b)
{
	load_mmm();
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
	load_mmm();
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

int _recognize::test_image(_supply_and_demand* pr)
{
	find_text13(0xFF0000FF); // синим цветом покупки
	if (elem.size() != size_offer * 2) return 3;
	i64 pre = 0;
	for (int i = 0; i < size_offer; i++)
	{
		i64 a = to_int(elem[i * 2i64].s);
		if (a <= pre) return 4;
		pre = a;
		if ((a < 1) || (a > 65000)) return 5;
		pr->demand.offer[size_offer - 1 - i].price = static_cast<ushort>(a);
		a = to_int(elem[i * 2i64 + 1i64].s);
		if ((a < 1) || (a > 2000000000)) return 6;
		pr->demand.offer[size_offer - 1 - i].number = static_cast<int>(a);
	}
	find_red_text13(24); // красным цветом продажи
	if (elem.size() != size_offer * 2) return 7;
	for (int i = 0; i < size_offer; i++)
	{
		i64 a = to_int(elem[i * 2i64].s);
		if (a <= pre) return 8;
		pre = a;
		if ((a < 1) || (a > 65000)) return 9;
		pr->supply.offer[i].price = static_cast<ushort>(a);
		a = to_int(elem[i * 2i64 + 1i64].s);
		if ((a < 1) || (a > 2000000000)) return 10;
		pr->supply.offer[i].number = static_cast<int>(a);
	}
	return 0;
}

int _recognize::read_prices_from_screen(_supply_and_demand* pr)
{
	load_mmm();
	HWND w = FindWindow(0, mmm3.c_str());
	if (!w) return 1;
	HWND w2 = FindSubWindow(w, L"InfoPriceTable", L"Сбербанк [МБ ФР: Т+ Акции и ДР] Котировки"); // InfoPriceTable HostWindow
	if (!w2) return 2;
	image.clear(0xFFFFFFFF); // т.к. если окно свернуто, то не грабится
	pr->time = time(0);
	image.grab_ecran_oo2(w2);
	find_text13(0xFF0000FF); // синим цветом покупки
	if (elem.size() != size_offer * 2) return 3;
	i64 pre = 0;
	for (i64 i = 0; i < size_offer; i++)
	{
		std::wstring swe = elem[i * 2].s;
		i64 a = to_int(swe);
		if (a <= pre) return 4;
		pre = a;
		if ((a < 1) || (a > 65000)) return 5;
		pr->demand.offer[size_offer - 1 - i].price = static_cast<ushort>(a);
		a = to_int(elem[i * 2 + 1].s);
		if ((a < 1) || (a > 2000000000)) return 6;
		pr->demand.offer[size_offer - 1 - i].number = static_cast<int>(a);
	}
	find_red_text13(24); // красным цветом продажи
	if (elem.size() != size_offer * 2) return 7;
	for (i64 i = 0; i < size_offer; i++)
	{
		i64 a = to_int(elem[i * 2].s);
		if (a <= pre) return 8;
		pre = a;
		if ((a < 1) || (a > 65000)) return 9;
		pr->supply.offer[i].price = static_cast<ushort>(a);
		a = to_int(elem[i * 2 + 1].s);
		if ((a < 1) || (a > 2000000000)) return 10;
		pr->supply.offer[i].number = static_cast<int>(a);
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

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void buy_stock(_tetron* tt, bool buy)
{
	load_mmm();
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
		uchar* cc = (uchar*)&c;
		bool cfpok = (cc[0] != cc[1]) || (cc[0] != cc[2]); // не серый цвет
		c = recognize.image.sl(recognize.elem[n_pro].area.y.min)[recognize.elem[n_pro].area.x.min - 1];
		cc = (uchar*)&c;
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

