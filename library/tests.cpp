#include "tests.h"
#include "graphics.h"
#include <algorithm>
#include <chrono>

namespace
{

	struct _coordinates
	{
		_iarea iarea;
		_area area;
		bool rep;
		bool inv;
		_color c;
		_color c2;
		double d;
	};

	_coordinates generate_coordinates(const _picture& picture, bool with_rep)
	{
		_coordinates result;
		auto ss = std::max(picture.size.x, picture.size.y);
		i64 k1 = rnd(ss * 2) - ss / 2;
		i64 k2 = rnd(ss * 2) - ss / 2;
		i64 k3 = rnd(ss * 2) - ss / 2;
		i64 k4 = rnd(ss * 2) - ss / 2;
		double z1 = rnd(1024) / 1024.0;
		double z2 = rnd(1024) / 1024.0;
		double z3 = rnd(1024) / 1024.0;
		double z4 = rnd(1024) / 1024.0;
		result.iarea = _ixy(k1, k2);
		result.iarea |= _ixy(k3, k4);
		result.area = _xy(k1 + z1, k2 + z2);
		result.area |= _xy(k3 + z3, k4 + z4);
		if (rnd(7) == 1)
		{
			result.iarea.x.max = result.iarea.x.min + 1 + rnd(3);
			result.area.x.max = result.area.x.min + (1 + rnd(30)) / 10.0;
		}
		if (rnd(7) == 1)
		{
			result.iarea.y.max = result.iarea.y.min + 1 + rnd(3);
			result.area.y.max = result.area.y.min + (1 + rnd(30)) / 10.0;
		}
		if (result.iarea.x.length() != 0 && (result.iarea.x.length() & 1) == 0) result.iarea.x.max++; // против неопределенности линий
		if (result.iarea.y.length() != 0 && (result.iarea.y.length() & 1) == 0) result.iarea.y.max++; // против неопределенности линий
		result.d = rnd(70) / 10.0;
		result.rep = (rnd(3) == 1) && with_rep;
		result.inv = rnd(2);
		result.c.c = rnd();
		result.c2.c = rnd();
		if (result.rep)
		{
			result.c.a = 255;
			result.c2.a = 255;
		}
		return result;
	}

	_coordinates generate_coordinates(const _picture& picture, bool rep, bool vertical, bool gorisontal)
	{
		_coordinates result;
		auto ss = std::max(picture.size.x, picture.size.y);
		i64 k1 = rnd(ss * 2) - ss / 2;
		i64 k2 = rnd(ss * 2) - ss / 2;
		i64 k3 = rnd(ss * 2) - ss / 2;
		i64 k4 = rnd(ss * 2) - ss / 2;
		double z1 = rnd(1024) / 1024.0;
		double z2 = rnd(1024) / 1024.0;
		double z3 = rnd(1024) / 1024.0;
		double z4 = rnd(1024) / 1024.0;
		result.iarea = _ixy(k1, k2);
		result.iarea |= _ixy(k3, k4);
		result.area = _xy(k1 + z1, k2 + z2);
		result.area |= _xy(k3 + z3, k4 + z4);
		if (vertical)
		{
			result.iarea.x.max = result.iarea.x.min + 1;
			result.area.x.min = i64(result.area.x.min);
			result.area.x.max = result.area.x.min + 0.7;
		}
		if (gorisontal)
		{
			result.iarea.y.max = result.iarea.y.min + 1;
			result.area.y.min = i64(result.area.y.min);
			result.area.y.max = result.area.y.min + 0.7;
		}
		result.d = rnd(70) / 10.0;
		result.rep = rep;
		result.inv = rnd(2);
		result.c.c = rnd();
		result.c2.c = rnd();
		if (result.rep)
		{
			result.c.a = 255;
			result.c2.a = 255;
		}
		return result;
	}

	bool test_climbing_out_of_bounds(const _picture& picture, _color c)
	{
		for (i64 y = 0; y < picture.size.y; y++)
		{
			auto sl = picture.scan_line(y);
			for (i64 x = 0; x < picture.size.x; x++)
			{
				if (picture.drawing_area.test({ x,y })) continue;
				if (sl[x].c != c.c) return false;
			}
		}
		return true;
	}

	uchar max_delta(const _picture& picture1, const _picture& picture2)
	{
		if (picture1.size != picture2.size) return 255;
		uchar res = 0;
		for (i64 y = 0; y < picture1.size.y; y++)
		{
			auto sl1 = picture1.scan_line(y);
			auto sl2 = picture2.scan_line(y);
			for (i64 x = 0; x < picture1.size.x; x++)
			{
				uchar da = (sl1[x].a >= sl2[x].a) ? sl1[x].a - sl2[x].a : sl2[x].a - sl1[x].a;
				uchar dr = (sl1[x].r >= sl2[x].r) ? sl1[x].r - sl2[x].r : sl2[x].r - sl1[x].r;
				uchar dg = (sl1[x].g >= sl2[x].g) ? sl1[x].g - sl2[x].g : sl2[x].g - sl1[x].g;
				uchar db = (sl1[x].b >= sl2[x].b) ? sl1[x].b - sl2[x].b : sl2[x].b - sl1[x].b;
				res = std::max({ da, dr, dg, db, res });
			}
		}
		return res;
	}

