﻿#include "g_exchange_graph.h"
#include "exchange_data.h"
#include "exchange_research.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

_delta_offers::_delta_offers(const _offers& a, const _offers& b)
{
	i64 end_price;
	if (a.offer[1].price > a.offer[0].price)
	{
		delta_price = 1;
		start_price = std::min(a.offer.front().price, b.offer.front().price);
		end_price = std::min(a.offer.back().price, b.offer.back().price);
	}
	else
	{
		delta_price = -1;
		start_price = std::max(a.offer[0].price, b.offer[0].price);
		end_price = std::max(a.offer.back().price, b.offer.back().price);
	}
	delta_number.resize((end_price - start_price) * delta_price + 1, { 0, 0 });
	for (auto i : a.offer)
	{
		i64 n = (i.price - start_price) * delta_price;
		if ((n >= 0) && (n < (i64)delta_number.size())) delta_number[n].old_number = i.number;
	}
	for (auto i : b.offer)
	{
		i64 n = (i.price - start_price) * delta_price;
		if ((n >= 0) && (n < (i64)delta_number.size())) delta_number[n].new_number = i.number;
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

_delta_supply_and_demand::_delta_supply_and_demand(const _supply_and_demand& a, const _supply_and_demand& b):
	delta_demand(a.demand, b.demand), delta_supply(a.supply, b.supply)
{

}

_delta_supply_and_demand operator-(const _supply_and_demand& a, const _supply_and_demand& b)
{
	return _delta_supply_and_demand(b, a);
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
			}
		prev = &i;
	}
}

void exchange_fun2(_g_terminal* t)
{
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
