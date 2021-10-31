#include <ctime>
#include <thread>

#include "exchange_data.h"

constexpr wchar_t file_stock_statistics[] = L"..\\..\\data\\base.c3";

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void _exchange_data::save_to_file()
{
	std::wstring fn = exe_path + file_stock_statistics;
	_compressed_exchange_data cs[number_thread];
	std::vector<std::thread> threads;

	auto fun = [](_compressed_exchange_data* co, _supply_and_demand* sad, i64 k)
	{
		for (i64 i = 0; i < k; i++) co->add(sad[i]);
	};

	for (i64 i = 0; i < number_thread; i++)
	{
		threads.emplace_back(fun, &cs[i], &(*this)[size() * i / number_thread],
			(size() * (i + 1) / number_thread) - (size() * i / number_thread));
	}
	for (auto& th : threads) th.join();
	_stack mem;
	for (i64 i = 0; i < number_thread; i++)	cs[i].push_to(mem);
	info_compressed_size = mem.size;
	mem.save_to_file(fn);
}

void _exchange_data::load_from_file()
{
	std::wstring fn = exe_path + file_stock_statistics;
	_compressed_exchange_data cs[number_thread];
	std::vector<std::thread> threads;

	_stack mem;
	mem.load_from_file(fn);
	info_compressed_size = mem.size;
	i64 v = 0;
	for (i64 i = 0; i < number_thread; i++)
	{
		cs[i].pop_from(mem);
		v += cs[i].size;
	}
	resize(v);

	auto fun = [](_compressed_exchange_data* co, _supply_and_demand* sad)
	{
		for (i64 i = 0; i < co->size; i++) co->read(sad[i]);
	};

	v = 0;
	for (i64 i = 0; i < number_thread; i++)
	{
		threads.emplace_back(fun, &cs[i], &(*this)[v]);
		v += cs[i].size;
	}
	for (auto& th : threads) th.join();
}

void _exchange_data::push_back(const _supply_and_demand& c)
{ // *
	if (!empty())
	{
		if (c == back()) return;
		if (c.time <= back().time) return;
	}
	std::vector<_supply_and_demand>::push_back(c);
}

void _exchange_data::operator=(_compressed_exchange_data& cs)
{
	clear();
	reserve(cs.size);
	_supply_and_demand c;
	for (i64 i = 0; i < cs.size; i++)
	{
		cs.read(c);
		std::vector<_supply_and_demand>::push_back(c);
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool update_index_data()
{
	i64 vcc = 0;
	if (!index_data.empty()) vcc = index_data.back().ncc.max;
	if (vcc == (i64)exchange_data.size()) return false; // ничего не изменилось
	if (vcc > (i64)exchange_data.size())
	{
		index_data.clear(); // обработанных данных больше, чем исходных, потому пусть будет полный перерасчет
		vcc = 0;
	}
	if (exchange_data.size() < 2) return false; // мало данных для обработки
	i64 back_minute = exchange_data.back().time_to_minute();
	if (!index_data.empty())
	{
		if (back_minute == index_data.back().time + 1) return false; // еще рано
		if (back_minute <= index_data.back().time) // так быть не должно, полный перерасчет
		{
			index_data.clear();
			vcc = 0;
		}
	}
	if (exchange_data.size() - vcc < 2) return false; // мало данных для обработки
	time_t t = 0;
	_index cp;
	for (i64 i = vcc; i < (i64)exchange_data.size(); i++)
	{
		const _supply_and_demand& cc = exchange_data[i];
		time_t t2 = cc.time_to_minute();
		if (t2 == t)
		{
			double aa = ((i64)cc.demand[0].price + cc.supply[0].price) * (c_unpak * 0.5);
			if (aa < cp.min) cp.min = aa;
			if (aa > cp.max) cp.max = aa;
			if (cc.demand.back().price * c_unpak < cp.minmin) cp.minmin = cc.demand.back().price * c_unpak;
			if (cc.supply.back().price * c_unpak > cp.maxmax) cp.maxmax = cc.supply.back().price * c_unpak;
			cp.ncc.max++;
			cp.last = aa;
			if (cc.time % 60 == 3)
			{
				cp.c3_buy = cc.demand[0].price * c_unpak;
				cp.c3_sale = cc.supply[0].price * c_unpak;
			}
			continue;
		}
		if (t != 0)
		{
			index_data.push_back(cp);
		}
		if (t2 == back_minute) break; // последнюю минуту пока не трогать
		t = t2;
		cp.time = t;
		cp.ncc.min = i;
		cp.ncc.max = i + 1;
		cp.max = cp.min = cp.last = cp.first = ((i64)cc.demand[0].price + cc.supply[0].price) * (c_unpak * 0.5);
		cp.minmin = cc.demand.back().price * c_unpak;
		cp.maxmax = cc.supply.back().price * c_unpak;
		cp.c3_buy = cc.demand[0].price * c_unpak;
		cp.c3_sale = cc.supply[0].price * c_unpak;
	}
	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
