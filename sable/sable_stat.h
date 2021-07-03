#pragma once

#include "compression_stock_statistics.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct _stock_statistics
{
	static constexpr double c_unpak = 0.01; // распаковка цен
	static constexpr double c_pak = 100; // упаковка цен

	void push_back(const _supply_and_demand& c);
	const std::vector<_supply_and_demand>& operator*() const { return sad; }
	const std::vector<_supply_and_demand>* operator->() const { return &sad; }
	const _supply_and_demand& operator[](i64 n) const { return sad[n]; }
	void operator=(_compression_stock_statistics& cs);
	void save_to_file(std::wstring_view fn);
	void load_from_file(std::wstring_view fn);
	i64 size() const { return sad.size(); }

private:
	static const i64 number_thread = 16; // !! 16, не менять!!
	std::vector<_supply_and_demand> sad;
};

inline _stock_statistics stock_statistics; // посекундные цены

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
