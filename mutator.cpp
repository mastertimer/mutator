/*//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

памятка на будущее:
!!! не объеденять с другими проектами, т.к. потом основное желание упрощения мутатора - исключить чужеродные проекты

ключевые задачи для дальнейшего развития:

1. полноценное программирование на графах.
для этого нужен инструмент для визуального представления графа программы
поэтому нужно сделать новую версию _g_tetron (на c++) - комплексное представление графа в фрактальном и
классическом виде (f1.bmp)
2. всю функциональность из земной панели добавить в "new_g_tetron" (земную панель удалить)
3. переписать "new_g_tetron" на языке мутатора

с чего начать:
сделать новый комплексный _g_tetron (на с++)

...большое колесо - список тетронов. оно должно вызываться при создании нового тетрона

разные идеи:
-длинный список без полосы прокрутки: концы уменьшенные  ..aaAAaa..
-самодельный шрифт


----------------------------------------------------------------------------------------------------

упорядочить функции _picture
сделать список имен (i - цикл, t - tetron, s - строка и т.д.)

/*//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <functional>

#include "t_function.h"
#include "mutator.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

_tetron* create_tetron(const std::string& name)
{
#define make(name) {""#name, []() -> _tetron* { return new name; }}
	static std::map<std::string, std::function<_tetron* ()>> ss_tetron =
	{
		make(_tetron),
		make(_g_edit64bit),
		make(_g1list),
		make(_t_multi_string),
		make(_g_edit_int),
		make(_t_trans),
		make(_g_test_graph),
		make(_t_double),
		make(_g_link),
		make(_g_circle),
		make(_g_line),
		make(_t_xy),
		make(_g_rect),
		make(_g_froglif),
		make(_g_scrollbar),
		make(_t_string),
		make(_g_edit_string),
		make(_t_int),
		make(_g_list_link),
		make(_g_text),
		make(_g_edit_multi_string),
		make(_g_picture),
		make(_t_function),
		make(_g_edit_double),
		make(_g_button),
		make(_g_tetron)
	};
#undef make
	auto fun = ss_tetron.find(name);
	if (fun == ss_tetron.end()) return nullptr;
	return fun->second();
}

_tetron* create_tetron(uchar type)
{
	switch (type)
	{
	case  0: return new _tetron;              // 0x20
	case  1: return new _g_edit64bit;         // 0xFE
	case  2: return new _g1list;              // 0xF7
	case  3: return new _t_multi_string;      // 0x90
	case  4: return new _g_edit_int;          // 0xF9
	case  5: return new _t_trans;             // 0xF0
	case  6: return new _g_test_graph;        // 0xF2
	case  7: return new _t_double;            // 0x50
	case  8: return new _g_link;              // 0x7A

	case 10: return new _g_circle;            // 0x74
	case 11: return new _g_line;              // 0x70
	case 12: return new _g_rect;              // 0xF4

	case 14: return new _g_froglif;           // 0x7F
	case 15: return new _t_xy;                // 0x30
	case 16: return new _g_scrollbar;         // 0xF8

	case 24: return new _t_string;            // 0x10
	case 25: return new _g_edit_string;       // 0xFB
	case 26: return new _t_int;               // 0x40

	case 28: return new _g_list_link;         // 0xF5
	case 29: return new _g_text;              // 0xF1
	case 30: return new _g_edit_multi_string; // 0xFD
	case 31: return new _g_picture;           // 0xF6
	case 32: return new _t_function;          // 0xA0
	case 33: return new _g_edit_double;       // 0x79
	case 34: return new _g_button;            // 0xF3

	case 38: return new _g_tetron;            // 0xFA
	}
	return nullptr;
}

namespace mutator
{
	void save_to_txt_file(wstr fn)
	{
		_wjson tet(fn);
		std::map<u64, _tetron*> tt; // чтобы упорядочить тетроны
		for (auto i : all_tetron) tt[i->id] = i;
		tet.arr("tetrons");
		for (auto i : tt)
		{
			_tetron* t = i.second;
			tet.str().add("name", t->name()).add("id", i.first);
			t->push(tet);
			tet.arr("links");
			std::map<u64, _link*> li;
			for (auto j : t->link) li[(*j)(t)->id] = j;
			for (auto j : li) tet.str("", true).add("id", (*j.second)(t)->id).add_hex("flags", j.second->get_flags(t)).end();
			tet.end().end();
		}
		tet.end().arr("chosen");
		for (auto i : master_chosen) tet.add(i->id);
		tet.end().add("id_tetron", id_tetron);
	}

