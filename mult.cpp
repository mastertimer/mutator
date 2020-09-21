#include "basic.h"
#include "mult.h"

_bitmap mult(1920, 1080);

constexpr double radius   = 20.0; // радиус кружков
constexpr double dd       = 2.5;  // толщина ободка
constexpr double ddl      = 1.5;  // толщина линий связей
constexpr i64    v_type   = 16;   // количество типов кружков

constexpr i64 min_element = 380;  // минимальное количество кружков
constexpr i64 max_element = 420;  // максимальное количество кружков
constexpr i64 max_link1   = 6;    // максимальное количество связей для 1

constexpr i64 start_element = (max_element + min_element) / 2;  // первоначально количество кружков
constexpr i64 start_link    = start_element * 3; // первоначально количество связей
constexpr double best_dist  = radius * 4; // оптимальная дистанция

constexpr uint color[32] =
{ 
	0xFF0080FD, 0xFFEF0000, 0xFF9E3BFF, 0xFF938700, 0xFF12AA00, 0xFFD34E0D, 0xFF7470DC,	0xFF4D9682,
	0xFFC80FCE, 0xFFB06381,	0xFFE22653, 0xFF009D93, 0xFFC4439D, 0xFF258ECB, 0xFF659B00, 0xFF3F77FF,
	0xFF887E87, 0xFF9659DD, 0xFFB57000, 0xFF39A337, 0xFF5D84BE, 0xFFAF1FFB, 0xFFDE028A, 0xFF768F51,
	0xFFB400FC,	0xFFE23700, 0xFFCB29B2, 0xFFA3755A, 0xFFC45D37, 0xFFEB171B, 0xFF966DA8, 0xFFAC4ACF
};

constexpr uint fr[16] =
{
	0xBE3B0D3E, 0xD8B09960, 0x8BC79403, 0x590FF05C, 0x8E149C5D,	0xFE3A1270, 0xB16E7870, 0xC6663333,
	0x8FD23C3A,	0xEE817919,	0x9894BA4F, 0x4AE53B30, 0xBD9AA006, 0x4FB3F6FB, 0xBA015048,	0x320CDAD2
};

double delta_signal1 = 0.125;
double delta_signal2 = 0.125;

double k_signal = 0.0; // положение сигнала внутри тетронов (0 ... 2]

uint color_signal = 0xffffffff;

struct _mult_tetron;

struct _mult_link
{
	_mult_tetron* a; // связанный объект
	i64 f; // тип связи
};

struct _mult_tetron
{
	_xy p{}; // центр
	i64 type{}; // тип
	std::vector<_mult_link> link; // связи
	_xy f; // сила
};

struct _signal
{
	_mult_tetron* a = nullptr; // активный объект
	std::vector<i64> li; // номер активной связи
};

std::vector<_mult_tetron*> element; // тетроны
std::vector<_signal> signal; // ползущий сигнал

void create_link(_mult_tetron* a)
{
	double min_r2 = 1e8;
	_mult_tetron* b = nullptr;
	for (auto j : element)
	{
		if (j == a) continue;
		bool ok = true;
		for (auto jj : a->link)
			if (jj.a == j)
			{
				ok = false;
				break;
			}
		if (!ok) continue;
		double r2 = (a->p - j->p).len2();
		if (r2 < min_r2)
		{
			b = j;
			min_r2 = r2;
		}
	}
	if (b) a->link.push_back({ b, rnd(16) });
}

void init_mult()
{
	for (i64 i = 0; i < start_element; i++)
	{
		_mult_tetron *a = new _mult_tetron;
		a->type = rnd(v_type);
	restart:
		a->p = { rnd(mult.size.x - (i64)radius * 2) + radius, rnd(mult.size.y - (i64)radius * 2) + radius };
		for (i64 j = 0; j < i; j++)
			if ((a->p - element[j]->p).len2() < radius * radius * 4) goto restart;
		element.push_back(a);
	}
	for (i64 i = 0; i < start_link; i++) create_link(element[rnd(element.size())]);
}

void distance()
{
	for (auto i : element) i->f = { 0, 0 };
	for (auto i : element)
	{
		auto fot = [i](_xy v)
		{
			_xy r = v - i->p;
			double rr = r.len();
			if (rr < best_dist)
			{
				double k = 1000 * (rr - best_dist) / (rr * rr * rr);
				r *= k / r.len();
				i->f += r;
			}
		};
		fot({ 0.0, i->p.y });
		fot({ (double)mult.size.x, i->p.y });
		fot({ i->p.x, 0.0 });
		fot({ i->p.x, (double)mult.size.y });
		for (auto j : element) // отталкивание
		{
			if (i == j) continue;
			fot(j->p);
		}
		for (auto j : i->link) // притяжение
		{
			_xy r = j.a->p - i->p;
			double rr = r.len();
			if (rr <= best_dist) continue;
			double k = (rr - best_dist) * 0.001;
			r *= k / r.len();
			i->f += r;
			j.a->f -= r;
		}
	}
	for (auto i : element)
	{
		double ll = i->f.len2();
		if (ll > 10.0) i->f *= 10.0 / ll;
		i->p += i->f;
	}
}

