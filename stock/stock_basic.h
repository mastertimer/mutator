#pragma once

#include "basic.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

constexpr i64 size_offer = 20; // предложений продажи, предложений покупки ( ВСЕГО = size_offer * 2 );
constexpr double c_unpak = 0.01; // распаковка цен
constexpr double c_pak = 100; // упаковка цен

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct _offer
{
	int price;
	int number;

	bool operator!=(const _offer p) const { return (price != p.price) || (number != p.number); }
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct _offers
{
	_offer offer[size_offer];

	bool operator==(const _offers& p) const;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct _supply_and_demand // предложение и спрос
{
	_offers demand; // желающие купить
	_offers supply; // желающие продать
	time_t time;

	bool operator==(const _supply_and_demand& p) const; // время не учитывается при сравнении

	void clear()       noexcept { time = 0; } // метка пустого прайса
	bool empty() const noexcept { return (time == 0); } // проверка на пустоту 
	time_t time_to_minute() const { return time - (time % 60); } // обнулить секунды
	i64 time_hour();
	i64 time_minute();
};

