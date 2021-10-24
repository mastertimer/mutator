#pragma once

#include "basic.h"

struct _bit_vector
{
	std::vector<u64> data;
	uchar bit = 64; // ������� ��� ��������� � ��������� �����
	i64 bit_read = 0; // ������� ���� ��� ������

	void push1(u64 a) noexcept; // �������� 1 ���
	void pushn(u64 a, uchar n) noexcept; // �������� n ���
	void pushnod(u64 a, u64 n) noexcept; // �������� n ���������� ���
	void pushn1(u64 a) noexcept; // �������� ������������ ���������� ���, 1xxxxxxxx
	u64 pop1() noexcept; // ��������� 1 ���
	u64 pop1_safely() noexcept; // ��������� 1 ��� (���������)
	u64 popn(uchar n) noexcept; // ��������� n ���
	i64 size() const noexcept { return (i64)data.size() * 64 - (64 - bit); } // � �����!!!
	bool empty() const noexcept { return (size() == 0); }
	void resize(i64 v);
	void clear() { data.clear(); bit = 64; bit_read = 0; }

	void save(_stack& mem);
	void load(_stack& mem);
};
