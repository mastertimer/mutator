#include "exchange_basic.h"

#include "e_terminal.h"
#include "json.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace
{

	constexpr wchar_t mmm_file[] = L"..\\..\\se\\data\\mmm.txt";
	bool mmm_loaded = false;
	std::wstring mmm1s = L"1";
	std::wstring mmm2s = L"2";
	std::wstring mmm3s = L"3";

	void load_mmm()
	{
		mmm_loaded = true;
		_rjson fs(exe_path / mmm_file);
		fs.read("mmm1", mmm1s);
		fs.read("mmm2", mmm2s);
		fs.read("mmm3", mmm3s);
	}

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

std::wstring& mmm1()
{
	if (!mmm_loaded) load_mmm();
	return mmm1s;
}

std::wstring& mmm2()
{
	if (!mmm_loaded) load_mmm();
	return mmm2s;
}

std::wstring& mmm3()
{
	if (!mmm_loaded) load_mmm();
	return mmm3s;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool _supply_and_demand::operator==(const _supply_and_demand& p) const
{ // *
	return (demand == p.demand) && (supply == p.supply);
}

i64 _supply_and_demand::time_hour()
{ // *
	tm t3;
	localtime_s(&t3, &time);
	return t3.tm_hour;
}

i64 _supply_and_demand::time_minute()
{ // *
	tm t3;
	localtime_s(&t3, &time);
	return t3.tm_min;
}

bool _supply_and_demand::valid()
{
	if (empty()) return false;
	int pre = 0;
	for (i64 i = 0; i < size_offer; i++)
	{
		int a = demand[size_offer - 1 - i].price;
		if (a <= pre) return false;
		pre = a;
		if ((a < 1) || (a > 65000)) return false;
		a = demand[size_offer - 1 - i].number;
		if ((a < 1) || (a > 2000000000)) return false;
	}
	for (i64 i = 0; i < size_offer; i++)
	{
		int a = supply[i].price;
		if (a <= pre) return false;
		pre = a;
		if ((a < 1) || (a > 65000)) return false;
		a = supply[i].number;
		if ((a < 1) || (a > 2000000000)) return false;
	}
	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

_e_terminal& operator << (_e_terminal& t, const _supply_and_demand& sd)
{
	tm a;
	localtime_s(&a, &sd.time);
	char s[20];
	strftime(s, sizeof(s), "%d.%m.%G %T", &a);
	t.print(L">>>>>>> " + string_to_wstring(s));
	for (i64 i = size_offer - 1; i >= 0; i--)
		t.print(double_to_wstring(sd.supply[i].price * c_unpak, 2) + L": " + std::to_wstring(sd.supply[i].number));
	t.print(L"-------");
	for (auto i : sd.demand) t.print(double_to_wstring(i.price * c_unpak, 2) + L": " + std::to_wstring(i.number));
	return t;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
