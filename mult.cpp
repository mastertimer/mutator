#include "mgraphics.h"
#include "mult.h"

_bitmap mult(1920, 1080);

constexpr double radius    = 20.0; // радиус кружков
constexpr double dd        = 2.5;  // толщина ободка
constexpr i8 start_element = 400;  // первоначально количество кружков
constexpr i8 v_type        = 16;   // количество типов кружков

constexpr u4 color[32] =
{ 
	0xFF0080FD, 0xFFEF0000, 0xFF9E3BFF, 0xFF938700, 0xFF12AA00, 0xFFD34E0D, 0xFF7470DC,	0xFF4D9682,
	0xFFC80FCE, 0xFFB06381,	0xFFE22653, 0xFF009D93, 0xFFC4439D, 0xFF258ECB, 0xFF659B00, 0xFF3F77FF,
	0xFF887E87, 0xFF9659DD, 0xFFB57000, 0xFF39A337, 0xFF5D84BE, 0xFFAF1FFB, 0xFFDE028A, 0xFF768F51,
	0xFFB400FC,	0xFFE23700, 0xFFCB29B2, 0xFFA3755A, 0xFFC45D37, 0xFFEB171B, 0xFF966DA8, 0xFFAC4ACF
};

constexpr u4 fr[16] =
{
	0xBE3B0D3E, 0xD8B09960, 0x8BC79403, 0x590FF05C, 0x8E149C5D,	0xFE3A1270, 0xB16E7870, 0xC6663333,
	0x8FD23C3A,	0xEE817919,	0x9894BA4F, 0x4AE53B30, 0xBD9AA006, 0x4FB3F6FB, 0xBA015048,	0x320CDAD2
};

struct _mult_tetron
{
	_coo2 p; // центр
	i8 type; // тип
};

std::vector<_mult_tetron> element;

void init_mult()
{
	for (i8 i = 0; i < start_element; i++)
	{
		_mult_tetron a;
		a.type = rnd(v_type);
	restart:
		a.p = { rnd(mult.size.x - (i8)radius * 2) + radius, rnd(mult.size.y - (i8)radius * 2) + radius };
		for (i8 j = 0; j < i; j++)
			if ((a.p - element[j].p).len2() < radius * radius * 6) goto restart;
		element.push_back(a);
	}
}

void move_mult(i8 dt)
{

}

_bitmap& draw_mult()
{
	static i8 t_pr = 0;
	i8 t = GetTickCount64();
	if (t_pr == 0) init_mult(); else move_mult(t - t_pr);
	t_pr = t;
	mult.clear();
	double y = 0.7 * (radius - dd) * 2;
	for (auto& i : element)
	{
		mult.ring(i.p, radius, dd, color[i.type]);
		mult.froglif(i.p - _coo2{ y / 2, y / 2 }, y, (uchar*)&fr[i.type], 2, color[i.type]);

	}
	return mult;
};