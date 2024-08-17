#pragma once

#include "exchange_basic.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct _delta_number
{
	int old_number;
	int new_number;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct _delta_offers
{
	std::vector<_delta_number> number;
	i64 start;
	i64 delta;

	_delta_offers(const _offers& a, const _offers& b);
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct _delta_supply_and_demand // изменение предложения и спроса
{
	_delta_offers demand; // желающие купить
	_delta_offers supply; // желающие продать

	_delta_supply_and_demand(const _supply_and_demand& a, const _supply_and_demand& b);
};

_delta_supply_and_demand operator-(const _supply_and_demand& a, const _supply_and_demand& b);

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct _e_terminal;

void exchange__fun1(_e_terminal& trm, const std::vector<std::wstring>& parameters);
void exchange__fun2(_e_terminal& trm, const std::vector<std::wstring>& parameters);
void exchange__fun3(_e_terminal& trm, const std::vector<std::wstring>& parameters);
void test__filter(_e_terminal& trm, const std::vector<std::wstring>& parameters);

_e_terminal& operator << (_e_terminal& t, const _delta_supply_and_demand& delta);

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
