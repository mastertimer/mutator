#include <ctime>
#include <thread>

#include "stock_statistics.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void _stock_statistics::save_to_file(std::wstring_view fn)
{
	_compression_stock_statistics cs[number_thread];
	std::vector<std::thread> threads;

	auto fun = [](_compression_stock_statistics* co, _supply_and_demand* sad, i64 k)
	{
		for (i64 i = 0; i < k; i++) co->add(sad[i]);
	};

	for (i64 i = 0; i < number_thread; i++)
	{
		threads.emplace_back(fun, &cs[i], &sad[sad.size() * i / number_thread],
			(sad.size() * (i + 1) / number_thread) - (sad.size() * i / number_thread));
	}
	for (auto& th : threads) th.join();
	_stack mem;
	for (i64 i = 0; i < number_thread; i++)	cs[i].push_to(mem);
	mem.save_to_file(fn);
}

void _stock_statistics::load_from_file(std::wstring_view fn)
{
	_compression_stock_statistics cs[number_thread];
	std::vector<std::thread> threads;

	_stack mem;
	mem.load_from_file(fn);
	i64 v = 0;
	for (i64 i = 0; i < number_thread; i++)
	{
		cs[i].pop_from(mem);
		v += cs[i].size;
	}
	sad.resize(v);

	auto fun = [](_compression_stock_statistics* co, _supply_and_demand* sad)
	{
		for (i64 i = 0; i < co->size; i++) co->read(sad[i]);
	};

	v = 0;
	for (i64 i = 0; i < number_thread; i++)
	{
		threads.emplace_back(fun, &cs[i], &sad[v]);
		v += cs[i].size;
	}
	for (auto& th : threads) th.join();
}

void _stock_statistics::push_back(const _supply_and_demand& c)
{
	if (!sad.empty())
	{
		if (c == sad.back()) return;
		//		if (c.time <= sad.back().time) return;
	}
	sad.push_back(c);
}

void _stock_statistics::operator=(_compression_stock_statistics& cs)
{
	sad.clear();
	sad.reserve(cs.size);
	_supply_and_demand c;
	for (i64 i = 0; i < cs.size; i++)
	{
		cs.read(c);
		sad.push_back(c);
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

