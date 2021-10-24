#pragma once

#include <vector>

#include "basic.h"
#include "bit_vector.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct _frequency2
{
	i64 frequency[256] = {};
	i64 number = 256;

	i64 size();
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void stir_vector(std::vector<uchar>& v); // перемешать
std::vector<uchar> generate_vector(_frequency2& f);

double entropy(std::vector<uchar>& a); // простая энтропия в байтах
 // количество информации в байтах сочетания + частоты
double information(std::vector<uchar>& a, double* permutations = nullptr, double* frequency = nullptr);
double size_arithmetic_coding(std::vector<uchar>& a, double frequency0 = 1.0); // идеальный размер арифметика

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void arithmetic_coding(const std::vector<uchar>& data, _bit_vector &res);
void arithmetic_decoding(_bit_vector& data, std::vector<uchar>& res);

uchar ppm(const std::vector<uchar>& data, std::vector<uchar>& res, u64 g = 0);

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
