#pragma once

#include <map>
#include <functional>

#include "mgraphics.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct _canvas
{
	_bitmap bitmap; // изменившийся кусок
	_area2 changed_area; // область изменившиеся

	_canvas(_size2i size);
	void add_obl_izm(const _area2& a) { if (!(a & bitmap.size).empty()) changed_area += a; }
	void resize(_size2i size);
};

struct _peach;

struct _interactivity
{
	std::function<bool(_peach* a)> mouse_down_left;
};

struct _peach
{
	_area2 local_area;
	_transformation2 trans;
	uint c = 0xFF208040;
	uint c2 = 0;

	void insert(_coordinate layer, _peach* peach);
	_picture* draw();
	_area2 changed(); // изменившиеся обасть
	void set_picture(_size2i size);
	void set_mouse_down_left(std::function<bool(_peach* a)> b);
	void delete_me();

	bool mouse_button_left(_number2 xy, bool pressed, _transformation2 tr = _transformation2());

protected:
	std::unique_ptr<_canvas> picture;
	std::multimap<_coordinate, std::unique_ptr<_peach>> flavor;
	std::unique_ptr<_interactivity> ia; // интерактивность
	_peach* parent = nullptr;
	_area2 area;
	bool area_is_bad = true;

	_area2 calc_area(); // вычислить область
	void add_area(_area2 a = {}, bool start = true); // эта область добавлена
	void del_area(_area2 a = {}, bool start = true); // эта область добавлена
	virtual void draw_one(_bitmap& pic, _transformation2 tr) {}

private:
	void draw(_bitmap& pic, const _area2& obl_izm, _transformation2 tr);
};

struct _p_rect : public _peach
{
	_p_rect(const _area2& b = { { 0, 100}, {0, 100} }, _transformation2 tr = {}, uint c_ = 0xFF208040, uint c2_ = 0);

protected:
	void draw_one(_bitmap& pic, _transformation2 tr) override;
};

struct _p_text : public _peach
{
	_string text;

	_p_text(std::string_view s);

protected:
	void draw_one(_bitmap& pic, _transformation2 tr) override;
};
