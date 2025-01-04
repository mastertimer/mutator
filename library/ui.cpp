#include "ui.h"

#include "win_basic.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

_ui_element::_ui_element(_ui* ui_): ui(ui_)
{
}

_ui_element::~_ui_element()
{
}

void _ui_element::ris2(_trans tr)
{
}

void _ui_element::update()
{
	auto copy_subelements = subelements;
	for (auto element : copy_subelements) element->update();
}

void _ui_element::key_down(u64 key)
{
}

void _ui_element::key_press(u64 key)
{
}

bool _ui_element::mouse_wheel2(_xy r, short value)
{
	return false;
}

bool _ui_element::mouse_down_left2(_xy r)
{
	return false;
}

void _ui_element::mouse_move_left2(_xy r)
{
}

void _ui_element::mouse_up_left2(_xy r)
{
}

void _ui_element::cha_area(std::optional<_area> a)
{
	if (!a) a = local_area;
	if (!parent)
	{
		if (ui->n_ko.get() == this) ui->add_changed_area(trans(*a));
		return;
	}
	parent->cha_area(trans(*a));
}

void _ui_element::ris(_trans tr)
{
	tr *= trans;
	if (((tr(calc_area())) & ui->changed_area).empty()) return;
	ui->canvas.set_drawing_area(tr(local_area) & ui->changed_area);
	ris2(tr);
	for (auto element : subelements) element->ris(tr);
}

_area _ui_element::calc_area()
{
	if (area) return *area;
	area = local_area;
	for (auto element : subelements)
	{
		element->calc_area();
		*area |= element->trans(*element->area);
	}
	return *area;
}

void _ui_element::add_child(std::shared_ptr<_ui_element> element)
{
	subelements.insert(element);
	element->parent = shared_from_this();
	element->add_area();
}

bool _ui_element::test_local_area(_xy b)
{
	return local_area.test(b);
}

bool _ui_element::mouse_move2(_xy r)
{
	return true;
}

bool _ui_element::mouse_move(_trans tr)
{
	for (auto element : subelements)
	{
		_trans tr2 = tr * element->trans;
		if (!element->calc_area().test(tr2.inverse(mouse.position))) continue;
		if (element->mouse_move(tr2)) return true;
	}
	_xy r = tr.inverse(mouse.position);
	if (test_local_area(r)) // ДЕЙСТВИЕ
	{
		ui->master_trans_go = tr;
		if (mouse_move2(r))
		{
			if ((ui->n_go_move.get() != this) || (ui->master_trans_go_move != ui->master_trans_go))
			{
				if (ui->n_go_move)
				{
					ui->n_go_move->mouse_finish_move();
				}
				ui->n_go_move = shared_from_this();
				ui->master_trans_go_move = ui->master_trans_go;
			}
			return true;
		}
	}
	return false;
}

void _ui_element::mouse_finish_move()
{

}

bool _ui_element::mouse_wheel_turn(_trans tr, short value)
{
	for (auto element : subelements)
	{
		_trans tr2 = tr * element->trans;
		if (!element->area->test(tr2.inverse(mouse.position))) continue;
		if (element->mouse_wheel_turn(tr2, value)) return true;
	}
	_xy r = tr.inverse(mouse.position);
	if (test_local_area(r)) // ДЕЙСТВИЕ
	{
		ui->master_trans_go = tr;
		if (mouse_wheel2(r, value)) return true;
	}
	return false;
}

bool _ui_element::mouse_down_left(_trans tr)
{
	for (auto element : subelements)
	{
		_trans tr2 = tr * element->trans;
		if (!element->area->test(tr2.inverse(mouse.position))) continue;
		if (element->mouse_down_left(tr2)) return true;
	}
	_xy r = tr.inverse(mouse.position);
	if (test_local_area(r))
	{
		if (key_fokus)
		{
			std::shared_ptr<_ui_element> b;
			if (ui->n_act_key)
				if (ui->n_act_key.get() != this)
					b = ui->n_act_key;
			ui->n_act_key = shared_from_this();
			if (b) b->cha_area();
			cha_area();
		}
		ui->master_trans_go = tr; // было в if
		if (mouse_down_left2(r))
		{
			ui->n_tani = shared_from_this();
			//				master_trans_go = tr;
			return true;
		}
		if (key_fokus) return true;
	}
	return false;
}