void del_element(_mult_tetron* a)
{
	auto n = element.begin();
	for (auto i = element.begin(); i != element.end(); ++i)
	{
		if (*i == a) { n = i; continue; }
		for (auto j = (*i)->link.begin(); j != (*i)->link.end(); ++j)
			if (j->a == a)
			{
				(*i)->link.erase(j);
				break;
			}
	}
	delete* n;
	element.erase(n);
}

void create_element(_mult_tetron* a)
{
	_mult_tetron* b = new _mult_tetron;
	b->p = a->p + _xy{ radius, 0 }.rotation(rnd(628) * 0.01);
	b->type = rnd(v_type);
	if (rnd(2))
		a->link.push_back({ b, rnd(16) });
	else
		b->link.push_back({ a, rnd(16) });
	element.push_back(b);
}

void move_signal()
{
	if (signal.empty())
	{
		_signal a;
		a.a = element[rnd(element.size())];
		signal.push_back(a);
		a.a = element[rnd(element.size())];
		if (a.a != signal[0].a)
		{
			signal.push_back(a);
			a.a = element[rnd(element.size())];
			if ((a.a != signal[0].a) && (a.a != signal[1].a)) signal.push_back(a);
		}
		k_signal = 0.0;
		color_signal = (uint)(rnd() | 0xff808080);
	}
	if (k_signal < 1.0)
		k_signal += delta_signal1;
	else
		k_signal += delta_signal2;
	if (k_signal == 1.0)
	{
		decltype(signal) signal2 = signal;
		signal.clear();
		for (auto& i : signal2)
			switch (rnd(8))
			{
			case 0:
				if (element.size() <= min_element)
					signal.push_back(i);
				else
					del_element(i.a);
				break;
			case 1:
				signal.push_back(i);
				if (element.size() < max_element) create_element(i.a);
				break;
			case 2:
				if (i.a->link.size()) i.a->link.erase(i.a->link.begin() + rnd(i.a->link.size()));
				signal.push_back(i);
				break;
			case 3: case 4: case 5:
				if (i.a->link.size() < max_link1) create_link(i.a);
				signal.push_back(i);
				break;
			default:
				signal.push_back(i);
			}

		for (auto& i : signal)
			for (u64 j = 0; j < i.a->link.size(); j++)
				if (rnd(3) == 0) i.li.push_back(j);
	}
	if (k_signal > 2.0)
	{
		std::vector<_mult_tetron*> element2;
		for (auto& i : signal)
			for (auto j : i.li)
			{
				_mult_tetron* a = i.a->link[j].a;
				bool est = false;
				for (auto k : element2) if (k == a) { est = true; break; }
				if (!est) element2.push_back(a);
			}
		signal.clear();
		for (auto k : element2)
		{
			_signal a;
			a.a = k;
			signal.push_back(a);
		}
		k_signal = 0.0;
	}
}

void move_mult(i64 dt)
{
	move_signal();
	distance();
}

_bitmap* draw_mult()
{
	static i64 t_pr = 0;
	i64 t = GetTickCount64();
	if (t_pr == 0) init_mult(); else move_mult(t - t_pr);
	t_pr = t;
	mult.clear();
	double y = 0.7 * (radius - dd) * 2;
	for (auto i : element)
	{
		mult.ring(i->p, radius, dd, color[i->type]);
		mult.froglif(i->p - _xy{ y / 2, y / 2 }, y, (uchar*)&fr[i->type], 2, color[i->type]);
	}
	for (auto i : element)
		for (auto j : i->link)
		{
			_xy p1 = i->p;
			_xy p2 = j.a->p;
			_xy v1 = p2 - p1;
			_xy e = -v1;
			v1 *= radius / v1.len();
			p1 += v1.rotation(-0.2);
			p2 += (-v1).rotation(0.2);
			mult.lines(p1, p2, ddl, color[16 + j.f]);
			e *= 5.0 / e.len();
			_xy e1 = e.rotation(0.3);
			_xy e2 = e.rotation(-0.3);
			mult.lines(p2, p2 + e1, ddl, color[16 + j.f]);
			mult.lines(p2, p2 + e2, ddl, color[16 + j.f]);
		}
	for (auto& i : signal)
		if (k_signal <= 1.0)
		{
			mult.ring(i.a->p, radius * k_signal, dd*1.5, color_signal);
		}
		else
			for (auto j : i.li)
			{
				_xy p1 = i.a->p;
				_xy p2 = i.a->link[j].a->p;
				_xy v1 = p2 - p1;
				_xy e = -v1;
				v1 *= radius / v1.len();
				p1 += v1.rotation(-0.2);
				p2 += (-v1).rotation(0.2);
				mult.fill_circle(p1 + (p2 - p1) * (k_signal - 1.0), ddl*3, color_signal);
			}
	return &mult;
};