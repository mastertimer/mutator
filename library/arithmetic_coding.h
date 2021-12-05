#pragma once

#include "bit_vector.h"

void arithmetic_coding(const std::vector<uchar>& data, _bit_vector& res);
void arithmetic_decoding(_bit_vector& data, std::vector<uchar>& res);