	void init_layers()
	{
		master_layers.clear();
		if (!n_go_layer) return;
		for (auto i : n_go_layer->link)
		{
			_tetron* a = (*i)(n_go_layer);
			if (!i->test_flags(a, flag_parent)) continue;
			_t_double* b = *a;
			if (b == nullptr) continue;
			double n = b->a;
			master_layers[n] = b;
		}
	}

	bool load_from_txt_file(wstr fn)
	{
		_rjson tet(fn);
		tet.arr("tetrons");
		while (!tet.error)
		{
			tet.obj();
			if (tet.null) break;
			std::string name = tet.read_string("name");
			u64 my_id;
			tet.read("id", my_id);
			id_tetron = my_id;
			_tetron* tt = create_tetron(name);
			if (tt->type() == 18) terminal = tt; // временно
			tt->pop(tet);
			tet.arr("links");
			while (!tet.error)
			{
				tet.obj();
				if (tet.null) break;
				u64 id;
				tet.read("id", id);
				u64 flags;
				tet.read("flags", flags);
				if (id <= my_id) _id(my_id)->add_flags(_id(id), flags, false);
				tet.end();
			}
			tet.end();
			tet.end();
		}
		tet.end();
		tet.arr("chosen");
		for (uint i = 0; i < master_chosen.size(); i++)	tet.read("", master_chosen[i]->id);
		tet.end();
		tet.read("id_tetron", id_tetron);
		init_layers();
		return (tet.error == 0);
	}

	void resize(_isize r)
	{
		static auto prev_size = _isize{ 1200, 800 };
		if (prev_size != _isize{ 1200, 800 })
		{
			_t_trans* kor = *n_ko;
			kor->trans.offset.x += (double)(r.x - prev_size.x) / 2;
			kor->trans.offset.y += (double)(r.y - prev_size.y) / 2;
			n_move_all->run(0, n_move_all, flag_run);
			master_obl_izm = r;  // обновить экран
		}
		prev_size = r;
	}

	void draw(_isize r)
	{
		if (master_bm.resize(r)) master_obl_izm = r;
		if (master_obl_izm.empty()) return;
		master_bm.set_drawing_area(master_obl_izm);
		master_obl_izm &= master_bm.size;
		master_bm.clear({ cc0 });
		master_chain_go.clear();
		n_ko->operator _t_trans* ()->ris(_trans(), false);
		master_obl_izm = _area();
	}

