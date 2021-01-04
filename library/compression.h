#pragma once

#include <vector>

#include "basic.h"

void arithmetic_coding(const std::vector<uchar>& data, std::vector<uchar>& res);
void arithmetic_decoding(const std::vector<uchar>& data, std::vector<uchar>& res);

void arithmetic_coding2(const std::vector<uchar>& data, std::vector<uchar>& res);

uchar ppm(const std::vector<uchar>& data, std::vector<uchar>& res, u64 g = 0);

void AC_pak(const std::vector<uchar>& data, std::vector<uchar>& res);
void AC_pak64(const std::vector<uchar>& data, std::vector<uchar>& res);
std::vector<unsigned char> AC_unpak(std::vector<unsigned char>& A);
