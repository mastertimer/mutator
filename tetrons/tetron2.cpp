#include "tetron2.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void _g_graph::add(const _matrix& b, std::string_view s, bool bar)
{
	curve.push_back({ b, std::string(s), bar });
}

void _g_graph::ris2(_trans tr, bool final)
{
	constexpr double ot = 0.03; // отступ от каждой стороны
	_area a = tr(local_area);
	master_bm.rectangle(a, c_def);

	unsigned int col_setka = c_min - 0xA0000000; // цвет сетки
	unsigned int col_font = c_max - 0x80000000; // цвет шрифта

	// вычисление диапазона
	double minx = 1;
	double maxx = 0;
	double miny = 1;
	double maxy = 0;

	for (auto& i : curve)
	{
		_matrix* c = &i.a;
		if ((c->size.x < 1) || (c->size.x > 2)) continue; // должно быть 1-2 столбца
		double min, max;
		if (c->size.x == 2)
			c->column(0).min_max(&min, &max);
		else
		{
			min = 0;
			max = c->size.y - 1;
		}
		if (minx > maxx)
		{
			minx = min;
			maxx = max;
		}
		else
		{
			if (min < minx) minx = min;
			if (max > maxx) maxx = max;
		}
		if (c->size.x == 2)
			c->column(1).min_max(&min, &max);
		else
			c->column(0).min_max(&min, &max);
		if (i.bar)
		{
			if (max < 0) max = 0;
			if (min > 0) min = 0;
		}
		if (miny > maxy)
		{
			miny = min;
			maxy = max;
		}
		else
		{
			if (min < miny) miny = min;
			if (max > maxy) maxy = max;
		}
	}
	if ((minx > maxx) || (miny > maxy)) return;
	double dx = maxx - minx;
	if (dx == 0) dx = 1;
	double dy = maxy - miny;
	if (dy == 0) dy = 1;
	minx -= dx * ot;
	maxx += dx * ot;
	miny -= dy * ot;
	maxy += dy * ot;
	// рисование графиков
	double kx = a.x.length() / (maxx - minx);
	double ky = a.y.length() / (maxy - miny);
	int ng = 0;
	double delta_bar = 0;
	for (auto& j : curve)
	{
		_matrix* c = &j.a;
		if ((c->size.x < 1) || (c->size.x > 2)) continue; // должно быть 1-2 столбца
		double xpr = 0;
		double ypr = 0;
		uint cc = (ng < _countof(color_set)) ? color_set[ng] : c_max;
		//		cc -= 0x80000000;
		ng++;
		for (int i = 0; i < c->size.y; i++)
		{
			double x, y;
			if (c->size.x == 2)
			{
				x = (*c)[i][0];
				y = (*c)[i][1];
			}
			else
			{
				x = i;
				y = (*c)[i][0];
			}
			double xx = (a.x.min + (x - minx) * kx);
			double yy = (a.y.max - (y - miny) * ky);
			if (j.bar)
				master_bm.lines({ xx + delta_bar, (a.y.max + miny * ky) }, { xx + delta_bar, yy }, j.width, cc);
			else
				if (i > 0) master_bm.lines({ xpr, ypr }, { xx, yy }, j.width, cc);
			xpr = xx;
			ypr = yy;
		}
		if (j.bar) delta_bar += j.width;
	}
	// рисование осей
	if (a.y.length() > 10)
	{
		int maxN = a.x.length() / 9; // 1-е приближение
		if (maxN > 1)
		{
			double mi, step;
			os_pordis(minx, maxx, maxN, mi, step);
			int zn = (int)(-log10(step * 1.1) + 1);
			if (zn < 0) zn = 0;
			i64 dex = std::max(master_bm.size_text16(double_to_astring(minx, zn)).x,
				master_bm.size_text16(double_to_astring(maxx, zn)).x) + 4;
			maxN = a.x.length() / dex; // 2-е приближение
			if (maxN > 1)
			{
				os_pordis(minx, maxx, maxN, mi, step);
				zn = (int)(-log10(step * 1.1) + 1);
				if (zn < 0) zn = 0;
				for (double x = mi; x < maxx; x += step)
				{
					double xx = (x - minx) * kx;
					master_bm.line({ i64(xx + a.x.min), (i64)a.y.min }, { i64(xx + a.x.min), (i64)a.y.max }, col_setka);
					std::string s = double_to_astring(x, zn);
					_isize l = master_bm.size_text16(s);
					if (xx < a.x.length() - 50)	master_bm.text16(a.x.min + xx - l.x / 2, std::max(a.y.min, 0.0),
						s, col_font);
					if (xx > 50) master_bm.text16(a.x.min + xx - l.x / 2, std::min((i64)a.y.max, master_bm.size.y) - 13,
						s, col_font);
				}
			}
		}
	}
	if (a.x.length() > 10)
	{
		//		int dex = 35; // длина подписи
		int maxN = a.y.length() / 15;
		if (maxN > 1)
		{
			double mi, step;
			os_pordis(miny, maxy, maxN, mi, step);
			int zn = (int)(-log10(step * 1.1) + 1);
			if (zn < 0) zn = 0;
			for (double y = mi; y < maxy; y += step)
			{
				double yy = (y - miny) * ky;
				master_bm.line({ (i64)a.x.min, i64(a.y.max - yy) }, { (i64)a.x.max, i64(a.y.max - yy) }, col_setka);
				std::string s = double_to_astring(y, zn);
				_isize l = master_bm.size_text16(s);
				if (a.y.length() - yy > 16) master_bm.text16(std::max(a.x.min, 0.0) + 2, a.y.max - yy - 6, s, col_font);
				if (yy > 16) master_bm.text16(std::min((i64)a.x.max, master_bm.size.x) - l.x - 2, a.y.max - yy - 6,
					s, col_font);
			}
		}
	}
	if ((a.x.length() > 50) && (a.y.length() > 50))
	{
		i64 y = std::max(a.y.min, 0.0);
		i64 x = std::min((i64)a.x.max, master_bm.size.x) - 50;
		ng = -1;
		for (auto& j : curve)
		{
			ng++;
			if (j.caption == "") continue;
			y += 16;
			uint cc = (ng < _countof(color_set)) ? color_set[ng] : c_max;
			master_bm.text16(x, y, j.caption, cc);
			master_bm.lines({ x - 20.0, y + 6.0 }, { x - 4.0, y + 6.0 }, j.width, cc);
		}

	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

