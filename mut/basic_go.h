#pragma once

#include "tetron.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct _t_basic_go : public _tetron
{
	_area2 area; // визуальная область полная
	bool area_definite = false; // area задана

	operator _t_basic_go* () { return this; }
	void after_create_link(_link* li);          // вызывается после создания связи
	void before_delete_link(_link* li);         // вызывается перед удалением связи
	_area2 calc_area();                          // вычислить область
	void cha_area(_area2 a = {}, bool first = true); // эта область изменена внутри
	void cha_area(_trans tr); // эта область изменена внутри c известной трансформацией
	void add_area(_area2 a = {}, bool first = true);         // эта область добавлена
	void del_area(_area2 a = {}, bool first = true);         // эта область удалена
	void find_pot_act(_coo2 r);                           // найти потенциально активный
	virtual bool mouse_move(_trans tr, bool final) = 0; // перемещение мышки
	virtual void ris(_trans tr, bool final) = 0; // нарисовать
	bool mouse_down_left(_trans tr);                    // щелчок мышки
	bool mouse_wheel(_trans tr);                        // повернуто колесо мышки
	_trans oko_trans(bool* ko = 0); // абсолютная трансформация (*ko - докопался до корневого объекта)
	bool final_fractal(const _trans& tr); // является ли данный тетрон последним?
	virtual void clear_go_rod() {}        // !!! нет универсальности, придумать как лучше
	virtual void priem_gv();              // сюда перетащен гвоздь активного элемента
	virtual _t_trans* ttrans() { return nullptr; } // ищет первый тетрон с трансофрмацией, указывающий на этот _t_go
	virtual double final_radius() { return 8.0; } // минимальный полуразмер, после которого не рисуется структура
	void set_layer(double n); // задать слой
	_t_trans* set_t_trans(_tetron* go, uint64 flags); // именяет прокладочный тетрон ??
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct _t_trans : public _t_basic_go
{
	_trans trans;

	~_t_trans() { if (run_before_del_link) del_area(); }

	uchar     type()               override { return 5; }
	int       get_froglif()        override { return 0xF0; }
	void      push(_stack* mem)    override { _t_basic_go::push(mem); *mem << trans; }
	void      pop(_stack* mem)     override { _t_basic_go::pop(mem); *mem >> trans; }
	void      push(_wjson& b)      override { _t_basic_go::push(b); b.add("trans", trans); }
	void      pop(_rjson& b)       override { _t_basic_go::pop(b); trans = b.read_trans("trans"); }
	_t_trans* ttrans()             override { return this; } // ищет первый тетрон с трансофрмацией, указывающий на этот _t_go

	operator _t_trans* ()          override { return this; }

	void clear_go_rod() override; // удалить все графические родительские связи
	bool mouse_move(_trans tr, bool final) override; // перемещение мышки
	void ris(_trans tr, bool final) override; // нарисовать
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct _t_go : public _t_basic_go
{
	//	static constexpr uint c1_default = c_def; // цвет рисования по умолчанию
	static constexpr uint c2_default = 0; // цвет фона по умолчанию

	_area2 local_area; // область только этого объекта
	bool key_fokus; // делать активным для управления клавиатурой

	uint get_c();
	void set_c(uint c);
	uint get_c2();
	void set_c2(uint c);
	_t_go();
	~_t_go() { if (run_before_del_link) del_area(); }
	operator _t_go* () { return this; }

	void push(_stack* mem) override { _t_basic_go::push(mem); *mem << local_area; }
	void pop(_stack* mem)  override { _t_basic_go::pop(mem); *mem >> local_area; }
	void push(_wjson& b)   override { _t_basic_go::push(b); b.add("local_area", local_area); }
	void pop(_rjson& b)    override { _t_basic_go::pop(b); local_area = b.read_area2("local_area"); }

	virtual void ris2(_trans tr, bool final = false) = 0; // нарисовать индивидуальное
	virtual void mega_ris() { cha_area(); } // перерисовать с измененными параметрами
	virtual bool mouse_wheel2(_coo2 r) { return false; } // повернуто колесо мышки действие
	virtual bool mouse_move2(_coo2 r); // перемещение мышки действие
	virtual void mouse_finish_move(); // мышка ушла
	virtual bool mouse_down_left2(_coo2 r); // начало перетаскивания
	virtual void mouse_move_left2(_coo2 r); // процесс перетаскивания
	virtual void mouse_up_left2(_coo2 r); // конец перетаскивания
	void mouse_up_middle(); // отпущено колесо мышки
	virtual void key_down(ushort key) {} // нажата кнопка
	virtual void key_press(ushort key) {} // введен символ
	virtual void resize(); // был изменен local_area_
	virtual bool test_local_area(_coo2 b) { return local_area.test(b); } // лежит ли точка внутри
	_t_trans* ttrans(); // ищет первый тетрон с трансофрмацией, указывающий на этот _t_go
	void clear_go_rod(); // удалить все промежуточные трансформации
	bool mouse_move(_trans tr, bool final) override; // перемещение мышки
	void ris(_trans tr, bool final) override; // нарисовать
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct _chain_go // цепочка графических объектов с множественными петлями
{
	struct para
	{
		int k = 0;
		_trans tr;
	};

	__hash_table<_pair_t<para> > hash;
	std::vector<_t_basic_go*> chain;

	void clear() { chain.clear(); hash.clear(); }
	void push(_t_basic_go* a, _trans& tr);
	void pop();
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct _layers_go // для цикла по слоям графических объектов
{
	_layers_go(_tetron* t);
	void operator++(int);
	operator bool() { return tetron2; }
	operator _t_basic_go* () { return tetron2; }
	_t_basic_go* operator->() { return tetron2; }

private:
	_tetron* tetron = nullptr;
	std::multimap<double, _id> map_;
	std::map<double, _id>::iterator i_m;
	_t_basic_go* tetron2 = nullptr;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct _g_circle : public _t_go
{
	_g_circle() { calc_local_area(); }

	uchar type()            override { return 10; }
	int   get_froglif()     override { return 0x74; }
	void  push(_stack* mem) override { _t_go::push(mem); *mem << center << radius << width; }
	void  pop(_stack* mem)  override { _t_go::pop(mem); *mem >> center >> radius >> width; }

	operator _g_circle* ()  override { return this; }

	void push(_wjson& b)                              override;
	void pop(_rjson& b)                               override;
	void ris2(_trans tr, bool final)                 override;
	void run(_tetron* tt0, _tetron* tt, uint64 flags) override;
	bool test_local_area(_coo2 b)                     override; // лежит ли точка внутри

	void calc_local_area();

private:
	_coo2 center{ 0,0 };
	double radius{ 20 };
	double width{ 2 };
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct _g_froglif : public _t_go
{
	static constexpr int size_f = 43;
	static constexpr int r_f = 4;
	union
	{
		uchar f[size_f]{};
		int f_int;
		int64 f_int64;
	};

	_g_froglif() { local_area = { {0, 24}, {0, 24} }; }

	uchar type()               override { return 14; }
	int   get_froglif()        override { return 0x7F; }
	void  push(_stack* mem)    override { _t_go::push(mem); mem->push_data(f, size_f); }
	void  pop(_stack* mem)     override { _t_go::pop(mem); mem->pop_data(f, size_f); }
	void  push(_wjson& b)      override { _t_go::push(b); b.add_mem("f", f, size_f); }
	void  pop(_rjson& b)       override { _t_go::pop(b); b.read_mem("f", f, size_f); }

	operator int64* ()         override { return &f_int64; }

	void ris2(_trans tr, bool final) override;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct _g_line : public _t_go
{
	_g_line() { calc_local_area(); }

	uchar type()            override { return 11; }
	int   get_froglif()     override { return 0x70; }
	void  push(_stack* mem) override { _t_go::push(mem); *mem << p1 << p2 << width; }
	void  pop(_stack* mem)  override { _t_go::pop(mem); *mem >> p1 >> p2 >> width; }

	void push(_wjson& b)                              override;
	void pop(_rjson& b)                               override;
	void ris2(_trans tr, bool final)                 override;
	void run(_tetron* tt0, _tetron* tt, uint64 flags) override;
	bool test_local_area(_coo2 b)                     override; // лежит ли точка внутри

	void calc_local_area();

private:
	_coo2 p1{ 0, 0 }, p2{ 100, 100 }; // начало и конец
	double width{ 1 };
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct _g_picture : public _t_go
{
	_picture pic;

	_g_picture() { local_area = { {0, 100}, {0, 100} }; }

	uchar type()            override { return 31; }
	int   get_froglif()     override { return 0xF6; }
	void  push(_stack* mem) override { _t_go::push(mem); *mem << pic; }
	void  pop(_stack* mem)  override { _t_go::pop(mem); *mem >> pic; }
	void  push(_wjson& b)   override { _t_go::push(b); b.add("pic", pic); }
	void  pop(_rjson& b)    override { _t_go::pop(b); b.read("pic", pic); }

	operator _g_picture* () override { return this; }
	operator _picture* ()   override { return &pic; }

	void ris2(_trans tr, bool final) override;

	void new_size(int rx3, int ry3);
	bool load_from_file(std::filesystem::path fn);
	bool save_to_file(std::filesystem::path fn) const;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct _g_text : public _t_go
{
	std::wstring s = L"";

	_g_text() { local_area = { {0, 13}, {0, 13} }; }

	uchar type()              override { return 29; }
	int   get_froglif()       override { return 0xF1; }
	bool  mouse_move2(_coo2 r)  override { return false; }
	void  push(_stack* mem)   override { _t_go::push(mem); *mem << s; }
	void  pop(_stack* mem)    override { _t_go::pop(mem); *mem >> s; }
	void  push(_wjson& b)     override { _t_go::push(b); b.add("s", s); }
	void  pop(_rjson& b)      override { _t_go::pop(b); b.read("s", s); }

	operator std::wstring* () override { return &s; }
	operator _g_text* ()      override { return this; }

	void ris2(_trans tr, bool final) override;

	void set_text(std::wstring_view s2);
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct _g_rect : public _t_go
{
	_g_rect() { local_area = { {0, 100}, {0, 100} }; }

	uchar type()         override { return 12; }
	int   get_froglif()  override { return 0xF4; }

	operator _g_rect* () override { return this; }

	void ris2(_trans tr, bool final) override;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct _g_test_graph : public _t_go // !! НЕ УДАЛЯТЬ, ПРИГОДИТСЯ !!
{
	_picture a;

	_g_test_graph();
	uchar type() { return 6; }
	int get_froglif() { return 0xF2; }
	void ris2(_trans tr, bool final) override;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct _g_scrollbar : public _t_go
{
	double position = 0; // положение стрелки [0-1]
	char   vid = 0; // 0 - горизонтальный 1 - вертикальный 2 - снизу 3 - справа 4 - сверху 5 - слева

	_g_scrollbar() { local_area = { {0, 10}, {0, 10} }; }
	~_g_scrollbar() { calc_area(); }

	operator double* ()                override { return &position; }
	operator _g_scrollbar* ()          override { return this; }

	uchar type()                       override { return 16; }
	int   get_froglif()                override { return 0xF8; }
	void  push(_stack* mem)            override { _t_go::push(mem); *mem << position << vid; }
	void  pop(_stack* mem)             override { _t_go::pop(mem); *mem >> position >> vid; }
	bool  mouse_down_left2(_coo2 r)    override { mouse_move_left2(r); return true; }
	void  mouse_up_left2(_coo2 r)      override {}

	void  push(_wjson& b)              override;
	void  pop(_rjson& b)               override;
	void  mouse_move_left2(_coo2 r)    override;
	void  ris2(_trans tr, bool final)  override;
	void  after_create_link(_link* li) override;

	void  prilip(_t_go* r); // прилипание к графическому объекту
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

extern bool      time_ris;             // отображать время рисования
extern _trans    master_trans_go;      // трансформация тяни-толкай объекта, или объекта под мышкой
extern _trans    master_trans_go_move; // трансформация n_go_move
extern _coo2     par_koo1;             // .....вспомогательная переменная  !!!ИЗБАВИТЬСЯ!!!
extern _chain_go master_chain_go;      // активная цепочка графических объектов

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void del_hint();                                 // удалить подсказку
void add_hint(std::wstring_view hint, _t_go* g); // создать подсказку
void change_hint(std::wstring_view hint);        // изменить подсказку

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

