#pragma once

#include "graphics.h"
#include <memory>
#include <optional>
#include <set>
#include <functional>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct _ui;

struct _ui_element : public std::enable_shared_from_this<_ui_element>
{
	_area local_area;
	_trans trans;
	std::set<std::shared_ptr<_ui_element>> subelements;
	std::shared_ptr<_ui_element> parent;
	_color c{ 0xFF208040 };
	_color c2{ 0 };
	_ui* ui;

	_ui_element(_ui* ui_);
	virtual ~_ui_element();

	void ris(_trans tr); // нарисовать
	void add_child(std::shared_ptr<_ui_element> element);
	void cha_area(std::optional<_area> a = std::nullopt);
	void add_area(std::optional<_area> a = std::nullopt); // эта область добавлена
	void del_area(std::optional<_area> a = std::nullopt); // эта область удалена
	_area calc_area(); // вычислить область

	virtual void update();
	virtual void key_down(ushort key);
	virtual void key_press(ushort key);
	virtual bool mouse_wheel2(_xy r, short value);
	virtual bool mouse_down_left2(_xy r);
	virtual void mouse_move_left2(_xy r);
	virtual void mouse_up_left2(_xy r); // конец перетаскивания
	bool mouse_down_left(_trans tr); // щелчок мышки
	bool mouse_move(_trans tr); // перемещение мышки
	bool mouse_wheel_turn(_trans tr, short value); // поворот колеса мышки
	virtual bool mouse_move2(_xy r); // перемещение мышки действие
	virtual void mouse_finish_move(); // мышка ушла
	virtual bool test_local_area(_xy b); // лежит ли точка внутри
	template <typename _t> std::shared_ptr<_t> find1(); // найти указатель нужного типа на глубине 1

protected:
	bool key_fokus = false;
	std::optional<_area> area;

	virtual void ris2(_trans tr);
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct _e_function : public _ui_element
{
	std::function<void()> run;

	_e_function(_ui* ui_, std::function<void()> run_ = nullptr);
	void update() override;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct _e_scrollbar : public _ui_element
{
	double position = 0; // положение стрелки [0-1]
	char vid = 0; // 0 - горизонтальный 1 - вертикальный 2 - снизу 3 - справа 4 - сверху 5 - слева

	_e_scrollbar(_ui* ui_);
	~_e_scrollbar();
	bool mouse_down_left2(_xy r) override;
	void mouse_move_left2(_xy r) override;
	void ris2(_trans tr) override;
	void prilip(); // прилипание к графическому объекту
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct _e_button : public _ui_element
{
	bool checkbox = false;
	bool checked = false;
	std::wstring hint;
	_picture picture;
	std::function<void(_e_button&)> run;

	_e_button(_ui* ui_);
	void mouse_up_left2(_xy r);
	void ris2(_trans tr) override;
	bool mouse_move2(_xy r) override;
	void mouse_finish_move() override;
	bool mouse_down_left2(_xy r) override;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct _e_text : public _ui_element
{
	std::wstring s = L"";

	_e_text(_ui* ui_);
	bool mouse_move2(_xy r) override;
	void ris2(_trans tr) override;
	void update() override;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct _ui
{
	_bitmap canvas;
	_area changed_area;

	std::shared_ptr<_ui_element> n_ko;
	std::shared_ptr<_ui_element> n_act_key;
	std::shared_ptr<_ui_element> n_tani;
	std::shared_ptr<_ui_element> n_go_move;
	std::shared_ptr<_ui_element> n_hint;
	std::set<std::shared_ptr<_ui_element>> n_timer1000;
	std::set<std::shared_ptr<_ui_element>> n_timer250;
	bool n_s_ctrl = false;
	bool n_s_shift = false;
	bool n_s_left = false;
	bool n_s_right = false;
	bool n_s_middle = false;
	bool n_perenos = false;
	_xy mouse_xy{ 0, 0 };
	_xy mouse_xy_pr{ 0, 0 };

	_trans master_trans_go; // трансформация тяни-толкай объекта, или объекта под мышкой
	_trans master_trans_go_move; // трансформация n_go_move

	_color cc0 = black_color; // цвет фона
	_color cc1 = 0xFF208040; // цвет 1
	_color cc2 = 0xFF40FF80; // цвет 2
	_color cc3 = 0xFFA0FFC0; // цвет 3
	_color cc4 = 0xFF104020; // цвет 4

	_ui();
	void paint(HWND hwnd);
	void add_changed_area(_area a);
	void draw(_isize r);
	void run_timer1000();
	void run_timer250();
	void key_down(ushort key);
	void key_press(ushort key);
	void mouse_move();
	void mouse_button_left_down(bool dblclk = false);
	void mouse_button_left_up();
	void mouse_button_right_down(bool dblclk = false);
	void mouse_button_right_up();
	void mouse_button_middle_down(bool dblclk = false);
	void mouse_button_middle_up();
	void mouse_wheel_turn(short value);
	void add_hint(std::wstring_view hint, std::shared_ptr<_ui_element> g);
	void del_hint();
	void erase(std::shared_ptr<_ui_element> e);
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template <typename _t> std::shared_ptr<_t> _ui_element::find1()
{
	for (auto element : subelements)
	{
		auto tel = std::dynamic_pointer_cast<_t>(element);
		if (tel) return tel;
	}
	return nullptr;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
