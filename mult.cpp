#include "mgraphics.h"
#include "mult.h"

_picture mult(1920, 1080);

constexpr double radius = 30; // радиус кружков
constexpr i8 start_element = 10; // первоначально количество кружков
constexpr i8 v_type = 16; // количество типов кружков

constexpr uint color[32] =
{ 
	0xFF0080FD, 0xFFEF0000, 0xFF9E3BFF, 0xFF938700, 0xFF12AA00, 0xFFD34E0D, 0xFF7470DC,	0xFF4D9682,
	0xFFC80FCE, 0xFFB06381,	0xFFE22653, 0xFF009D93, 0xFFC4439D, 0xFF258ECB, 0xFF659B00, 0xFF3F77FF,
	0xFF887E87, 0xFF9659DD, 0xFFB57000, 0xFF39A337, 0xFF5D84BE, 0xFFAF1FFB, 0xFFDE028A, 0xFF768F51,
	0xFFB400FC,	0xFFE23700, 0xFFCB29B2, 0xFFA3755A, 0xFFC45D37, 0xFFEB171B, 0xFF966DA8, 0xFFAC4ACF
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
		a.p = { (double)rnd(100), (double)rnd(100) };
		element.push_back(a);
	}
}

void draw()
{
	static i8 t_pr = 0;
	i8 t = GetTickCount64();
	if (t_pr == 0)
	{
		init_mult();
		t_pr = t;
		return;
	}
};