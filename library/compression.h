#pragma once

#include <vector>

#include "basic.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

double entropy(std::vector<uchar>& a); // простая энтропия в байтах
 // количество информации в байтах сочетания + частоты
double information(std::vector<uchar>& a, double* permutations = nullptr, double* frequency = nullptr);
double size_arithmetic_coding(std::vector<uchar>& a); // идеальный размер арифметика = information

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void arithmetic_coding(const std::vector<uchar>& data, _bit_vector &res);
void arithmetic_decoding(_bit_vector& data, std::vector<uchar>& res);

uchar ppm(const std::vector<uchar>& data, std::vector<uchar>& res, u64 g = 0);

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
