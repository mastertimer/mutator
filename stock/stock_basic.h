#pragma once

#include "basic.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

constexpr i64 roffer = 20; // ����������� �������, ����������� ������� ( ����� = roffer * 2 );

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
	_offer offer[roffer];

	bool operator==(const _offers& p) const;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct _supply_and_demand // ����������� � �����
{
	_offers demand; // �������� ������
	_offers supply; // �������� �������
	time_t time;

	bool operator==(const _supply_and_demand& p) const; // ����� �� ����������� ��� ���������

	void clear()       noexcept { time = 0; } // ����� ������� ������
	bool empty() const noexcept { return (time == 0); } // �������� �� ������� 
	time_t time_to_minute() const { return time - (time % 60); } // �������� �������
	i64 time_hour();
	i64 time_minute();
};

