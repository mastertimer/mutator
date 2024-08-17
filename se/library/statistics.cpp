#include "statistics.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void _basic_statistics::push(i64 x)
{
	if (data.empty()) start = x;
	if (x - start >= (i64)data.size()) data.resize(x - start + 1, 0);
	if (x < start) { data.insert(data.begin(), start - x, 0); start = x; }
	data[x - start]++;
}

i64 _basic_statistics::number() const noexcept
{
	i64 s = 0;
	for (auto i : data) s += i;
	return s;
}

i64 _basic_statistics::number(i64 be, i64 en) const noexcept
{
	be -= start;
	en -= start;
	if (be < 0) be = 0;
	if (en > (i64)data.size()) en = (i64)data.size();
	i64 s = 0;
	for (i64 i = be; i < en; i++) s += data[i];
	return s;
}

i64 _basic_statistics::operator[](i64 x) const noexcept
{
	if (x - start >= (i64)data.size()) return 0;
	if (x < start) return 0;
	return data[x - start];
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

double _interval_statistics::min_mean()
{
	if (element.empty()) return 0;
	return element.front().mean;
}

double _interval_statistics::max_mean()
{
	if (element.empty()) return 0;
	return element.back().mean;
}

void _interval_statistics::init_equiprobable(_basic_statistics& bs, i64 n, double k)
{
	element.resize(n);
	i64 nbs = bs.number();
	i64 ii = 0;
	i64 s = 0;
	for (i64 i = 0; i < n; i++)
	{
		i64 ss = nbs * (i + 1) / n;
		i64 s_p = s;
		i64 ii_p = i;
		double mean = 0;
		while ((s < ss) && (ii < (i64)bs.data.size()))
		{
			mean += ((ii + bs.start) * k) * bs.data[ii];
			s += bs.data[ii];
			ii++;
		}
		element[i].interval = { (ii_p + bs.start - 0.5) * k, (ii + bs.start - 0.5) * k };
		if (s - s_p) mean /= (s - s_p);
		element[i].mean = mean;
		element[i].number = s - s_p;
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

i64 _statistics::number(i64 be, i64 en) const noexcept
{
	auto li1 = std::lower_bound(data.begin(), data.end(), be, [](_one_stat a, i64 b) { return (a.value < b); });
	auto li2 = std::lower_bound(data.begin(), data.end(), en, [](_one_stat a, i64 b) { return (a.value < b); });
	return number(li1, li2);
}

i64 _statistics::number(_cit be, _cit en) const noexcept
{
	i64 s = 0;
	for (auto i = be; i != en; ++i) s += i->number;
	return s;
}

bool _statistics::operator==(const _statistics& a) const noexcept
{
	if (data.size() != a.data.size()) return false;
	for (i64 i = 0; i < (i64)data.size(); i++)
		if (data[i] != a.data[i]) return false;
	return true;
}

void _statistics::operator+=(const _statistics& a)
{
	_statistics res;
	auto i = data.begin();
	auto ai = a.data.begin();
	while ((i != data.end()) && (ai != a.data.end()))
	{
		if (i->value < ai->value)
		{
			res.data.push_back(*i);
			++i;
			continue;
		}
		if (ai->value < i->value)
		{
			res.data.push_back(*ai);
			++ai;
			continue;
		}
		res.data.push_back({ i->value, i->number + ai->number });
		++i;
		++ai;
	}
	for (; i != data.end(); ++i) res.data.push_back(*i);
	for (; ai != a.data.end(); ++ai) res.data.push_back(*ai);
	*this = res;
}

void _statistics::operator=(const _basic_statistics& a)
{
	i64 k = 0;
	for (auto i : a.data) if (i) k++;
	data.clear();
	data.reserve(k);
	for (i64 i = 0; i < (i64)a.data.size(); i++)
		if (a.data[i]) data.push_back({ a.start + i, a.data[i] });
}

i64 _statistics::first_zero()
{
	for (i64 i = 1; i < (i64)data.size(); i++)
		if (data[i].value != data[i - 1].value + 1)
			return data[i - 1].value + 1;
	return -1;
}

double _statistics::arithmetic_size(_it be, _it en)
{
	double s = 0;
	double n = 1.0 / number(be, en);
	for (auto i = be; i != en; ++i) s += i->number * log(i->number * n);
	return -s / log(2.0);
}

_matrix _statistics::to_matrix()
{
	_matrix res(data.size(), 2);
	for (i64 i = 0; i < (i64)data.size(); i++)
	{
		res[i][0] = data[i].value;
		res[i][1] = data[i].number;
	}
	return res;
}

_matrix _statistics::to_matrix(i64 mi, i64 ma)
{
	i64 n;
	i64 k = data.size() - 1;
	for (n = 0; n <= k; n++) if (data[n].value >= mi) break;
	for (; k >= 0; k--) if (data[k].value <= ma) break;
	if (k < n) return {};
	_matrix res(k - n + 1, 2);
	for (i64 i = n; i <= k; i++)
	{
		res[i - n][0] = data[i].value;
		res[i - n][1] = data[i].number;
	}
	return res;
}

i64 _statistics::operator[](i64 n) const noexcept
{
	auto li = std::lower_bound(data.begin(), data.end(), n, [](_one_stat a, i64 b) { return (a.value < b); });
	if (li == data.end()) return 0;
	return (li->value == n) ? li->number : 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
