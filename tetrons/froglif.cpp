#include "froglif.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct _froglif_function : public _picture
{
	void froglif(_xy p, double r, uchar* f, int rf, uint c, uint c2);
};

void froglif(_picture& picture, _xy p, double r, uchar* f, int rf, uint c, uint c2)
{
	((_froglif_function*)&picture)->froglif(p, r, f, rf, c, c2);
}

//  1 -      1
//  2 -      3
//  3 -     11
//  4 -     43
//  5 -    171
//  6 -    683
//  7 -   2731
//  8 -  10923
//  9 -  43691
// 10 - 174763
// 11 - 699051
void _froglif_function::froglif(_xy p, double r, uchar* f, int rf, uint c, uint c2)
{
	if (r < 1) return; // нечего рисовать
	if (((c | c2) >> 24) == 0x00) return; // полностью прозрачный
	uint kk = 255 - (c >> 24);
	uint k2 = 256 - kk;
	uint c_1 = (c & 255);
	uint c_2 = ((c >> 8) & 255);
	uint c_3 = ((c >> 16) & 255);
	uint d1 = c_1 * k2;
	uint d2 = c_2 * k2;
	uint d3 = c_3 * k2;

	double k_a = 0.75; // коэффициент уменьшения толщины линий х.з какой лучше

	i64 x1 = (i64)p.x;
	i64 x2 = (i64)(p.x + r);
	i64 y1 = (i64)p.y;
	i64 y2 = (i64)(p.y + r);

	if (x1 < drawing_area.x.min) x1 = drawing_area.x.min;
	if (y1 < drawing_area.y.min) y1 = drawing_area.y.min;
	if (x2 >= drawing_area.x.max) x2 = drawing_area.x.max - 1;
	if (y2 >= drawing_area.y.max) y2 = drawing_area.y.max - 1;
	if ((x2 < x1) || (y2 < y1)) return;
	fill_rectangle(_iarea{ {x1 - 1, x2 + 2}, {y1 - 1, y2 + 2} }, c2);

	double aa = 1.0625;    // минимальная толщина линии
	double bb = 1.0625;    // минимальное пустое место
	double kk_aa = 1.0 / k_a; // коэффициент увеличения линии
	int    rf0 = 0;
	double sl1 = 2 * aa + aa / (kk_aa - 2);
	double sl2 = bb - aa / (kk_aa - 2);
	double ll = aa;
	while (sl1 * kk_aa + sl2 * 2 <= r)
	{
		rf0++;
		sl1 *= kk_aa;
		sl2 *= 2;
		ll *= kk_aa;
	}
	if (rf0 > rf) rf0 = rf;

	struct xxxyyy
	{
		int    ii;     // разреженные биты
		int    sm1;    // смещение ii до 1 на конце
		double l1, l2; // границы столбика
		uchar* f;      // начало нужного куска
		i64 x1, x2;    // реальные координаты начала-конца не выходящие за рамки
		uint kk_1, kk_2;                         // коэффициенты границ
		uint d1_1, d2_1, d3_1, d1_2, d2_2, d3_2; // коэффициенты границ
	};

	int rr = (1 << rf0);
	int rr_1 = rr - 1;

	const int Nxx = 17;
	if (rr + 1 > Nxx) return;
	static xxxyyy xx[Nxx]; // вектор размера rr+1
	static xxxyyy yy[Nxx]; // вектор размера rr+1

	int drf2 = (rf - rf0) * 2;

	for (int i = 0; i <= rr; i++)
	{
		xx[i].ii = ((i & 0x1) + ((i & 0x2) << 1) + ((i & 0x4) << 2) + ((i & 0x8) << 3) + ((i & 0x10) << 4) +
			((i & 0x20) << 5) + ((i & 0x40) << 6) + ((i & 0x80) << 7) + ((i & 0x100) << 8) +
			((i & 0x200) << 9) + ((i & 0x400) << 10) + ((i & 0x800) << 11) + ((i & 0x1000) << 12) +
			((i & 0x2000) << 13) + ((i & 0x4000) << 14))
			<< drf2;
	}

	int sm1 = rf * 2;

	xx[0].l1 = (double)p.x;
	xx[0].l2 = p.x + ll;
	xx[0].sm1 = sm1;
	xx[0].f = f;
	xx[0].x1 = (int)xx[0].l1;
	xx[0].x2 = (int)xx[0].l2;
	xx[rr].l1 = p.x + r - ll;
	xx[rr].l2 = p.x + r;
	xx[rr].sm1 = sm1;
	xx[rr].f = f;
	xx[rr].x1 = (int)xx[rr].l1;
	xx[rr].x2 = (int)xx[rr].l2;
	xxxyyy* xxx = &(xx[0]);
	if ((xxx->x1 <= x2) && (xxx->x2 >= x1))
	{
		if (xxx->l1 < x1)
		{
			xxx->x1 = x1;
			xxx->kk_1 = kk;
			xxx->d1_1 = d1;
			xxx->d2_1 = d2;
			xxx->d3_1 = d3;
		}
		else
		{
			uint k3 = (uint)(k2 * ((xxx->x1 + 1.0) - xxx->l1));
			xxx->kk_1 = 256 - k3;
			xxx->d1_1 = c_1 * k3;
			xxx->d2_1 = c_2 * k3;
			xxx->d3_1 = c_3 * k3;
		}
		if (xxx->x2 > x2)
		{
			xxx->x2 = x2;
			xxx->kk_2 = kk;
			xxx->d1_2 = d1;
			xxx->d2_2 = d2;
			xxx->d3_2 = d3;
		}
		else
		{
			uint k3 = (uint)(k2 * (xxx->l2 - xxx->x2));
			xxx->kk_2 = 256 - k3;
			xxx->d1_2 = c_1 * k3;
			xxx->d2_2 = c_2 * k3;
			xxx->d3_2 = c_3 * k3;
		}
	}
	xxx = &(xx[rr]);
	if ((xxx->x1 <= x2) && (xxx->x2 >= x1))
	{
		if (xxx->l1 < x1)
		{
			xxx->x1 = x1;
			xxx->kk_1 = kk;
			xxx->d1_1 = d1;
			xxx->d2_1 = d2;
			xxx->d3_1 = d3;
		}
		else
		{
			uint k3 = (uint)(k2 * ((xxx->x1 + 1.0) - xxx->l1));
			xxx->kk_1 = 256 - k3;
			xxx->d1_1 = c_1 * k3;
			xxx->d2_1 = c_2 * k3;
			xxx->d3_1 = c_3 * k3;
		}
		if (xxx->x2 > x2)
		{
			xxx->x2 = x2;
			xxx->kk_2 = kk;
			xxx->d1_2 = d1;
			xxx->d2_2 = d2;
			xxx->d3_2 = d3;
		}
		else
		{
			uint k3 = (uint)(k2 * (xxx->l2 - xxx->x2));
			xxx->kk_2 = 256 - k3;
			xxx->d1_2 = c_1 * k3;
			xxx->d2_2 = c_2 * k3;
			xxx->d3_2 = c_3 * k3;
		}
	}

	int    step = (rr >> 1);
	uchar* ff = f;
	int    dff = 1;
	while (step)
	{
		sm1 -= 2;
		ll *= k_a;
		for (int i = step; i <= rr; i += step * 2)
		{
			double  ce = (xx[i - step].l2 + xx[i + step].l1) * 0.5;
			xxx = &(xx[i]);
			xxx->l1 = ce - ll * 0.5;
			xxx->l2 = ce + ll * 0.5;
			xxx->sm1 = sm1;
			xxx->f = ff;
			xxx->x1 = (int)xxx->l1;
			xxx->x2 = (int)xxx->l2;
			if ((xxx->x1 > x2) || (xxx->x2 < x1)) continue;
			if (xxx->l1 < x1)
			{
				xxx->x1 = x1;
				xxx->kk_1 = kk;
				xxx->d1_1 = d1;
				xxx->d2_1 = d2;
				xxx->d3_1 = d3;
			}
			else
			{
				uint k3 = (uint)(k2 * ((xxx->x1 + 1.0) - xxx->l1));
				xxx->kk_1 = 256 - k3;
				xxx->d1_1 = c_1 * k3;
				xxx->d2_1 = c_2 * k3;
				xxx->d3_1 = c_3 * k3;
			}
			if (xxx->x2 > x2)
			{
				xxx->x2 = x2;
				xxx->kk_2 = kk;
				xxx->d1_2 = d1;
				xxx->d2_2 = d2;
				xxx->d3_2 = d3;
			}
			else
			{
				uint k3 = (uint)(k2 * (xxx->l2 - xxx->x2));
				xxx->kk_2 = 256 - k3;
				xxx->d1_2 = c_1 * k3;
				xxx->d2_2 = c_2 * k3;
				xxx->d3_2 = c_3 * k3;
			}
		}
		ff += dff;
		if (dff == 1)
			dff = 2;
		else
			dff *= 4;
		step >>= 1;
	}
	double dy = (double)(p.y - p.x);
	for (int i = 0; i <= rr; i++)
	{
		yy[i].f = xx[i].f;
		yy[i].ii = (xx[i].ii << 1);
		yy[i].sm1 = xx[i].sm1;
		yy[i].l1 = xx[i].l1 + dy;
		yy[i].l2 = xx[i].l2 + dy;
	}
	yy[0].l1 = (double)p.y;     // из-за маленькой дельточки возникла ошибка!
	yy[rr].l2 = p.y + r; // на всякий случай тоже

	int ay = 0;
	while (ay < rr)
	{
		if (y1 < yy[ay].l2) break;
		ay++;
	}
	int ax1 = 0;
	while (ax1 < rr)
	{
		if (x1 < xx[ax1].l2) break;
		ax1++;
	}

	xxxyyy* yyay_1 = &(yy[ay]);
	if (ay > 0) yyay_1 = &(yy[ay - 1]); //!!!! исправляет ошибку, но возможно что-то портит
	xxxyyy* yyay = &(yy[ay]);
	for (i64 y = y1; y <= y2; y++)
	{
		if (y >= yyay->l2)
		{
			ay++;
			if (ay > rr) break;
			yyay_1 = yyay;
			yyay = &(yy[ay]);
		}
		int ax = ax1;
		if (y + 1.0 <= yyay->l1) // рисуются только вертикальные линии
		{
			while (true)
			{
				// поиск рисуемой палочки
				while (ax <= rr)
				{
					uint ii;
					if (ax & (rr_1))
						ii = (xx[ax].ii + yy[ay - 1].ii) >> xx[ax].sm1; // ay не может быть равно 0
					else
						ii = 4 + 2 * (ax >> rf0); // первая и последняя палочка
					if (xx[ax].f[ii >> 3] & (uchar(1) << (ii & 7))) break;
					ax++;
				}
				if (ax > rr) break;
				xxxyyy* xxax = &(xx[ax]);
				i64 x11 = xxax->x1;
				if (x11 > x2) break;
				i64 x22 = xxax->x2;
				uchar* cc = (uchar*)&(data[y * size.x + x11]);
				cc[0] = (cc[0] * xxax->kk_1 + xxax->d1_1) >> 8;
				cc[1] = (cc[1] * xxax->kk_1 + xxax->d2_1) >> 8;
				cc[2] = (cc[2] * xxax->kk_1 + xxax->d3_1) >> 8;
				cc += 4;
				i64 d = x11 - x22 + 2;
				if (d <= 1)
				{
					while (d++ <= 0)
					{
						cc[0] = (cc[0] * kk + d1) >> 8;
						cc[1] = (cc[1] * kk + d2) >> 8;
						cc[2] = (cc[2] * kk + d3) >> 8;
						cc += 4;
					}
					cc[0] = (cc[0] * xxax->kk_2 + xxax->d1_2) >> 8;
					cc[1] = (cc[1] * xxax->kk_2 + xxax->d2_2) >> 8;
					cc[2] = (cc[2] * xxax->kk_2 + xxax->d3_2) >> 8;
				}
				ax++;
			}
			continue;
		}
		int stepy = 1 << ((yyay->sm1 - drf2) >> 1);
		int ayx = ax - 1;
		if (ayx < 0) ayx = 0;
		ayx &= (0xFFFFFFFF << ((yyay->sm1 - drf2) >> 1));
		if ((y == (int)yyay->l1) && (yyay->l1 > 0))
		{
			uint k3 = (uint)(k2 * ((y + 1.0) - yyay->l1));
			uint kkw = 256 - k3;
			uint d1w = c_1 * k3;
			uint d2w = c_2 * k3;
			uint d3w = c_3 * k3;
			while (true)
			{
				// поиск рисуемой палочки
				while (ayx < rr)
				{
					uint ii;
					if (ay & (rr_1))
						ii = (((xx[ayx].ii + yyay->ii) >> yyay->sm1) - 1) & 0xFFFFFFFE;
					else
						ii = 5 + 2 * (ay >> rf0); // первая и последняя палочка
					if (yyay->f[ii >> 3] & (uchar(1) << (ii & 7))) break;
					ayx += stepy;
				}
				int ayx_next = ayx + stepy; // следующая после серии палочек
				while (ayx_next < rr)
				{
					uint ii;
					if (ay & (rr_1))
						ii = (((xx[ayx_next].ii + yyay->ii) >> yyay->sm1) - 1) & 0xFFFFFFFE;
					else
						ii = 5 + 2 * (ay >> rf0); // первая и последняя палочка
					if ((yyay->f[ii >> 3] & (uchar(1) << (ii & 7))) == 0) break;
					ayx_next += stepy;
				}
				bool nac = true;
				bool kon = true;
				i64 x11, x22;
				if (ayx >= rr)
				{
					x11 = x2 + 1;
					x22 = x2; // ?? не нужно ??
				}
				else
				{
					x11 = xx[ayx].x1;
					x22 = xx[ayx_next].x2;
					if (x11 < x1)
					{
						x11 = x1;
						nac = false;
					}
					if (x22 > x2)
					{
						x22 = x2;
						kon = false;
					}
				}
				if (x11 > x1) // вертикальные хвостики
				{
					while (true)
					{
						// поиск рисуемой палочки
						bool vepa = false;
						while (ax <= rr)
						{
							xxxyyy* xxax = &(xx[ax]);
							uint    ii, iii;
							if (ax & rr_1)
							{
								if (ay & rr_1)
								{
									ii = (xxax->ii + yyay_1->ii) >> xxax->sm1;
									iii = (xxax->ii + yyay->ii) >> xxax->sm1;
								}
								else
								{
									if (ay)
										ii = (xxax->ii + yyay_1->ii) >> xxax->sm1;
									else
										ii = (xxax->ii + yyay->ii) >> xxax->sm1;
									iii = ii;
								}
							}
							else
							{
								ii = 4 + 2 * (ax >> rf0); // первая и последняя палочка
								iii = ii;
							}
							if (xxax->f[ii >> 3] & (uchar(1) << (ii & 7)))
							{
								if (ay) vepa = true;
								break;
							}
							if (xxax->f[iii >> 3] & (uchar(1) << (iii & 7))) break;
							ax++;
						}
						if (ax > rr) break;
						xxxyyy* xxax = &(xx[ax]);
						i64 xx11 = xxax->x1;
						if ((xx11 >= x11) || (xx11 > x2)) break;
						i64 xx22 = xxax->x2;
						uchar* cc = (uchar*)&(data[y * size.x + xx11]);
						if (vepa)
						{
							cc[0] = (cc[0] * xxax->kk_1 + xxax->d1_1) >> 8;
							cc[1] = (cc[1] * xxax->kk_1 + xxax->d2_1) >> 8;
							cc[2] = (cc[2] * xxax->kk_1 + xxax->d3_1) >> 8;
							cc += 4;
							i64 d = xx11 - xx22 + 2;
							if (d <= 1)
							{
								while (d++ <= 0)
								{
									cc[0] = (cc[0] * kk + d1) >> 8;
									cc[1] = (cc[1] * kk + d2) >> 8;
									cc[2] = (cc[2] * kk + d3) >> 8;
									cc += 4;
								}
								cc[0] = (cc[0] * xxax->kk_2 + xxax->d1_2) >> 8;
								cc[1] = (cc[1] * xxax->kk_2 + xxax->d2_2) >> 8;
								cc[2] = (cc[2] * xxax->kk_2 + xxax->d3_2) >> 8;
							}
						}
						else
						{
							k3 = ((256 - xxax->kk_1) * (256 - kkw) / k2);
							uint kkw2 = 256 - k3;
							cc[0] = (cc[0] * kkw2 + c_1 * k3) >> 8;
							cc[1] = (cc[1] * kkw2 + c_2 * k3) >> 8;
							cc[2] = (cc[2] * kkw2 + c_3 * k3) >> 8;
							cc += 4;
							i64 d = xx11 - xx22 + 2;
							if (d <= 1)
							{
								while (d++ <= 0)
								{
									cc[0] = (cc[0] * kkw + d1w) >> 8;
									cc[1] = (cc[1] * kkw + d2w) >> 8;
									cc[2] = (cc[2] * kkw + d3w) >> 8;
									cc += 4;
								}
								k3 = ((256 - xxax->kk_2) * (256 - kkw) / k2);
								kkw2 = 256 - k3;
								cc[0] = (cc[0] * kkw2 + c_1 * k3) >> 8;
								cc[1] = (cc[1] * kkw2 + c_2 * k3) >> 8;
								cc[2] = (cc[2] * kkw2 + c_3 * k3) >> 8;
							}
						}
						ax++;
					}
				}
				if (x11 > x2) break;
				uchar* cc = (uchar*)&(data[y * size.x + x11]);
				while (x11 <= x22)
				{
					// поиск рисуемой палочки
					if (ay == 0)
						ax = ayx_next + 1;
					else
						while (ax <= ayx_next)
						{
							xxxyyy* xxax = &(xx[ax]);
							uint    ii;
							if (ax & rr_1)
								ii = (xxax->ii + yyay_1->ii) >> xxax->sm1;
							else
								ii = 4 + 2 * (ax >> rf0); // первая и последняя палочка
							if (xxax->f[ii >> 3] & (uchar(1) << (ii & 7))) break;
							ax++;
						}
					xxxyyy* xxax = 0;
					i64 xx11, xx22 = 0; // для параноии компилятора
					if (ax <= rr)
					{
						xxax = &(xx[ax]);
						xx11 = xxax->x1;
						if (xx11 > x22) xx11 = x22 + 1;
						xx22 = xxax->x2;
					}
					else
					{
						xx11 = x22 + 1;
					}
					if (xx11 > x11)
					{
						i64 d = x11 - xx11 + 1;
						if (nac)
						{
							nac = false;
							d++;
							k3 = ((256 - xx[ayx].kk_1) * (256 - kkw) / k2);
							uint kkw2 = 256 - k3;
							cc[0] = (cc[0] * kkw2 + c_1 * k3) >> 8;
							cc[1] = (cc[1] * kkw2 + c_2 * k3) >> 8;
							cc[2] = (cc[2] * kkw2 + c_3 * k3) >> 8;
							cc += 4;
						}
						if (xx11 <= x22)
						{
							while (d++ <= 0)
							{
								cc[0] = (cc[0] * kkw + d1w) >> 8;
								cc[1] = (cc[1] * kkw + d2w) >> 8;
								cc[2] = (cc[2] * kkw + d3w) >> 8;
								cc += 4;
							}
						}
						else
						{
							if (d <= 0)
							{
								if (kon) d++;
								while (d++ <= 0)
								{
									cc[0] = (cc[0] * kkw + d1w) >> 8;
									cc[1] = (cc[1] * kkw + d2w) >> 8;
									cc[2] = (cc[2] * kkw + d3w) >> 8;
									cc += 4;
								}
								if (kon)
								{
									k3 = ((256 - xx[ayx_next].kk_2) * (256 - kkw) / k2);
									uint kkw2 = 256 - k3;
									cc[0] = (cc[0] * kkw2 + c_1 * k3) >> 8;
									cc[1] = (cc[1] * kkw2 + c_2 * k3) >> 8;
									cc[2] = (cc[2] * kkw2 + c_3 * k3) >> 8;
								}
							}
							break;
						}
					}
					uint sl1_, sl2_, kkw2;
					sl2_ = 256 - kkw;
					if (nac)
					{
						nac = false;
						cc[0] = (cc[0] * xxax->kk_1 + xxax->d1_1) >> 8;
						cc[1] = (cc[1] * xxax->kk_1 + xxax->d2_1) >> 8;
						cc[2] = (cc[2] * xxax->kk_1 + xxax->d3_1) >> 8;
					}
					else
					{
						sl1_ = 256 - xxax->kk_1;
						k3 = sl1_ + sl2_ - sl1_ * sl2_ / k2;
						kkw2 = 256 - k3;
						cc[0] = (cc[0] * kkw2 + c_1 * k3) >> 8;
						cc[1] = (cc[1] * kkw2 + c_2 * k3) >> 8;
						cc[2] = (cc[2] * kkw2 + c_3 * k3) >> 8;
					}
					cc += 4;
					i64 d = xx11 - xx22 + 2;
					if (d <= 1)
					{
						while (d++ <= 0)
						{
							cc[0] = (cc[0] * kk + d1) >> 8;
							cc[1] = (cc[1] * kk + d2) >> 8;
							cc[2] = (cc[2] * kk + d3) >> 8;
							cc += 4;
						}
						if ((xx22 >= x22) && kon)
						{
							cc[0] = (cc[0] * xxax->kk_2 + xxax->d1_2) >> 8;
							cc[1] = (cc[1] * xxax->kk_2 + xxax->d2_2) >> 8;
							cc[2] = (cc[2] * xxax->kk_2 + xxax->d3_2) >> 8;
						}
						else
						{
							sl1_ = 256 - xxax->kk_2;
							k3 = sl1_ + sl2_ - sl1_ * sl2_ / k2;
							kkw2 = 256 - k3;
							cc[0] = (cc[0] * kkw2 + c_1 * k3) >> 8;
							cc[1] = (cc[1] * kkw2 + c_2 * k3) >> 8;
							cc[2] = (cc[2] * kkw2 + c_3 * k3) >> 8;
						}
						cc += 4;
					}
					x11 = xx22 + 1;
					ax++;
				}
				if (x22 == x2) break;
				ayx = ayx_next;
			}
			continue;
		}
		if (y == (int)yyay->l2)
		{
			uint k3 = (uint)(k2 * (yyay->l2 - y));
			uint kkw = 256 - k3;
			uint d1w = c_1 * k3;
			uint d2w = c_2 * k3;
			uint d3w = c_3 * k3;
			while (true)
			{
				// поиск рисуемой палочки
				while (ayx < rr)
				{
					uint ii;
					if (ay & (rr_1))
						ii = (((xx[ayx].ii + yyay->ii) >> yyay->sm1) - 1) & 0xFFFFFFFE;
					else
						ii = 5 + 2 * (ay >> rf0); // первая и последняя палочка
					if (yyay->f[ii >> 3] & (uchar(1) << (ii & 7))) break;
					ayx += stepy;
				}
				int ayx_next = ayx + stepy; // следующая после серии палочек
				while (ayx_next < rr)
				{
					uint ii;
					if (ay & (rr_1))
						ii = (((xx[ayx_next].ii + yyay->ii) >> yyay->sm1) - 1) & 0xFFFFFFFE;
					else
						ii = 5 + 2 * (ay >> rf0); // первая и последняя палочка
					if ((yyay->f[ii >> 3] & (uchar(1) << (ii & 7))) == 0) break;
					ayx_next += stepy;
				}
				bool nac = true;
				bool kon = true;
				i64 x11, x22;
				if (ayx >= rr)
				{
					x11 = x2 + 1;
					x22 = x2; // ?? не нужно ??
				}
				else
				{
					x11 = xx[ayx].x1;
					x22 = xx[ayx_next].x2;
					if (x11 < x1)
					{
						x11 = x1;
						nac = false;
					}
					if (x22 > x2)
					{
						x22 = x2;
						kon = false;
					}
				}
				if (x11 > x1) // вертикальные хвостики
				{
					while (true)
					{
						// поиск рисуемой палочки
						bool nipa = false;
						while (ax <= rr)
						{
							xxxyyy* xxax = &(xx[ax]);
							uint    ii, iii;
							if (ax & rr_1)
							{
								if (ay & rr_1)
								{
									ii = (xxax->ii + yyay_1->ii) >> xxax->sm1;
									iii = (xxax->ii + yyay->ii) >> xxax->sm1;
								}
								else
								{
									if (ay)
										ii = (xxax->ii + yyay_1->ii) >> xxax->sm1;
									else
										ii = (xxax->ii + yyay->ii) >> xxax->sm1;
									iii = ii;
								}
							}
							else
							{
								ii = 4 + 2 * (ax >> rf0); // первая и последняя палочка
								iii = ii;
							}
							if (xxax->f[iii >> 3] & (uchar(1) << (iii & 7)))
							{
								if (ay < rr) nipa = true;
								break;
							}
							if (xxax->f[ii >> 3] & (uchar(1) << (ii & 7))) break;
							ax++;
						}
						if (ax > rr) break;
						xxxyyy* xxax = &(xx[ax]);
						i64 xx11 = xxax->x1;
						if ((xx11 >= x11) || (xx11 > x2)) break;
						i64 xx22 = xxax->x2;
						uchar* cc = (uchar*)&(data[y * size.x + xx11]);
						if (nipa)
						{
							cc[0] = (cc[0] * xxax->kk_1 + xxax->d1_1) >> 8;
							cc[1] = (cc[1] * xxax->kk_1 + xxax->d2_1) >> 8;
							cc[2] = (cc[2] * xxax->kk_1 + xxax->d3_1) >> 8;
							cc += 4;
							i64 d = xx11 - xx22 + 2;
							if (d <= 1)
							{
								while (d++ <= 0)
								{
									cc[0] = (cc[0] * kk + d1) >> 8;
									cc[1] = (cc[1] * kk + d2) >> 8;
									cc[2] = (cc[2] * kk + d3) >> 8;
									cc += 4;
								}
								cc[0] = (cc[0] * xxax->kk_2 + xxax->d1_2) >> 8;
								cc[1] = (cc[1] * xxax->kk_2 + xxax->d2_2) >> 8;
								cc[2] = (cc[2] * xxax->kk_2 + xxax->d3_2) >> 8;
							}
						}
						else
						{
							k3 = ((256 - xxax->kk_1) * (256 - kkw) / k2);
							uint kkw2 = 256 - k3;
							cc[0] = (cc[0] * kkw2 + c_1 * k3) >> 8;
							cc[1] = (cc[1] * kkw2 + c_2 * k3) >> 8;
							cc[2] = (cc[2] * kkw2 + c_3 * k3) >> 8;
							cc += 4;
							i64 d = xx11 - xx22 + 2;
							if (d <= 1)
							{
								while (d++ <= 0)
								{
									cc[0] = (cc[0] * kkw + d1w) >> 8;
									cc[1] = (cc[1] * kkw + d2w) >> 8;
									cc[2] = (cc[2] * kkw + d3w) >> 8;
									cc += 4;
								}
								k3 = ((256 - xxax->kk_2) * (256 - kkw) / k2);
								kkw2 = 256 - k3;
								cc[0] = (cc[0] * kkw2 + c_1 * k3) >> 8;
								cc[1] = (cc[1] * kkw2 + c_2 * k3) >> 8;
								cc[2] = (cc[2] * kkw2 + c_3 * k3) >> 8;
							}
						}
						ax++;
					}
				}
				if (x11 > x2) break;
				uchar* cc = (uchar*)&(data[y * size.x + x11]);
				while (x11 <= x22)
				{
					// поиск рисуемой палочки
					if (ay == rr)
						ax = ayx_next + 1;
					else
						while (ax <= ayx_next)
						{
							xxxyyy* xxax = &(xx[ax]);
							uint    ii;
							if (ax & rr_1)
								ii = (xxax->ii + yyay->ii) >> xxax->sm1;
							else
								ii = 4 + 2 * (ax >> rf0); // первая и последняя палочка
							if (xxax->f[ii >> 3] & (uchar(1) << (ii & 7))) break;
							ax++;
						}
					xxxyyy* xxax = 0;
					i64 xx11, xx22 = 0; // для параноии компилятора
					if (ax <= rr)
					{
						xxax = &(xx[ax]);
						xx11 = xxax->x1;
						if (xx11 > x22) xx11 = x22 + 1;
						xx22 = xxax->x2;
					}
					else
					{
						xx11 = x22 + 1;
					}
					if (xx11 > x11)
					{
						i64 d = x11 - xx11 + 1;
						if (nac)
						{
							nac = false;
							d++;
							k3 = ((256 - xx[ayx].kk_1) * (256 - kkw) / k2);
							uint kkw2 = 256 - k3;
							cc[0] = (cc[0] * kkw2 + c_1 * k3) >> 8;
							cc[1] = (cc[1] * kkw2 + c_2 * k3) >> 8;
							cc[2] = (cc[2] * kkw2 + c_3 * k3) >> 8;
							cc += 4;
						}
						if (xx11 <= x22)
						{
							while (d++ <= 0)
							{
								cc[0] = (cc[0] * kkw + d1w) >> 8;
								cc[1] = (cc[1] * kkw + d2w) >> 8;
								cc[2] = (cc[2] * kkw + d3w) >> 8;
								cc += 4;
							}
						}
						else
						{
							if (d <= 0)
							{
								if (kon) d++;
								while (d++ <= 0)
								{
									cc[0] = (cc[0] * kkw + d1w) >> 8;
									cc[1] = (cc[1] * kkw + d2w) >> 8;
									cc[2] = (cc[2] * kkw + d3w) >> 8;
									cc += 4;
								}
								if (kon)
								{
									k3 = ((256 - xx[ayx_next].kk_2) * (256 - kkw) / k2);
									uint kkw2 = 256 - k3;
									cc[0] = (cc[0] * kkw2 + c_1 * k3) >> 8;
									cc[1] = (cc[1] * kkw2 + c_2 * k3) >> 8;
									cc[2] = (cc[2] * kkw2 + c_3 * k3) >> 8;
								}
							}
							break;
						}
					}
					uint sl1_, sl2_, kkw2;
					sl2_ = 256 - kkw;
					if (xxax == nullptr) break; // для параноии компилятора
					if (nac)
					{
						nac = false;
						cc[0] = (cc[0] * xxax->kk_1 + xxax->d1_1) >> 8;
						cc[1] = (cc[1] * xxax->kk_1 + xxax->d2_1) >> 8;
						cc[2] = (cc[2] * xxax->kk_1 + xxax->d3_1) >> 8;
					}
					else
					{
						sl1_ = 256 - xxax->kk_1;
						k3 = sl1_ + sl2_ - sl1_ * sl2_ / k2;
						kkw2 = 256 - k3;
						cc[0] = (cc[0] * kkw2 + c_1 * k3) >> 8;
						cc[1] = (cc[1] * kkw2 + c_2 * k3) >> 8;
						cc[2] = (cc[2] * kkw2 + c_3 * k3) >> 8;
					}
					cc += 4;
					i64 d = xx11 - xx22 + 2;
					if (d <= 1)
					{
						while (d++ <= 0)
						{
							cc[0] = (cc[0] * kk + d1) >> 8;
							cc[1] = (cc[1] * kk + d2) >> 8;
							cc[2] = (cc[2] * kk + d3) >> 8;
							cc += 4;
						}
						if ((xx22 >= x22) && kon)
						{
							cc[0] = (cc[0] * xxax->kk_2 + xxax->d1_2) >> 8;
							cc[1] = (cc[1] * xxax->kk_2 + xxax->d2_2) >> 8;
							cc[2] = (cc[2] * xxax->kk_2 + xxax->d3_2) >> 8;
						}
						else
						{
							sl1_ = 256 - xxax->kk_2;
							k3 = sl1_ + sl2_ - sl1_ * sl2_ / k2;
							kkw2 = 256 - k3;
							cc[0] = (cc[0] * kkw2 + c_1 * k3) >> 8;
							cc[1] = (cc[1] * kkw2 + c_2 * k3) >> 8;
							cc[2] = (cc[2] * kkw2 + c_3 * k3) >> 8;
						}
						cc += 4;
					}
					x11 = xx22 + 1;
					ax++;
				}
				if (x22 == x2) break;
				ayx = ayx_next;
			}
			continue;
		}
		while (true)
		{
			// поиск рисуемой палочки
			while (ayx < rr)
			{
				uint ii;
				if (ay & (rr_1))
					ii = (((xx[ayx].ii + yyay->ii) >> yyay->sm1) - 1) & 0xFFFFFFFE;
				else
					ii = 5 + 2 * (ay >> rf0); // первая и последняя палочка
				if (yyay->f[ii >> 3] & (uchar(1) << (ii & 7))) break;
				ayx += stepy;
			}
			int ayx_next = ayx + stepy; // следующая после серии палочек
			while (ayx_next < rr)
			{
				uint ii;
				if (ay & (rr_1))
					ii = (((xx[ayx_next].ii + yyay->ii) >> yyay->sm1) - 1) & 0xFFFFFFFE;
				else
					ii = 5 + 2 * (ay >> rf0); // первая и последняя палочка
				if ((yyay->f[ii >> 3] & (uchar(1) << (ii & 7))) == 0) break;
				ayx_next += stepy;
			}
			bool nac = true;
			bool kon = true;
			i64 x11, x22;
			if (ayx >= rr)
			{
				x11 = x2 + 1;
				x22 = x2; // ?? не нужно ??
			}
			else
			{
				x11 = xx[ayx].x1;
				x22 = xx[ayx_next].x2;
				if (x11 < x1)
				{
					x11 = x1;
					nac = false;
				}
				if (x22 > x2)
				{
					x22 = x2;
					kon = false;
				}
			}
			if (x11 > x1) // вертикальные хвостики x222
			{
				while (true)
				{
					// поиск рисуемой палочки
					while (ax <= rr)
					{
						xxxyyy* xxax = &(xx[ax]);
						uint    ii, iii;
						if (ax & rr_1)
						{
							if (ay & rr_1)
							{
								ii = (xxax->ii + yyay_1->ii) >> xxax->sm1;
								iii = (xxax->ii + yyay->ii) >> xxax->sm1;
							}
							else
							{
								if (ay)
									ii = (xxax->ii + yyay_1->ii) >> xxax->sm1;
								else
									ii = (xxax->ii + yyay->ii) >> xxax->sm1;
								iii = ii;
							}
						}
						else
						{
							ii = 4 + 2 * (ax >> rf0); // первая и последняя палочка
							iii = ii;
						}
						if (xxax->f[ii >> 3] & (uchar(1) << (ii & 7))) break;
						if (xxax->f[iii >> 3] & (uchar(1) << (iii & 7))) break;
						ax++;
					}
					if (ax > rr) break;
					xxxyyy* xxax = &(xx[ax]);
					i64 xx11 = xxax->x1;
					if ((xx11 >= x11) || (xx11 > x2)) break;
					i64 xx22 = xxax->x2;
					uchar* cc = (uchar*)&(data[y * size.x + xx11]);
					cc[0] = (cc[0] * xxax->kk_1 + xxax->d1_1) >> 8;
					cc[1] = (cc[1] * xxax->kk_1 + xxax->d2_1) >> 8;
					cc[2] = (cc[2] * xxax->kk_1 + xxax->d3_1) >> 8;
					cc += 4;
					i64 d = xx11 - xx22 + 2;
					if (d <= 1)
					{
						while (d++ <= 0)
						{
							cc[0] = (cc[0] * kk + d1) >> 8;
							cc[1] = (cc[1] * kk + d2) >> 8;
							cc[2] = (cc[2] * kk + d3) >> 8;
							cc += 4;
						}
						cc[0] = (cc[0] * xxax->kk_2 + xxax->d1_2) >> 8;
						cc[1] = (cc[1] * xxax->kk_2 + xxax->d2_2) >> 8;
						cc[2] = (cc[2] * xxax->kk_2 + xxax->d3_2) >> 8;
					}
					ax++;
				}
			}
			if (x11 > x2) break;
			uchar* cc = (uchar*)&(data[y * size.x + x11]);
			i64 d = x11 - x22 + 1;
			xxxyyy* xxax = &(xx[ayx]);
			if (nac)
			{
				d++;
				cc[0] = (cc[0] * xxax->kk_1 + xxax->d1_1) >> 8;
				cc[1] = (cc[1] * xxax->kk_1 + xxax->d2_1) >> 8;
				cc[2] = (cc[2] * xxax->kk_1 + xxax->d3_1) >> 8;
				cc += 4;
			}
			if (d <= 1)
			{
				if (!kon) d--;
				while (d++ <= 0)
				{
					cc[0] = (cc[0] * kk + d1) >> 8;
					cc[1] = (cc[1] * kk + d2) >> 8;
					cc[2] = (cc[2] * kk + d3) >> 8;
					cc += 4;
				}
				if (kon)
				{
					xxax = &(xx[ayx_next]);
					cc[0] = (cc[0] * xxax->kk_2 + xxax->d1_2) >> 8;
					cc[1] = (cc[1] * xxax->kk_2 + xxax->d2_2) >> 8;
					cc[2] = (cc[2] * xxax->kk_2 + xxax->d3_2) >> 8;
				}
			}
			if (x22 == x2) break;
			ayx = ayx_next;
			ax = ayx + 1;
		}
	}
}

