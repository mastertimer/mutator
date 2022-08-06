#include "tests.h"
#include "graphics.h"
#include <algorithm>

namespace
{

	struct _coordinates
	{
		_iarea iarea;
		bool rep;
		_color c;
	};

	_coordinates generate_coordinates(const _picture& picture, bool with_rep)
	{
		_coordinates result;
		auto ss = std::max(picture.size.x, picture.size.y);
		result.iarea = _ixy(rnd(ss * 2) - ss / 2, rnd(ss * 2) - ss / 2);
		result.iarea |= _ixy(rnd(ss * 2) - ss / 2, rnd(ss * 2) - ss / 2);
		if (rnd(7) == 1) result.iarea.x.max = result.iarea.x.min + 1 + rnd(3);
		if (rnd(7) == 1) result.iarea.y.max = result.iarea.y.min + 1 + rnd(3);
		result.rep = (rnd(3) == 1) && with_rep;
		result.c.c = rnd();
		if (result.rep && !picture.transparent) result.c.a = 255;
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

	void draw_figure_old(_picture& picture, const _coordinates& coo)
	{
		picture.fill_rectangle(coo.iarea, coo.c.c, coo.rep);
	}

	void draw_figure_new(_picture& picture, const _coordinates& coo)
	{
		picture.fill_rectangle2(coo.iarea, coo.c, coo.rep);
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
					draw_figure_new(picture, generate_coordinates(picture, true));
				}
				if (save1file)
				{
					save1file = false;
					picture.save_to_file(L"e:\\cob_tests.bmp");
				}
				if (!test_climbing_out_of_bounds(picture, color[j])) return false;
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
			auto delta = max_delta(picture_old, picture_new);
			if (delta == 0) continue;
			if (delta > 1)
			{
				return false;
			}
			picture_new = picture_old;
		}
	}
	return true;
}
