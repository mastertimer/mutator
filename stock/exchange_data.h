#pragma once

#include "compressed_exchange_data.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct _exchange_data
{
	i64 info_compressed_size = 0;

	void push_back(const _supply_and_demand& c);
	const std::vector<_supply_and_demand>& operator*() const { return sad; }
	const std::vector<_supply_and_demand>* operator->() const { return &sad; }
	const _supply_and_demand& operator[](i64 n) const { return sad[n]; }
	void operator=(_compressed_exchange_data& cs);
	void save_to_file(std::wstring_view fn);
	void load_from_file(std::wstring_view fn);
	i64 size() const { return sad.size(); }

private:
	std::vector<_supply_and_demand> sad;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

inline _exchange_data exchange_data; // посекундные цены

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
