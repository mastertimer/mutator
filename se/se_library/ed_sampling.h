#pragma once

#include "exchange_data.h"
#include "statistics.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct filtered_sad_
{
	int index = 0;
	uint demand_filter = 0xfffff; // 20 бит
	uint supply_filter = 0xfffff; // 20 бит
	bool empty() const { return (demand_filter == 0) && (supply_filter == 0); }
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct ed_sampling_
{
	std::vector<filtered_sad_> data;

	ed_sampling_() = default;
	ed_sampling_(const _exchange_data& _ed);
	i64 size() const{ return data.size(); }
	_statistics get_statistics(); // статистика количеств
	void forr(std::function<void(_offer)> fun) const;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

ed_sampling_ fltr_demand(const ed_sampling_& eds);
ed_sampling_ fltr_supply(const ed_sampling_& eds);
ed_sampling_ fltr_position(const ed_sampling_& eds, uint filter);
ed_sampling_ fltr_ruble1(const ed_sampling_& eds, int r1); // по последней цифре рубля
ed_sampling_ fltr_kopeck2(const ed_sampling_& eds, int k2); // 2 цифры копеек

int max_number(const ed_sampling_& eds);
