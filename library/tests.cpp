#include "tests.h"
#include "graphics.h"

namespace
{
	i64 max_size = 200;

	struct _coordinates
	{
		_iarea iarea;
		bool rep;
		_color c;
	};

	_coordinates generate_coordinates(const _picture& picture)
	{
		_coordinates result;
		result.iarea = _ixy(rnd(max_size * 2) - max_size / 2, rnd(max_size * 2) - max_size / 2);
		result.iarea |= _ixy(rnd(max_size * 2) - max_size / 2, rnd(max_size * 2) - max_size / 2);
		result.rep = (rnd(3) == 1);
		result.c.c = rnd();
		if (result.rep && !picture.transparent) result.c.a = 255;
		return result;
	}

	void draw_figure_new(_picture& picture, const _coordinates& coo)
	{
		picture.fill_rectangle(coo.iarea, coo.c.c, coo.rep);
	}

	bool test_climbing_out_of_bounds(const _picture& picture, _color c)
	{
		for (i64 y = 0; y < picture.size.y; y++)
		{
			auto sl = picture.scan_line(y);
			for (i64 x = 0; x < picture.size.x; x++)
			{
				if (!picture.drawing_area.test({ x,y })) continue;
				if (sl[x].c != c.c) return false;
			}
		}
		return true;
	}

}

bool test_graph_climbing_out_of_bounds()
{
	static bool save1file = true;
	constexpr int number_of_graphic_elements = 1000;
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
					draw_figure_new(picture, generate_coordinates(picture));
				}
				if (!test_climbing_out_of_bounds(picture, color[j])) return false;
				if (save1file)
				{
					save1file = false;
					picture.save_to_file(L"e:\\cob_tests.bmp");
				}
			}
		}
	}
	return true;
}

