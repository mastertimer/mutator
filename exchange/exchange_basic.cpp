#include "exchange_basic.h"

#include "g_terminal.h"

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

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

_g_terminal& operator << (_g_terminal& t, const _supply_and_demand& sd)
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
