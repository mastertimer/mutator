#pragma once

#include "stock_basic.h"

struct _compression_stock_statistics
{
	i64 size = 0; // количество записей
	_bit_vector data; // сжатые данные

	bool add(const _supply_and_demand& c); // добавить цены (сжать)
	bool read(_supply_and_demand& c); // прочитать цены (расжать)
	void push_to(_stack& mem);
	void pop_from(_stack& mem);

private:
	std::vector<_offer> base_buy; // база покупки (первых 20 - последние цены)
	std::vector<_offer> base_sale; // база продажи (первых 20 - последние цены)
	time_t back_time = 0; // время прочитанных цен

	bool add0(const _supply_and_demand& c); // не дельта!
	bool add1(const _supply_and_demand& c); // дельта
	bool add12(const _offer* v1, std::vector<_offer>& v0, i64 izm);
	bool coding_delta_number(i64 a, i64 b);
	i64  decoding_delta_number(i64 a);
	bool read0(_supply_and_demand& c);
	bool read1(_supply_and_demand& c);
	bool read12(_offer* v1, std::vector<_offer>& v0);
};

