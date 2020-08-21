/* TODO:
 * Научиться рисовать текст
 */

#pragma once

#include "color.h"
#include "mgeometry.h"

// рисует но не владеет данными
struct _renderer
{
	_color*   data;
	vec2<int> size;

	_renderer(_color* data = nullptr, vec2<int> size = {0, 0}) : data(data), size(size) { compute_cache(); }
	// TODO: c++20: шаблонный конструктор data + size;

	_renderer& target(_color* data, vec2<int> size)
	{
		this->data = data;
		this->size = size;
		compute_cache();
		return *this;
	}

	_color* begin() { return data; }
	_color* end() { return data + _size_p; }
	_color& px(int x, int y) { return data[y * size.x + x]; }

	_renderer& pixel(int x, int y, _color c);
	_renderer& pixel(vec2<int> pos, _color c) { return pixel(pos.x, pos.y, c); }
	_renderer& clear(_color c = {});
	_renderer& fill(_color c = {});

	void x_lineF(int beginx, int beginy, int endx, _color c);
	void x_line(int beginx, int beginy, int endx, _color c);
	void x_line(int beginx, int beginy, int endx, _color::rgb_cache cc);
	void y_lineF(int beginx, int beginy, int endy, _color c);
	void y_line(int beginx, int beginy, int endy, _color c);
	void y_line(int beginx, int beginy, int endy, _color::rgb_cache cc);

	void lineF(vec2i p1, vec2i p2, _color c); // неточная, но очень быстрая линия
	void line(vec2f p1, vec2f p2, _color c, double w = 1);

	_renderer& rect(vec2i p1, vec2i p2, _color c);
	_renderer& rect(_areai_old r, _color c);
	_renderer& rectD(vec2f p1, vec2f p2, _color c);
	_renderer& rectD(_area_old r, _color c);

	// void circle(XY<double> p, double r, Color c);
	// void tri(XY<int> p1, XY<int> p2, XY<int> p3, Color c);
	// void curve(...);
	// void image(Color* pixels, XY<int> size, XY<double> pos, XY<double> dSize);

	_renderer& fill_rect(vec2i p1, vec2i p2, _color c);
	_renderer& fill_rectD(vec2f p1, vec2f p2, _color c);

	// void fill_circle(XY<double> p, double r, Color c);
	// void fill_tri(XY<int> p1, XY<int> p2, XY<int> p3, Color c);

	_renderer& image(int x, int y, int w, int h, _color* src, vec2i src_size, bool transparent = false, int src_x = 0,
	                 int src_y = 0);

  private:
	void _fill_rectJ(vec2<int> p1, vec2<int> p2, _color c);

	// вспомогательный кэш
	size_t _size_p = 0;     // Количество пикселей всего
	size_t _size_b = 0;     // Размер массива пикселей в байтах
	bool   _2x_dv  = false; // Делится ли размер на 2
	void   compute_cache()
	{
		_size_p = size_t(size.x) * size.y;
		_size_b = _size_p * sizeof(_color);
		_2x_dv  = _size_p % 2 == 0;
	}
};
