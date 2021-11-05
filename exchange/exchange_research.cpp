#include "exchange_research.h"
#include "g_exchange_graph.h"
#include "exchange_data.h"
#include "g_terminal.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

_delta_offers::_delta_offers(const _offers& a, const _offers& b)
{
	i64 end_price;
	if (a[1].price > a[0].price)
	{
		delta = 1;
		start = std::min(a.front().price, b.front().price);
		end_price = std::min(a.back().price, b.back().price);
	}
	else
	{
		delta = -1;
		start = std::max(a.front().price, b.front().price);
		end_price = std::max(a.back().price, b.back().price);
	}
	number.resize((end_price - start) * delta + 1, { 0, 0 });
	for (auto i : a)
	{
		i64 n = (i.price - start) * delta;
		if ((n >= 0) && (n < (i64)number.size())) number[n].old_number = i.number;
	}
	for (auto i : b)
	{
		i64 n = (i.price - start) * delta;
		if ((n >= 0) && (n < (i64)number.size())) number[n].new_number = i.number;
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

_delta_supply_and_demand::_delta_supply_and_demand(const _supply_and_demand& a, const _supply_and_demand& b):
	demand(a.demand, b.demand), supply(a.supply, b.supply)
{
}

_delta_supply_and_demand operator-(const _supply_and_demand& a, const _supply_and_demand& b)
{
	return _delta_supply_and_demand(b, a);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void exchange_fun1(_g_terminal* t)
{ // общая информация
	start_stock();
	t->print(L"количество цен: " + std::to_wstring(ed.size()));
	t->print(L"размер сжатой записи: " + double_to_wstring(double(ed.info_compressed_size) / ed.size(), 1)); // 20.2
	const _supply_and_demand* prev = nullptr;
	for (auto& i : ed)
	{
		if (prev)
			if (i.time - prev->time == 1)
			{
				auto ee = i - *prev;
			}
		prev = &i;
	}
	if (prev) *t << *prev;
}

void exchange_fun2(_g_terminal* t, std::vector<std::wstring>& parameters)
{ // вывод конкретных цен
	if (parameters.empty()) return;
	start_stock();
	auto n = std::stoll(parameters[0]);
	if ((n < 0) || (n >= (i64)ed.size())) return;
	*t << ed[n];
}

void exchange_fun3(_g_terminal* t, std::vector<std::wstring>& parameters)
{ // вывод сравнения цен
	if (parameters.size() != 2) return;
	start_stock();
	auto n1 = std::stoll(parameters[0]);
	if ((n1 < 0) || (n1 >= (i64)ed.size())) return;
	auto n2 = std::stoll(parameters[1]);
	if ((n2 < 0) || (n2 >= (i64)ed.size())) return;
	auto delta = ed[n2] - ed[n1];
	*t << delta;
}

_g_terminal& operator << (_g_terminal& t, const _delta_supply_and_demand& delta)
{
	for (i64 i = delta.supply.number.size() - 1; i >= 0; i--)
	{
		auto num1 = delta.supply.number[i].old_number;
		auto num2 = delta.supply.number[i].new_number;
		if (num1 == 0 && num2 == 0) continue;
		t.print(double_to_wstring((delta.supply.start + i * delta.supply.delta) * c_unpak, 2) +	L": " +
			std::to_wstring(num1) + ((num1 == num2)?L" == ":((num1 > num2) ? L" -- " : L" ++ ")) +
			std::to_wstring(num2));
	}
	t.print(L"-------");
	for (i64 i = 0; i < (i64)delta.demand.number.size(); i++)
	{
		auto num1 = delta.demand.number[i].old_number;
		auto num2 = delta.demand.number[i].new_number;
		if (num1 == 0 && num2 == 0) continue;
		t.print(double_to_wstring((delta.demand.start + i * delta.demand.delta) * c_unpak, 2) + L": " +
			std::to_wstring(num1) + ((num1 == num2) ? L" == " : ((num1 > num2) ? L" -- " : L" ++ ")) +
			std::to_wstring(num2));
	}
	return t;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
