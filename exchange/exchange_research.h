﻿#pragma once

#include "exchange_basic.h"
#include "tetron.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct _delta_offer
{
	int price;
	int delta_number;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct _delta_offers
{
	_delta_offer delta_offer[size_offer];
	i64 size;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct _delta_supply_and_demand // предложение и спрос
{
	_delta_offers delta_demand; // желающие купить
	_delta_offers delta_supply; // желающие продать
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void exchange_fun1(_g_terminal* t);
void exchange_fun2(_g_terminal* t);

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
