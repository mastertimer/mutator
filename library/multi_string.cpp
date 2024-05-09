#include "multi_string.h"

void _multi_string::div2line(i64 y, i64 x)
{
	if ((y < 0) || (y >= (int)line.size())) return;
	int l = (int)line[y].size();
	if (x < 0) x = 0;
	if (x > l) x = l;
	line.insert(line.begin() + y, line[y]);
	line[y].erase(x);
	line[y + 1LL].erase(0, x);
}

void _multi_string::insert_char(i64 y, i64 x, wchar_t c)
{
	if ((y < 0) || (y >= (int)line.size())) return;
	if (x < 0) x = 0;
	int l = (int)line[y].size();
	if (x > l) x = l;
	line[y].insert(x, 1, c);
}

bool _multi_string::delete_char(i64 y, i64 x)
{
	if ((y < 0) || (y >= (int)line.size())) return false;
	if (x < 0)
	{
		if (y == 0) return false;
		line[y - 1LL] += line[y];
		line.erase(line.begin() + y);
		return true;
	}
	if (x >= (int)line[y].size())
	{
		if (y == (i64)line.size() - 1) return false;
		line[y] += line[y + 1LL];
		line.erase(line.begin() + (y + 1ULL));
		return true;
	}
	line[y].erase(x, 1);
	return true;
}
