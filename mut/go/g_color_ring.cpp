#include "g_color_ring.h"

void _g_color_ring::ris2(_trans tr, bool final)
{
	const double toll = 2.0;
	_area_old a = tr(local_area);
	double r = a.radius();
	_xy c = a.center();
	master_bm.ring(a.center(), r, r * 0.04, c_def);
	master_bm.fill_circle(c.x, c.y, r * 0.3, color);
	double r1 = 0.4 * r;
	double r2 = 0.5 * r;
	double r1_ = li_r_1 * r; // для ползунка
	double r2_ = li_r_2 * r; // для ползунка
	double dk = 1.0 / ((pi * 0.5 - zazor) * r2);
	for (double k = 0; k <= 1; k += dk)
	{
		double alpha = zazor * 0.5 + (pi * 0.5) * 0 + (pi * 0.5 - zazor) * k;
		_hsva cv = color;
		cv.h = k * 360;
		master_bm.lines(c + _xy{ r1 * cos(alpha), -r1 * sin(alpha) }, c + _xy{ r2 * cos(alpha), -r2 * sin(alpha) }, toll, cv);
	}
	for (double k = 0; k <= 1; k += dk)
	{
		double alpha = zazor * 0.5 + (pi * 0.5) * 1 + (pi * 0.5 - zazor) * k;
		_hsva cv = color;
		cv.s = k;
		master_bm.lines(c + _xy{ r1 * cos(alpha), -r1 * sin(alpha) }, c + _xy{ r2 * cos(alpha), -r2 * sin(alpha) }, toll, cv);
	}
	for (double k = 0; k <= 1; k += dk)
	{
		double alpha = zazor * 0.5 + (pi * 0.5) * 2 + (pi * 0.5 - zazor) * k;
		_hsva cv = color;
		cv.v = k;
		master_bm.lines(c + _xy{ r1 * cos(alpha), -r1 * sin(alpha) }, c + _xy{ r2 * cos(alpha), -r2 * sin(alpha) }, toll, cv);
	}
	for (double k = 0; k <= 1; k += dk)
	{
		double alpha = zazor * 0.5 + (pi * 0.5) * 3 + (pi * 0.5 - zazor) * k;
		_hsva cv = color;
		cv.a = k;
		master_bm.lines(c + _xy{ r1 * cos(alpha), -r1 * sin(alpha) }, c + _xy{ r2 * cos(alpha), -r2 * sin(alpha) }, toll, cv);
	}
	double k = color.h / 360.0;
	double alpha = zazor * 0.5 + (pi * 0.5) * 0 + (pi * 0.5 - zazor) * k;
	master_bm.lines(c + _xy{ r1_ * cos(alpha), -r1_ * sin(alpha) }, c + _xy{ r2_ * cos(alpha), -r2_ * sin(alpha) }, 1.2, 0xFFFFFFFF);
	k = color.s;
	alpha = zazor * 0.5 + (pi * 0.5) * 1 + (pi * 0.5 - zazor) * k;
	master_bm.lines(c + _xy{ r1_ * cos(alpha), -r1_ * sin(alpha) }, c + _xy{ r2_ * cos(alpha), -r2_ * sin(alpha) }, 1.2, 0xFFFFFFFF);
	k = color.v;
	alpha = zazor * 0.5 + (pi * 0.5) * 2 + (pi * 0.5 - zazor) * k;
	master_bm.lines(c + _xy{ r1_ * cos(alpha), -r1_ * sin(alpha) }, c + _xy{ r2_ * cos(alpha), -r2_ * sin(alpha) }, 1.2, 0xFFFFFFFF);
	k = color.a;
	alpha = zazor * 0.5 + (pi * 0.5) * 3 + (pi * 0.5 - zazor) * k;
	master_bm.lines(c + _xy{ r1_ * cos(alpha), -r1_ * sin(alpha) }, c + _xy{ r2_ * cos(alpha), -r2_ * sin(alpha) }, 1.2, 0xFFFFFFFF);
}

void _g_color_ring::change(_xy p, bool start)
{
	double r = local_area.radius();
	_xy c = local_area.center();
	_xy v = p - c;
	double rr = v.len() / r;
	if ((rr > li_r_1) && (rr < li_r_2))
	{
		double alpha;
		int i;
		if (start)
		{
			alpha = atan2(-v.y, v.x);
			if (alpha < 0) alpha += 2 * pi;
			i = (int)(alpha / (pi / 2));
			iii = i;
		}
		else
		{
			alpha = atan2(-v.y, v.x);
			i = iii;
			if (((alpha < 0) && (i != 0)) || (i == 3)) alpha += 2 * pi;
		}
		alpha -= (pi / 2) * i;
		double k = (alpha - (zazor * 0.5)) / (pi / 2 - zazor);
		if (k < 0) k = 0;
		if (k > 1) k = 1;
		switch (i)
		{
		case 0: color.h = 360 * k; break;
		case 1: color.s = k; break;
		case 2: color.v = k; break;
		case 3: color.a = k; break;
		}
		cha_area();
	}
}

