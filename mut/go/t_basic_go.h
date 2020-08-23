#pragma once

#include "tetron.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

extern bool time_ris; // отображать время рисования
extern _trans master_trans_go;  // трансформация тяни-толкай объекта, или объекта под мышкой
extern _trans master_trans_go_move; // трансформация n_go_move
extern _xy par_koo1; // .....вспомогательная переменная  !!!ИЗБАВИТЬСЯ!!!

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct _t_basic_go : public _tetron
{
	_area_old area; // визуальная область полная

	operator _t_basic_go* () { return this; }
	void after_create_link(_link* li);          // вызывается после создания связи
	void before_delete_link(_link* li);         // вызывается перед удалением связи
	_area_old calc_area();                          // вычислить область
	void cha_area(_area_old a = _tarea::indefinite); // эта область изменена внутри
	void cha_area(_trans tr); // эта область изменена внутри c известной трансформацией
	void add_area(_area_old a = _tarea::indefinite);         // эта область добавлена
	void del_area(_area_old a = _tarea::indefinite);         // эта область удалена
	void find_pot_act(_xy r);                           // найти потенциально активный
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

	_area_old local_area; // область только этого объекта
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
	void pop(_rjson& b)    override { _t_basic_go::pop(b); local_area = b.read_area("local_area"); }

	virtual void ris2(_trans tr, bool final = false) = 0; // нарисовать индивидуальное
	virtual void mega_ris() { cha_area(); } // перерисовать с измененными параметрами
	virtual bool mouse_wheel2(_xy r) { return false; } // повернуто колесо мышки действие
	virtual bool mouse_move2(_xy r); // перемещение мышки действие
	virtual void mouse_finish_move(); // мышка ушла
	virtual bool mouse_down_left2(_xy r); // начало перетаскивания
	virtual void mouse_move_left2(_xy r); // процесс перетаскивания
	virtual void mouse_up_left2(_xy r); // конец перетаскивания
	void mouse_up_middle(); // отпущено колесо мышки
	virtual void key_down(ushort key) {} // нажата кнопка
	virtual void key_press(ushort key) {} // введен символ
	virtual void resize(); // был изменен local_area_
	virtual bool test_local_area(_xy b) { return local_area.test(b); } // лежит ли точка внутри
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

extern _chain_go master_chain_go; // активная цепочка графических объектов
