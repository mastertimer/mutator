#include "ed_sampling.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

ed_sampling_::ed_sampling_(const _exchange_data& _ed) : data(ed.size())
{
	for (int i = 0; i < _ed.size(); i++) data[i].index = i;
}

_statistics ed_sampling_::get_statistics()
{
	_basic_statistics st;
	forr([&](_offer a) { st.push(a.number); });
	return st;
}

void ed_sampling_::forr(std::function<void(_offer)> fun) const
{
	for (auto i : data)
	{
		const auto& sad = ed[i.index];
		for (auto j = 0; j < size_offer; j++)
		{
			uint mask = 1 << j;
			if (i.demand_filter & mask)	fun(sad.demand[j]);
			if (i.supply_filter & mask)	fun(sad.supply[j]);
		}
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

ed_sampling_ fltr_demand(const ed_sampling_& eds)
{
	ed_sampling_ result;
	result.data.reserve(eds.data.size());
	for (auto i : eds.data)
	{
		i.supply_filter = 0;
		if (i.demand_filter) result.data.push_back(i);
	}
	return result;
}

ed_sampling_ fltr_supply(const ed_sampling_& eds)
{
	ed_sampling_ result;
	result.data.reserve(eds.data.size());
	for (auto i : eds.data)
	{
		i.demand_filter = 0;
		if (i.supply_filter) result.data.push_back(i);
	}
	return result;
}

ed_sampling_ fltr_position(const ed_sampling_& eds, uint filter)
{
	ed_sampling_ result;
	result.data.reserve(eds.data.size());
	for (auto i : eds.data)
	{
		i.demand_filter &= filter;
		i.supply_filter &= filter;
		if (!i.empty()) result.data.push_back(i);
	}
	return result;
}

ed_sampling_ fltr_ruble1(const ed_sampling_& eds, int r1)
{
	ed_sampling_ result;
	result.data.reserve(eds.data.size());
	for (auto i : eds.data)
	{
		const auto& sad = ed[i.index];
		for (auto j = 0; j < size_offer; j++)
		{
			uint mask = 1 << j;
			if (i.demand_filter & mask)	if ((sad.demand[j].price / 100) % 10 != r1) i.demand_filter -= mask;
			if (i.supply_filter & mask)	if ((sad.supply[j].price / 100) % 10 != r1) i.supply_filter -= mask;
		}
		if (!i.empty()) result.data.push_back(i);
	}
	return result;
}

ed_sampling_ fltr_kopeck2(const ed_sampling_& eds, int k2)
{
	ed_sampling_ result;
	result.data.reserve(eds.data.size());
	for (auto i : eds.data)
	{
		const auto& sad = ed[i.index];
		for (auto j = 0; j < size_offer; j++)
		{
			uint mask = 1 << j;
			if (i.demand_filter & mask)	if (sad.demand[j].price % 100 != k2) i.demand_filter -= mask;
			if (i.supply_filter & mask)	if (sad.supply[j].price % 100 != k2) i.supply_filter -= mask;
		}
		if (!i.empty()) result.data.push_back(i);
	}
	return result;
}

int max_number(const ed_sampling_& eds)
{
	int result = 0;
	eds.forr([&](_offer a) { if (a.number > result) result = a.number; });
	return result;
}