	bool start(wstr fn)
	{
		master_chosen.push_back(&n_ko);                // !!корневой графический объект (трансформация)
		master_chosen.push_back(&n_s_shift);           // !!зажата клавиша Shift
		master_chosen.push_back(&n_s_alt);             // !!зажата клавиша Alt
		master_chosen.push_back(&n_s_ctrl);            // !!зажата клавиша Ctrl
		master_chosen.push_back(&n_s_left);            // зажата левая кнопка мышки
		master_chosen.push_back(&n_s_right);           // зажата правая кнопка мышки
		master_chosen.push_back(&n_s_middle);          // зажато колесо мышки
		master_chosen.push_back(&n_s_double);          // двойной щелчок мышки
		master_chosen.push_back(&n_down_left);         // нажата левая кнопка мышки
		master_chosen.push_back(&n_down_right);        // нажата правая кнопка мышки
		master_chosen.push_back(&n_down_middle);       // нажата средняя кнопка мышки (колесо)
		master_chosen.push_back(&n_up_left);           // отжата левая кнопка мышки
		master_chosen.push_back(&n_up_right);          // отжата правая кнопка мышки
		master_chosen.push_back(&n_up_middle);         // отжата средняя кнопка мышки (колесо)
		master_chosen.push_back(&n_move);              // перемещен курсор мышки
		master_chosen.push_back(&n_wheel);             // повернуто колесо мышки
		master_chosen.push_back(&n_hint);              // подсказка для элемента под курсором мышки
		master_chosen.push_back(&n_hex);               // АБСТРАКТНЫЙ ПАРАЗИТ 16-ричная система счисления
		master_chosen.push_back(&n_go_move);           // !!цепочка!! GO, над которым перемещается мышка
		master_chosen.push_back(&n_perenos);           // режим переноса
		master_chosen.push_back(&n_move_all);          // вызывается после глобального смещения или масштабирования
		master_chosen.push_back(&n_ramk);              // рамка выделения потенциально активного
		master_chosen.push_back(&n_ramk2);             // рамка выделения активного
		master_chosen.push_back(&n_pot_act);           // потенциально активный тетрон
		master_chosen.push_back(&n_act);               // активный тетрон
		master_chosen.push_back(&n_zagolovok);         // АБСТРАКТНЫЙ ПРЕДОК - заголовки
		master_chosen.push_back(&n_checkbox);          // АБСТРАКТНЫЙ ПРЕДОК - переключатель
		master_chosen.push_back(&n_go_layer);          // АБСТРАКТНЫЙ ПРЕДОК - графический слой
		master_chosen.push_back(&n_color_line);        // АБСТРАКТНЫЙ ПРЕДОК - цвет линий и текста
		master_chosen.push_back(&n_color_bg);          // АБСТРАКТНЫЙ ПРЕДОК - цвет фона
		master_chosen.push_back(&n_act_key);           // активный тетрон для управления клавиатурой
		master_chosen.push_back(&n_down_key);          // нажата кнопка клавиатуры
		master_chosen.push_back(&n_press_key);         // введен символ
		master_chosen.push_back(&n_timer1000);         // таймер с периодом 1000
		master_chosen.push_back(&n_tani);              // объект, который тянется, перемещается
		master_chosen.push_back(&n_fun_tani0);         // АБСТРАКТНЫЙ ПРЕДОК функция начала тяни-толкай
		master_chosen.push_back(&n_fun_tani);          // АБСТРАКТНЫЙ ПРЕДОК функция тяни-толкай
		master_chosen.push_back(&n_fun_tani1);         // АБСТРАКТНЫЙ ПРЕДОК функция конца тяни-толкай
		master_chosen.push_back(&n_temp_go);           // АБСТРАКТНЫЙ ПРЕДОК временный графический объект
		master_chosen.push_back(&n_center);            // АБСТРАКТНЫЙ ПРЕДОК центр
		master_chosen.push_back(&n_radius);            // АБСТРАКТНЫЙ ПРЕДОК радиус
		master_chosen.push_back(&n_width);             // АБСТРАКТНЫЙ ПРЕДОК толщина
		master_chosen.push_back(&n_begin);             // АБСТРАКТНЫЙ ПРЕДОК начало
		master_chosen.push_back(&n_end);               // АБСТРАКТНЫЙ ПРЕДОК конец
		master_chosen.push_back(&n_mouse_inactive);    // АБСТРАКТНАЯ ИНФОРМАЦИЯ неактивный для перемещения мышки
		master_chosen.push_back(&n_start_mouse_move);  // АБСТРАКТНЫЙ ПРЕДОК функция начала перемещения мышки над объектом
		master_chosen.push_back(&n_mouse_move);        // АБСТРАКТНЫЙ ПРЕДОК функция перемещения мышки над объектом
		master_chosen.push_back(&n_finish_mouse_move); // АБСТРАКТНЫЙ ПРЕДОК функция конца перемещения мышки над объектом
		master_chosen.push_back(&n_fun_up_middle);     // АБСТРАКТНЫЙ ПРЕДОК функция отжато колесо мышки
		master_chosen.push_back(&n_timer250);          // АБСТРАКТНЫЙ ПРЕДОК функция отжато колесо мышки
		master_chosen.push_back(&n_cc0);               // цвет фона
		master_chosen.push_back(&n_cc2);               // цвет сс2

		return load_from_txt_file(fn);
	}

	double get_main_scale()
	{
		_t_trans* tr = *n_ko;
		return tr->trans.scale;
	}

	void mouse_button_left(bool pressed)
	{
		*n_s_left->operator i64* () = pressed;
		if (pressed) n_down_left->run(0, n_down_left, flag_run); else n_up_left->run(0, n_up_left, flag_run);
	}

	void mouse_button_right(bool pressed)
	{
		*n_s_right->operator i64* () = pressed;
		if (pressed) n_down_right->run(0, n_down_right, flag_run); else n_up_right->run(0, n_up_right, flag_run);
	}

	void mouse_button_middle(bool pressed)
	{
		*n_s_middle->operator i64* () = pressed;
		if (pressed) n_down_middle->run(0, n_down_middle, flag_run); else n_up_middle->run(0, n_up_middle, flag_run);
	}

}