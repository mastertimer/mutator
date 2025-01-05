#pragma once

#include "graphics.h"
#include <memory>
#include <optional>
#include <set>
#include <functional>
#include "../main.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct _ui;

struct _ui_element : public std::enable_shared_from_this<_ui_element>
{
	_area local_area;
	_trans trans;
	std::set<std::shared_ptr<_ui_element>> subelements;
	std::shared_ptr<_ui_element> parent;
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
	virtual void key_down(u64 key);
	virtual void key_press(u64 key);
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

	virtual void draw(_trans tr);
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
	void draw(_trans tr) override;
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
	void draw(_trans tr) override;
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
	void draw(_trans tr) override;
	void update() override;
};

struct _e_scrollable_area : public _ui_element
{

};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct _ui: public _mode
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
	bool n_perenos = false;

	_trans master_trans_go; // трансформация тяни-толкай объекта, или объекта под мышкой
	_trans master_trans_go_move; // трансформация n_go_move

	_color background_color = black_color; // цвет фона
	_color border_color = 0xFF9A9A9A; // цвет рамок
	_color text_color = 0xFFDADADA; // цвет шрифта по умолчанию
	_color cc1 = 0xFF208040; // цвет 1
	_color cc2 = 0xFF40FF80; // цвет 2

	_ui();
	void paint(HWND hwnd);
	void add_changed_area(_area a);
	void mouse_button_left_down(bool dblclk = false);
	void mouse_button_left_up();
	void mouse_button_right_down(bool dblclk = false);
	void mouse_button_right_up();
	void mouse_button_middle_down(bool dblclk = false);
	void mouse_button_middle_up();
	void add_hint(std::wstring_view hint, std::shared_ptr<_ui_element> g);
	void del_hint();
	void erase(std::shared_ptr<_ui_element> e);

	virtual _bitmap& get_bitmap() override { return canvas; }
	virtual void timer250() override;
	virtual void timer1000() override;
	virtual void mouse_wheel(short value) override;
	virtual void mouse_leave() override;
	virtual void mouse_move() override;
	virtual _iarea draw(_isize r) override;
	virtual void key_down(u64 key) override;
	virtual void key_press(u64 key) override;
	virtual void mouse_button_left(bool pressed, bool dbl) override;
	virtual void mouse_button_right(bool pressed, bool dbl) override;
	virtual void mouse_button_middle(bool pressed, bool dbl) override;
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