void _ui_element::add_area(std::optional<_area> a)
{
	if (!a)
		a = calc_area();
	else
		if (!(a <= area)) area.reset();
	if (!parent)
	{
		if (ui->n_ko.get() == this) ui->add_changed_area(trans(*a));
		return;
	}
	parent->add_area(trans(*a));
}

void _ui_element::del_area(std::optional<_area> a)
{
	if (!a)
		a = calc_area();
	else
		if (!(a < area)) area.reset();
	if (!parent)
	{
		if (ui->n_ko.get() == this) ui->add_changed_area(trans(*a));
		return;
	}
	parent->del_area(trans(*a));
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void _ui::paint(HWND hwnd)
{
	HDC hdc = GetDC(hwnd);
	RECT rect;
	GetClientRect(hwnd, &rect);
	draw({ rect.right, rect.bottom });
	BitBlt(hdc, 0, 0, rect.right, rect.bottom, canvas.hdc, 0, 0, SRCCOPY);
	ReleaseDC(hwnd, hdc);
}

_ui::_ui()
{
	n_ko = std::make_shared<_ui_element>(this);
}

void _ui::add_changed_area(_area a)
{
	changed_area |= (a & canvas.get_size());
}

_iarea _ui::draw(_isize r)
{
	if (canvas.resize(r)) changed_area = r;
	if (changed_area.empty()) return {};
	canvas.set_drawing_area(changed_area);
	changed_area &= canvas.get_size();
	canvas.clear(background_color);
	n_ko->ris(_trans());
	_iarea result = changed_area;
	changed_area = _area();
	return result;
}

void _ui::timer1000()
{
	for (auto element : n_timer1000) element->update();
}

void _ui::timer250()
{
	for (auto element : n_timer250) element->update();
}

void _ui::key_down(u64 key)
{
	if (!n_act_key) return;
	n_act_key->key_down(key);
}

void _ui::key_press(u64 key)
{
	if (!n_act_key) return;
	n_act_key->key_press(key);
}

void _ui::mouse_move()
{
	if (n_tani)
	{
		if (!mouse.left_button)
		{
			mouse_button_left_up();
			return;
		}
		n_tani->mouse_move_left2(master_trans_go.inverse(mouse.position));
		return;
	}
	if (n_perenos)
	{
		if (mouse.left_button)
		{
			n_ko->cha_area(n_ko->calc_area());
			n_ko->trans.offset += mouse.position - mouse.prev_position;
			n_ko->cha_area(n_ko->calc_area());
		}
		return;
	}
	if (!n_ko->mouse_move(n_ko->trans))
		if (n_go_move)
		{
			n_go_move->mouse_finish_move();
			n_go_move.reset();
		}
}

void _ui::mouse_button_left_down(bool dblclk)
{
	mouse.left_button = true;
	if (n_perenos) return;
	n_ko->mouse_down_left(n_ko->trans);
}

void _ui::mouse_button_left_up()
{
	mouse.left_button = false;
	if (!n_tani) return;
	n_tani->mouse_up_left2(master_trans_go.inverse(mouse.position));
	n_tani = nullptr;
}

void _ui::mouse_button_right_down(bool dblclk)
{
	mouse.right_button = true;
}

void _ui::mouse_button_right_up()
{
	mouse.right_button = false;
}

void _ui::mouse_button_middle_down(bool dblclk)
{
	mouse.middle_button = true;
	if (dblclk)
	{
		_xy tr = mouse.position;
		n_ko->cha_area(n_ko->calc_area());
		n_ko->trans.scale_up(tr, 1 / n_ko->trans.scale);
		n_ko->trans.scale = 1;
		n_ko->cha_area(n_ko->calc_area());
	}
}

void _ui::mouse_button_middle_up()
{
	mouse.middle_button = false;
	n_perenos = !n_perenos;
	set_cursor((n_perenos) ? _cursor::size_all : _cursor::normal);
}

void _ui::mouse_wheel(short value)
{
	if (n_perenos)
	{
		_xy tr = mouse.position;
		if ((value > 0) && (n_ko->trans.scale > 1E12)) return;
		n_ko->cha_area(n_ko->calc_area());
		n_ko->trans.scale_up(tr, pow(1.1, value));
		n_ko->cha_area(n_ko->calc_area());
		return;
	}
	if (mouse.right_button)
	{
		return;
	}
	n_ko->mouse_wheel_turn(n_ko->trans, value);
}

void _ui::add_hint(std::wstring_view hint, std::shared_ptr<_ui_element> g)
{
	del_hint();
	if (hint.empty()) return;
	_trans tr = master_trans_go;
	_isize siz = canvas.size_text(hint, 13);
	tr.offset += _xy{ -siz.x * 0.5, -15.0 } + _xy{ 0.5 * g->local_area.x, g->local_area.y.min } *tr.scale;
	tr.scale = 1;
	auto go = std::make_shared<_e_text>(this);
	go->c =cc3;
	go->c2 = background_color;
	go->trans = n_ko->trans.inverse() * tr;
	go->s = hint;
	go->update();
	n_ko->add_child(go);
	n_hint = go;
}

void _ui::del_hint()
{
	erase(n_hint);
}

void _ui::erase(std::shared_ptr<_ui_element> e)
{
	if (!e) return;
	while (!e->subelements.empty()) erase(*e->subelements.begin());
	if (e->parent)
	{
		e->del_area();
		e->parent->subelements.erase(e);
		e->parent.reset();
	}
	n_timer1000.erase(e);
	n_timer250.erase(e);
	if (n_ko == e) n_ko.reset();
	if (n_act_key == e) n_act_key.reset();
	if (n_tani == e) n_tani.reset();
	if (n_go_move == e) n_go_move.reset();
	if (n_hint == e) n_hint.reset();
}

void _ui::mouse_leave()
{
	if (mouse.left_button) mouse_button_left_up();
	if (mouse.right_button) mouse_button_right_up();
	if (mouse.middle_button) mouse_button_middle_up();
}

void _ui::mouse_button_left(bool pressed, bool dbl)
{
	if (dbl) { mouse_button_left_down(true); return; }
	if (pressed) mouse_button_left_down(); else mouse_button_left_up();
}

void _ui::mouse_button_right(bool pressed, bool dbl)
{
	if (dbl) { mouse_button_right_down(true); return; }
	if (pressed) mouse_button_right_down(); else mouse_button_right_up();
}

void _ui::mouse_button_middle(bool pressed, bool dbl)
{
	if (dbl) { mouse_button_middle_down(true); return; }
	if (pressed) mouse_button_middle_down(); else mouse_button_middle_up();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

_e_function::_e_function(_ui* ui_, std::function<void()> run_) : _ui_element(ui_), run(run_)
{

}

void _e_function::update()
{
	if (run) run();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

_e_scrollbar::_e_scrollbar(_ui* ui_) : _ui_element(ui_)
{
	local_area = { {0, 10}, {0, 10} };
}

_e_scrollbar::~_e_scrollbar()
{
	calc_area();
}

bool _e_scrollbar::mouse_down_left2(_xy r)
{
	mouse_move_left2(r);
	return true;
}

void _e_scrollbar::mouse_move_left2(_xy r)
{
	double ii;
	if ((vid & 1) == 0)
		ii = (r.x - local_area.x.min) / local_area.x.length();
	else
		ii = (r.y - local_area.y.min) / local_area.y.length();
	if (ii < 0) ii = 0;
	if (ii > 1) ii = 1;
	if (ii != position)
	{
		position = ii;
		cha_area();
		parent->update();
	}
}

void _e_scrollbar::ris2(_trans tr)
{
	_area a = tr(local_area);
	uint c = ui->cc1;
	if ((vid & 1) == 0)
	{
		ui->canvas.line({ a.x.min, 0.5 * a.y }, { a.x.max, 0.5 * a.y }, c);
		ui->canvas.line({ position * a.x, a.y.min }, { position * a.x, a.y.max }, c);
	}
	else
	{
		ui->canvas.line({ 0.5 * a.x, a.y.min }, { 0.5 * a.x, a.y.max }, c);
		ui->canvas.line({ a.x.min, position * a.y }, { a.x.max, position * a.y }, c);
	}
}

void _e_scrollbar::prilip()
{
	if (!parent) return;
	del_area();
	double l = ((vid & 1) == 1) ? local_area.x.length() : local_area.y.length();
	_area& o = parent->local_area;
	if (vid == 2) local_area = { {o.x.min, o.x.max}, {o.y.max, o.y.max + l} };
	if (vid == 3) local_area = { {o.x.max, o.x.max + l}, {o.y.min, o.y.max} };
	if (vid == 4) local_area = { {o.x.min, o.x.max}, {o.y.min - l, o.y.min} };
	if (vid == 5) local_area = { {o.x.min - l, o.x.min}, {o.y.min, o.y.max} };
	//	if (vid > 1) trans = _trans(); //глобальная замена trans
	area.reset();
	add_area();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void _e_button::mouse_up_left2(_xy r)
{
	cha_area();
}

_e_button::_e_button(_ui* ui_) : _ui_element(ui_)
{
	local_area = { {0, 26}, {0, 26} };
}

void _e_button::ris2(_trans tr)
{
	_area oo = tr(local_area);
	uint c = 0;
	if (!picture.get_size().empty())
	{
		i64 rx2 = (i64)(picture.get_size().x * tr.scale + 0.5);
		i64 ry2 = (i64)(picture.get_size().y * tr.scale + 0.5);
		_ixy ce = oo.center();
		ui->canvas.stretch_draw(&picture, ce.x - rx2 / 2, ce.y - ry2 / 2, tr.scale);
	}
	else
		c = ui->cc1;
	if (checked) c = ui->cc1 - 0x40000000;
	if (ui->n_go_move.get() == this) c = ui->cc1 - 0x80000000;
	if (ui->n_tani.get() == this) c = ui->cc2 - 0x80000000;
	ui->canvas.fill_rectangle(oo, { c });
}

bool _e_button::mouse_move2(_xy r)
{
	if (ui->n_go_move.get() != this) cha_area(); // первое перемещение
	if (!ui->n_hint) ui->add_hint(hint, shared_from_this()); // нет подсказки
	return true;
}

void _e_button::mouse_finish_move()
{
	cha_area();
	ui->del_hint();
}

bool _e_button::mouse_down_left2(_xy r)
{
	if (checkbox) checked = !checked;
	if (run) run(*this);
	cha_area();
	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

_e_text::_e_text(_ui* ui_) : _ui_element(ui_)
{
	local_area = { {0, 13}, {0, 13} };
}

bool _e_text::mouse_move2(_xy r)
{
	return false;
}

void _e_text::ris2(_trans tr)
{
	int sf = (int)(13 * tr.scale + 0.5);
	if (sf < 5) return;
	ui->canvas.text(tr.offset, s.c_str(), sf, c, c2);
}

void _e_text::update()
{
	del_area();
	_isize size = ui->canvas.size_text(s, 13);
	local_area = { {-1, std::max((double)size.x, 13.0)}, {0, std::max((double)size.y, 13.0)} };
	area.reset();
	add_area();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

_e_list::_e_list(_ui* ui_) : _ui_element(ui_)
{
	local_area = { {0, 60}, {0, 100} };
}

void _e_list::ris2(_trans tr)
{
	_iarea oo = tr(local_area);
	ui->canvas.rectangle(oo, { c });
}
