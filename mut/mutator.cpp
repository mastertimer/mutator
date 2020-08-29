/*//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	одинаковые имена глобальный link и _tetron::link
	линию сделать более понятной
	быстро создавать цвета
	правильная альтернатива _g_tetron
	one_tetron удалить

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////*/

#include <functional>

#include "go.h"
#include "mjson.h"
#include "t_function.h"
#include "mjson.h"
#include "set.h"
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
		make(_g_rect),
		make(_one_tetron),
		make(_g_froglif),
		make(_g_edit_one),
		make(_g_scrollbar),
		make(_g_color_ring),
		make(_g_tetron2),
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
		make(_g_tetron),
		make(_g_graph)
	};
	auto fun = ss_tetron.find(name);
	if (fun == ss_tetron.end()) return nullptr;
	return fun->second();
#undef make
}

_tetron* create_tetron(uchar tipt)
{
	switch (tipt)
	{
	case  0: return new _tetron;				// 0x20  32
	case  1: return new _g_edit64bit;			// 0xFE 254
	case  2: return new _g1list;				// 0xF7 247
	case  3: return new _t_multi_string;		// 0x90 144
	case  4: return new _g_edit_int;			// 0xF9 249
	case  5: return new _t_trans;				// 0xF0 240
	case  6: return new _g_test_graph;			// 0xF2 242
	case  7: return new _t_double;				// 0x50  80
	case  8: return new _g_link;				// 0x7A 122
	case  9: return new _g_graph;				// ???? ???
	case 10: return new _g_circle;				// 0x74 116
	case 11: return new _g_line;				// 0x70 112
	case 12: return new _g_rect;				// 0xF4 244
	case 13: return new _one_tetron;			// 0xAF 175
	case 14: return new _g_froglif;				// 0x7F 127
	case 15: return new _g_edit_one;			// 0x71 113
	case 16: return new _g_scrollbar;			// 0xF8 248
	case 17: return new _g_color_ring;          // 0x72 114
	case 18: return new _g_tetron2;				// 0xFF 255

	case 24: return new _t_string;				// 0x10  16
	case 25: return new _g_edit_string;			// 0xFB 251
	case 26: return new _t_int;					// 0x40  64

	case 28: return new _g_list_link;			// 0xF5 245
	case 29: return new _g_text;				// 0xF1 241
	case 30: return new _g_edit_multi_string;	// 0xFD 253
	case 31: return new _g_picture;				// 0xF6 246
	case 32: return new _t_function;			// 0xA0 160
	case 33: return new _g_edit_double;			// 0x79 121
	case 34: return new _g_button;				// 0xF3 243

	case 38: return new _g_tetron;				// 0xFA 250
	}
	return nullptr;
}

namespace mutator
{
	void save_to_txt_file(std::filesystem::path fn)
	{
		_wjson                     tet(fn);
		std::map<uint64, _tetron*> tt; // чтобы упорядочить тетроны
		for (auto i : all_tetron) tt[i->id] = i;
		tet.arr("tetrons");
		for (auto i : tt)
		{
			_tetron* t = i.second;
			tet.str().add("name", t->name()).add("id", i.first);
			t->push(tet);
			tet.arr("links");
			std::map<uint64, _link*> li;
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

	bool load_from_txt_file(std::filesystem::path fn)
	{
		_rjson tet(fn);
		tet.arr("tetrons");
		while (!tet.error)
		{
			tet.obj();
			if (tet.null) break;
			std::string name = tet.read_string("name");
			uint64 my_id;
			tet.read("id", my_id);
			id_tetron = my_id;
			_tetron* tt = create_tetron(name);
			tt->pop(tet);
			tet.arr("links");
			while (!tet.error)
			{
				tet.obj();
				if (tet.null) break;
				uint64 id;
				tet.read("id", id);
				uint64 flags;
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

	void draw(_size2i r)
	{
		if (master_bm.resize(r)) master_obl_izm = r;
		if (master_obl_izm.empty()) return;
		master_bm.set_area(master_obl_izm);
		master_obl_izm &= master_bm.size;
		master_bm.clear(c_background);
		master_chain_go.clear();
		n_ko->operator _t_trans* ()->ris(_trans(), false);
		master_obl_izm.clear();
	}

	bool start(std::filesystem::path fn)
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

		return load_from_txt_file(fn);
	}

	double get_main_scale()
	{
		_t_trans* tr = *n_ko;
		return tr->trans.scale;
	}

	void mouse_button_left(bool pressed)
	{
		*n_s_left->operator int64* () = pressed;
		if (pressed) n_down_left->run(0, n_down_left, flag_run); else n_up_left->run(0, n_up_left, flag_run);
	}

	void mouse_button_right(bool pressed)
	{
		*n_s_right->operator int64* () = pressed;
		if (pressed) n_down_right->run(0, n_down_right, flag_run); else n_up_right->run(0, n_up_right, flag_run);
	}

	void mouse_button_middle(bool pressed)
	{
		*n_s_middle->operator int64* () = pressed;
		if (pressed) n_down_middle->run(0, n_down_middle, flag_run); else n_up_middle->run(0, n_up_middle, flag_run);
	}

}