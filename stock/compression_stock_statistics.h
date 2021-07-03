#pragma once

#include "stock_basic.h"

struct _compression_stock_statistics
{
	i64 size = 0; // ���������� �������
	_bit_vector data; // ������ ������

	bool add(const _supply_and_demand& c); // �������� ���� (�����)
	bool read(_supply_and_demand& c); // ��������� ���� (�������)
	void push_to(_stack& mem);
	void pop_from(_stack& mem);

private:
	std::vector<_offer> base_buy; // ���� ������� (������ 20 - ��������� ����)
	std::vector<_offer> base_sale; // ���� ������� (������ 20 - ��������� ����)
	time_t back_time = 0; // ����� ����������� ���

	bool add0(const _supply_and_demand& c); // �� ������!
	bool add1(const _supply_and_demand& c); // ������
	bool add12(const _offer* v1, std::vector<_offer>& v0, i64 izm);
	bool coding_delta_number(i64 a, i64 b);
	i64  decoding_delta_number(i64 a);
	bool read0(_supply_and_demand& c);
	bool read1(_supply_and_demand& c);
	bool read12(_offer* v1, std::vector<_offer>& v0);
};