	void draw_figure_0(_picture& picture, const _coordinates& coo)
	{
		picture.fill_rectangle(coo.iarea, {0}); // минимальная функция
	}

	void draw_figure_old(_picture& picture, const _coordinates& coo)
	{
		if (coo.inv)
			picture.line(coo.iarea.top_right(), coo.iarea.bottom_left(), coo.c, coo.rep);
		else
			picture.line(coo.iarea.top_left(), coo.iarea.bottom_right(), coo.c, coo.rep);
	}

	void draw_figure_new(_picture& picture, const _coordinates& coo)
	{
		if (coo.inv)
			picture.line4(coo.iarea.top_right(), coo.iarea.bottom_left(), coo.c, coo.rep);
		else
			picture.line4(coo.iarea.top_left(), coo.iarea.bottom_right(), coo.c, coo.rep);
	}

}

bool test_graph_climbing_out_of_bounds()
{
	static bool save1file = true;
	constexpr int number_of_graphic_elements = 1000;
	constexpr i64 max_size = 200;
	_isize picture_size[2] = { {max_size, i64(max_size * 0.7)}, {i64(max_size * 0.7), max_size} };
	_ixy delta[2] = { {15, 20}, {15, 20} };
	_color color[2] = { {0}, {0xFF000000} };
	for (auto i = 0; i < 2; i++)
	{
		_picture picture(picture_size[i]);
		for (auto j = 0; j < 2; j++)
		{
			picture.reset_drawing_area();
			picture.clear(color[j]);
			for (auto k = 0; k < 2; k++)
			{
				picture.set_drawing_area(_iarea({ delta[k].x, picture.size.x - delta[k].x }, { delta[k].y, picture.size.y - delta[k].y }));
				for (auto m = 0; m < number_of_graphic_elements; m++)
				{
					draw_figure_new(picture, generate_coordinates(picture, j == 0));
				}
				if (save1file)
				{
					save1file = false;
					picture.save_to_file(L"e:\\cob_tests.bmp");
				}
				if (!test_climbing_out_of_bounds(picture, color[j]))
				{
					picture.save_to_file(L"e:\\out_of_bounds.bmp");
					return false;
				}
			}
		}
	}
	return true;
}

bool test_graph_matching(bool with_transparent, bool with_rep)
{
	constexpr i64 max_size = 100;
	constexpr int number_of_graphic_elements = 1000;
	_picture picture_old({ max_size , max_size });
	_picture picture_new({ max_size , max_size });
	_color color[2] = { {0}, {0xFF000000} };
	for (auto i = (with_transparent) ? 0 : 1; i < 2; i++)
	{
		picture_old.clear(color[i]);
		picture_new.clear(color[i]);
		for (auto m = 0; m < number_of_graphic_elements; m++)
		{
			auto coo = generate_coordinates(picture_old, with_rep);
			draw_figure_old(picture_old, coo);
			draw_figure_new(picture_new, coo);
/*			if (i == 1 && m == 11)
			{
				picture_old.save_to_file(L"e:\\picture_old.bmp");
				picture_new.save_to_file(L"e:\\picture_new.bmp");
			}*/
			auto delta = max_delta(picture_old, picture_new);
			if (delta == 0) continue;
			if (delta > 2)
			{
				return false;
			}
			picture_new = picture_old;
		}
	}
	return true;
}

std::pair<i64, i64> test_graph_speed(i64 n, bool transparent, bool rep, bool vertical, bool gorisontal)
{
	_picture picture({ 1920 , 1080 });
	picture.clear(transparent ? _color{0} : _color{0xFF000000});
	rnd.init(0);
	auto t_start = std::chrono::high_resolution_clock::now();
	for (i64 i = 0; i < n; i++)
	{
		auto coo = generate_coordinates(picture, rep, vertical, gorisontal);
		draw_figure_0(picture, coo);
	}
	auto t0 = std::chrono::high_resolution_clock::now() - t_start;
	picture.clear(transparent ? _color{ 0 } : _color{ 0xFF000000 });
	rnd.init(0);
	t_start = std::chrono::high_resolution_clock::now();
	for (i64 i = 0; i < n; i++)
	{
		auto coo = generate_coordinates(picture, rep, vertical, gorisontal);
		draw_figure_old(picture, coo);
	}
	auto t1 = std::chrono::high_resolution_clock::now() - t_start;
	picture.clear(transparent ? _color{ 0 } : _color{ 0xFF000000 });
	rnd.init(0);
	t_start = std::chrono::high_resolution_clock::now();
	for (i64 i = 0; i < n; i++)
	{
		auto coo = generate_coordinates(picture, rep, vertical, gorisontal);
		draw_figure_new(picture, coo);
	}
	auto t2 = std::chrono::high_resolution_clock::now() - t_start;
	t1 -= t0;
	t2 -= t0;
	return { t1.count() / 1000000, t2.count() / 1000000 };
}