#include "g_exchange_graph.h"
#include "exchange_data.h"
#include "exchange_research.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

_delta_offer operator-(_offer a, _offer b) // =a-b
{
	return { a.price, a.number - b.number };
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

_delta_offers operator-(const _offers& a, const _offers& b)
{
	_delta_offers res;
	res.size = 0;
	bool increase = a.offer[1].price > a.offer[0].price;
	for (i64 ia = 0, ib = 0; ia < size_offer && ib < size_offer;)
	{
		if (a.offer[ia].price != b.offer[ib].price)
		{
			if (increase ^ (a.offer[ia].price < b.offer[ib].price)) ib++; else ia++;
			continue;
		}
		res.delta_offer[res.size++] = a.offer[ia++] - b.offer[ib++];
	}
	return res;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

_delta_offers2::_delta_offers2(const _offers& a, const _offers& b)
{
	if (a.offer[1].price > a.offer[0].price)
	{
		delta_price = 1;
		start_price = std::min(a.offer[0].price, b.offer[0].price);
//		int end_price = std::min(a.offer[0].price, b.offer[0].price);
	}
	else
	{
		delta_price = -1;
		start_price = std::max(a.offer[0].price, b.offer[0].price);

	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

_delta_supply_and_demand::_delta_supply_and_demand(const _supply_and_demand& a, const _supply_and_demand& b):
	delta_demand{ a.demand - b.demand }, delta_supply{ a.supply - b.supply }
{
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

_delta_supply_and_demand2::_delta_supply_and_demand2(const _supply_and_demand& a, const _supply_and_demand& b):
	delta_demand(a.demand, b.demand), delta_supply(a.supply, b.supply)
{

}

_delta_supply_and_demand2 operator-(const _supply_and_demand& a, const _supply_and_demand& b)
{
	return _delta_supply_and_demand2(a, b);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void exchange_fun1(_g_terminal* t)
{
	start_stock();
	t->add_text(L"количество цен: " + std::to_wstring(exchange_data.size()));
	t->add_text(L"размер сжатой записи: " + double_to_wstring(double(exchange_data.info_compressed_size) / exchange_data.size(), 1)); // 20.2
	const _supply_and_demand* prev = nullptr;
	for (auto& i : exchange_data)
	{
		if (prev)
			if (i.time - prev->time == 1)
			{
				auto ee = i - *prev;
				_delta_supply_and_demand delta(i, *prev);
			}
		prev = &i;
	}
}

void exchange_fun2(_g_terminal* t)
{
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
