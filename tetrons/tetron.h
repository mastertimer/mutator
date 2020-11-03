#pragma once

#include <map>

#include "basic.h"

constexpr uint c_background = 0xFF000000;
constexpr uint c_maxx       = 0xFFA0FFC0;
constexpr uint c_max        = 0xFF40FF80;
constexpr uint c_def        = 0xFF208040;
constexpr uint c_min        = 0xFF104020;
constexpr uint c_minn       = 0xFF082010;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

enum class _cursor { normal, size_all, hand_point, size_we, size_ns, drag }; // виды курсора

inline _cursor g_cursor = _cursor::normal; // установленный курсор

void set_cursorx(_cursor x);

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

inline u64 id_tetron = 1; // глобальный счетчик id тетронов
inline bool run_before_del_link = true; // вызывать ли спец функции перед удалением связи
inline _area master_obl_izm; // область изменений
inline _bitmap master_bm; // кусочный рисунок

inline _xy mouse_xy; // координаты мышки
inline _xy mouse_xy_pr; // предыдущие координаты мышки

inline u64 inverted_flags(u64 a) { return (a << 32) | (a >> 32); }

constexpr u64 flag_parent       = 0x001;
constexpr u64 flag_part         = 0x002;
constexpr u64 flag_run          = 0x004;
constexpr u64 flag_sub_go       = 0x010;
constexpr u64 flag_information  = 0x020;
constexpr u64 flag_information2 = 0x040;
constexpr u64 flag_specialty    = 0x080;
constexpr u64 flag_specialty2   = 0x100;

struct _link;
struct _tetron;
struct _t_function;
struct _t_trans;
struct _t_string;
struct _t_basic_go;
struct _t_int;
struct _t_double;
struct _t_go;
struct _t_xy;
struct _g_scrollbar;
struct _g_text;
struct _g1list;
struct _g_tetron;
struct _g_rect;
struct _g_picture;
struct _g_edit64bit;
struct _g_list_link;
struct _g_button;
struct _g_circle;

inline _hash_map<u64, _tetron*> all_tetron;

template <typename _t>
struct _pair_t
{
	_tetron* tetron = nullptr;
	_t a = {};

	_pair_t() = default;
	_pair_t(_tetron* b) : tetron(b) {}
	_pair_t(_tetron* b, const _t& c) : tetron(b), a(c) {}
	bool operator == (_tetron* b) const noexcept { return (tetron == b); }
	bool operator == (const _pair_t& b) const noexcept { return (tetron == b.tetron); }
};

typedef __hash_table<_pair_t<i64> > _hash_table_tetron;
typedef std::vector<_tetron*> _vector_tetron;

struct _id;

typedef std::vector<_id> _vector_id;

struct _tetron
{
	std::vector<_link*> link; // указатели на связи
	u64 id; // уникальный идентификатор в пространстве и времени

	_tetron();
	virtual ~_tetron();
	virtual uchar type() { return 0; }
	std::string name();
	virtual int get_froglif() { return 0x20; }
	virtual void push(_stack* mem) { }
	virtual void pop(_stack* mem) { }
	virtual void push(_wjson &b) { }
	virtual void pop(_rjson& b) { }
	virtual void run(_tetron* tt0, _tetron* tt, u64 flags); // выполнить, tt0 - вызывающий тетрон, tt - от чего имени

	u64 get_flags(_tetron* t); // если связи нет --> 0
	bool test_flags(_tetron* t, u64 f) { return ((get_flags(t) & f) == f); }
	void add_flags(_tetron* t, u64 flags, bool after = true) // изменяет флаги, может создать связь
	{
		set2_flags(t, flags, [](u64& f, u64 f2) { f |= f2; }, after);
	}
	void del_flags(_tetron* t, u64 flags, bool after = true) // изменяет флаги, может удалить связь
	{
		set2_flags(t, flags, [](u64& f, u64 f2) { f &= (~f2); }, after);
	}
	void xor_flags(_tetron* t, u64 flags, bool after = true) // изменяет флаги, может или создать или удалить связь
	{
		set2_flags(t, flags, [](u64& f, u64 f2) { f ^= f2; }, after);
	}

	virtual void add_unique_flags(_tetron* t, u64 flags, bool after = true); // создать уникальную связь
	virtual void after_create_link(_link* li) {} // выз. после создания связи
	virtual void before_delete_link(_link* li) {} // выз. перед удалением связи
	void traversal(_hash_table_tetron* ht, u64 flags, _vector_tetron* lt = 0);

	void copy(_tetron* a); // присвоение содержимого
	_tetron* copy_plus(); // копировать с хвостами

	template <typename _t> _t* find1(u64 flags); // найти указатель нужного типа на глубине 1, с заданными флагами  
	template <typename _t> _t* find_intermediate(_tetron* t, u64 flags_before, u64 flags_after); // промежуточный
	void find_all_intermediate(_tetron* t, u64 flags_before, u64 flags_after, _vector_id& res); // все промежуточные (у родителей тоже)

	        operator _tetron       * () { return this; }
	virtual operator double        * () { return nullptr; }
	virtual operator i64           * () { return nullptr; }
	virtual operator _xy           * () { return nullptr; }
	virtual operator std::wstring  * () { return nullptr; }
	virtual operator _t_xy         * () { return nullptr; }
	virtual operator _t_string     * () { return nullptr; }
	virtual operator _multi_string * () { return nullptr; }
	virtual operator _picture      * () { return nullptr; }
	virtual operator _t_function   * () { return nullptr; }
	virtual operator _t_go         * () { return nullptr; }
	virtual operator _t_int        * () { return nullptr; }
	virtual operator _t_double     * () { return nullptr; }
	virtual operator _g_scrollbar  * () { return nullptr; }
	virtual operator _g_text       * () { return nullptr; }
	virtual operator _g1list       * () { return nullptr; }
	virtual operator _g_tetron     * () { return nullptr; }
	virtual operator _g_circle     * () { return nullptr; }
	virtual operator _g_picture    * () { return nullptr; }
	virtual operator _g_edit64bit  * () { return nullptr; }
	virtual operator _g_list_link  * () { return nullptr; }
	virtual operator _g_button     * () { return nullptr; }
	virtual operator _g_rect       * () { return nullptr; }
	virtual operator _t_trans      * () { return nullptr; }
	virtual operator _t_basic_go   * () { return nullptr; }

private:
	typedef void (*func_fl)(u64&, u64);
	void set2_flags(_tetron* t, u64 flags, func_fl trans, bool after); // изменяет флаги, может или создать или удалить связь
};

struct _id
{
	u64 id = 0;

	_id() = default;
	_id(u64 b)      : id(b)                      {}
	_id(_tetron* b) : id(b->id)                  {}
	void operator = (_id b)                      { id = b.id; };
	void operator = (_tetron* b)                 { id = (b) ? b->id : 0; };

	operator _tetron* ()          const noexcept { return *all_tetron.find(id); }
	_tetron* operator->()         const noexcept { return *all_tetron.find(id); }
	operator bool()               const noexcept { return  all_tetron.find(id); }

	bool operator != (_tetron* b) const noexcept { return (b) ? (id != b->id) : (id != 0); }
	bool operator != (_id b)      const noexcept { return id != b.id; }
	bool operator == (_tetron* b) const noexcept { return (b) ? (id == b->id) : (id == 0); }
	bool operator == (_id b)      const noexcept { return id == b.id; }
};

inline std::vector<_id*> master_chosen; // дублирование особых связей

inline std::map<double, _id> master_layers; // тетроны слоёв

inline _id n_ko;                // !!корневой графический объект (трансформация)
inline _id n_s_shift;           // !!зажата клавиша Shift
inline _id n_s_alt;             // !!зажата клавиша Alt
inline _id n_s_ctrl;            // !!зажата клавиша Ctrl
inline _id n_s_left;            // зажата левая кнопка мышки
inline _id n_s_right;           // зажата правая кнопка мышки
inline _id n_s_middle;          // зажато колесо мышки
inline _id n_s_double;          // двойной щелчок мышки
inline _id n_down_left;         // нажата левая кнопка мышки
inline _id n_down_right;        // нажата правая кнопка мышки
inline _id n_down_middle;       // нажато колесо мышки
inline _id n_up_left;           // отжата левая кнопка мышки
inline _id n_up_right;          // отжата правая кнопка мышки
inline _id n_up_middle;         // отжато колесо мышки
inline _id n_move;              // перемещен курсор мышки
inline _id n_wheel;             // повернуто колесо мышки
inline _id n_hint;              // подсказка для элемента под курсором мышки
inline _id n_hex;               // АБСТРАКТНАЯ ИНФОРМАЦИЯ 16-ричная система счисления
inline _id n_go_move;           // !!цепочка!! GO, над которым перемещается мышка
inline _id n_perenos;           // режим переноса
inline _id n_move_all;          // вызывается после глобального смещения или масштабирования
inline _id n_ramk;              // рамка выделения потенциально активного
inline _id n_ramk2;             // рамка выделения активного
inline _id n_pot_act;           // потенциально активный тетрон
inline _id n_act;               // активный тетрон
inline _id n_zagolovok;         // АБСТРАКТНЫЙ ПРЕДОК  - заголовки
inline _id n_checkbox;          // АБСТРАКТНЫЙ ПРЕДОК  - переключатель
inline _id n_go_layer;          // АБСТРАКТНЫЙ ПРЕДОК  - графический слой
inline _id n_color_line;        // АБСТРАКТНЫЙ ПРЕДОК  - цвет линий и текста
inline _id n_color_bg;          // АБСТРАКТНЫЙ ПРЕДОК  - цвет фона
inline _id n_act_key;           // активный тетрон для управления клавиатурой
inline _id n_down_key;          // нажата кнопка клавиатуры
inline _id n_press_key;         // введен символ
inline _id n_timer1000;         // таймер с периодом 1000
inline _id n_tani;              // объект, который тянется, перемещается
inline _id n_fun_tani0;         // АБСТРАКТНЫЙ ПРЕДОК  функция начала тяни-толкай
inline _id n_fun_tani;          // АБСТРАКТНЫЙ ПРЕДОК  функция тяни-толкай
inline _id n_fun_tani1;         // АБСТРАКТНЫЙ ПРЕДОК  функция конца тяни-толкай
inline _id n_temp_go;           // АБСТРАКТНЫЙ ПРЕДОК  временный графический объект
inline _id n_center;            // АБСТРАКТНЫЙ ПРЕДОК  центр
inline _id n_radius;            // АБСТРАКТНЫЙ ПРЕДОК  радиус
inline _id n_width;             // АБСТРАКТНЫЙ ПРЕДОК  толщина
inline _id n_begin;             // АБСТРАКТНЫЙ ПРЕДОК  начало
inline _id n_end;               // АБСТРАКТНЫЙ ПРЕДОК  конец
inline _id n_mouse_inactive;    // АБСТРАКТНАЯ ИНФОРМАЦИЯ неактивный для перемещения мышки
inline _id n_start_mouse_move;  // АБСТРАКТНЫЙ ПРЕДОК  функция начала перемещения мышки над объектом
inline _id n_mouse_move;        // АБСТРАКТНЫЙ ПРЕДОК  функция перемещения мышки над объектом
inline _id n_finish_mouse_move; // АБСТРАКТНЫЙ ПРЕДОК  функция конца перемещения мышки над объектом
inline _id n_fun_up_middle;     // АБСТРАКТНЫЙ ПРЕДОК  функция отжато колесо мышки
inline _id n_timer250;          // таймер с периодом 1000

struct _pair_tetron // пара тетронов
{
	_tetron* low_tetron = nullptr; // тетрон с меньшим адресом
	_tetron* high_tetron = nullptr; // тетрон с большим адресом

	_pair_tetron() = default;
	_pair_tetron(_tetron* t1, _tetron* t2) : low_tetron(t1), high_tetron(t2)
	{
		if (t1 > t2) { low_tetron = t2;  high_tetron = t1; }
	}

	bool operator==(const _pair_tetron& pt) { return (low_tetron == pt.low_tetron) && (high_tetron == pt.high_tetron); }

	_tetron* operator()(_tetron* t) { return (t == low_tetron) ? high_tetron : low_tetron; }
};

struct _link : public _pair_tetron
{
	uint n_low = 0; // номер в списке
	uint n_high = 0; // номер в списке

	_link() = default;
	~_link();
	_link(const _link& a) = delete;
	_link(_link&& a) = delete;
	_link& operator = (const _link& a) = delete;
	void operator = (_link&& a) noexcept;

	u64 get_flags(_tetron* t) { return (t == low_tetron) ? flags : inverted_flags(flags); }

	bool test_flags(_tetron* const t, u64 flags_)
	{
		if (t != low_tetron) flags_ = inverted_flags(flags_);
		return ((flags & flags_) == flags_);
	}

	void set_flags(_tetron* t, u64 f)
	{
		if (f == 0)	{ this->~_link(); return; }
		flags = (t == low_tetron) ? f : inverted_flags(f);
	}

	template <typename _t> _t* get(_tetron* t, u64 f);

private:
	u64 flags = 0; // флаги
};

struct _he_intermediate
{
	_tetron* tetron_before       = nullptr;
	_tetron* tetron_after        = nullptr;
	_tetron* tetron_intermediate = nullptr; // искомый тетрон
	u64 flags_before = 0;
	u64 flags_after  = 0;
	u64 number       = 0; // порядковый номер

	_he_intermediate() = default;
	_he_intermediate(_tetron* t1, _tetron* t2, u64 f1, u64 f2) : tetron_before(t1), tetron_after(t2),
		flags_before(f1), flags_after(f2) {};
	_he_intermediate(_tetron* t1, _tetron* t2, _tetron* ti, u64 f1, u64 f2, u64 n) : tetron_before(t1),
		tetron_after(t2), tetron_intermediate(ti), flags_before(f1), flags_after(f2), number(n) {};
	bool operator==(const _he_intermediate& b) {
		return (tetron_before == b.tetron_before) && (tetron_after == b.tetron_after) &&
			(flags_before == b.flags_before) && (flags_after == b.flags_after);
	}
};

struct _frozen // для цикла по слепку связей
{
	_frozen(_tetron* t, u64 flags_);

	void operator++(int);
	operator bool()       { return tetron2; }
	operator _tetron* ()  { return tetron2; }
	_tetron* operator->() { return tetron2; }

private:
	_tetron* tetron;
	_speed<_vector_id> lt;
	uint i;
	_tetron* tetron2;
	u64 flags;
};

inline __hash_table <_he_intermediate> hash_intermediate;
inline u64 number_intermediate = 0;

extern __hash_table<_link> glink; // ??? если сделать inline - будут ошибки при закрытии

void delete_hvost(_tetron* t, bool del_t = false, bool run_func = true);
void optimize_hash_intermediate();
void add_obl_izm(_area a); // добавить область изменения
_tetron* create_tetron(uchar tipt); // определено в другом места

namespace SuperDelTetron2
{
	extern _speed<_vector_tetron> ud; // список на удаление

	void PodgSpUnikSv(_tetron* b);
	void FreeBank();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

uint hash_func(const _pair_tetron& a);
uint hash_func(const _he_intermediate& a);

template <typename _t>
uint hash_func(const _pair_t<_t>& a)
{
	return (uint)((((u64)a.tetron) >> 4) * 27644437);
}

template <typename _t> _t* _link::get(_tetron* t, u64 f)
{
	if (!test_flags(t, f)) return nullptr;
	_t* a = *(*this)(t);
	return a;
}

template <typename _t> _t* _tetron::find_intermediate(_tetron* t, u64 flags_before, u64 flags_after)
{
	auto ii = hash_intermediate.find(_he_intermediate(this, t, flags_before, flags_after));
	if (ii)
	{ // !!!!!!!!!!!!!!!!!! вызываются функции у мёртвых тетронов !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
		if (ii->tetron_before->test_flags(ii->tetron_intermediate, flags_before) &&
			ii->tetron_intermediate->test_flags(ii->tetron_after, flags_after))
		{
			_t* x = *ii->tetron_intermediate;
			if (x)
			{
				ii->number = ++number_intermediate;
				return x;
			}
		}
		hash_intermediate.erase(ii); // delete erase remove
	}
	_t* res = nullptr;
	if (link.size() < t->link.size())
	{
		for (auto i : link)
		{
			_tetron* a = (*i)(this);
			if (!i->test_flags(this, flags_before)) continue;
			_t* x = *a;
			if (x == nullptr) continue;
			if (a->test_flags(t, flags_after))
			{
				res = x;
				break;
			}
		}
	}
	else
		for (auto i : t->link)
		{
			_tetron* a = (*i)(t);
			if (!i->test_flags(t, inverted_flags(flags_after))) continue;
			_t* x = *a;
			if (x == nullptr) continue;
			if (test_flags(a, flags_before))
			{
				res = x;
				break;
			}
		}
	if (res)
	{
		hash_intermediate.insert(_he_intermediate(this, t, (_tetron*)res, flags_before, flags_after, ++number_intermediate));
		optimize_hash_intermediate();
	}
	return res;
}

template <typename _t> _t* _tetron::find1(u64 flags)
{
	for (auto i : link)
	{
		_tetron* a = (*i)(this);
		if (!i->test_flags(this, flags)) continue;
		if (_t* x = *a) return x;
	}
	return nullptr;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct _t_int : public _tetron
{
	i64 a = 0;

	operator i64* ()        override { return &a; }
	operator _t_int* ()     override { return this; }

	uchar type()            override { return 26; }
	int   get_froglif()     override { return 0x40; }
	void  push(_stack* mem) override { _tetron::push(mem); *mem << a; }
	void  pop(_stack* mem)  override { _tetron::pop(mem);  *mem >> a; }
	void  push(_wjson& b)   override { _tetron::push(b);   b.add("a", a); }
	void  pop(_rjson& b)    override { _tetron::pop(b);    b.read("a", a); }
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct _t_double : public _tetron
{
	double a = 0.0;

	operator _t_double* ()  override { return this; }

	uchar type()            override { return 7; }
	int   get_froglif()     override { return 0x50; }
	void  push(_stack* mem) override { _tetron::push(mem); *mem << a; }
	void  pop(_stack* mem)  override { _tetron::pop(mem);  *mem >> a; }
	void  push(_wjson& b)   override { _tetron::push(b);   b.add("a", a); }
	void  pop(_rjson& b)    override { _tetron::pop(b);    b.read("a", a); }

	operator double* ()     override { return &a; }
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct _t_string : public _tetron
{
	std::wstring s;

	uchar type()              override { return 24; }
	int   get_froglif()       override { return 0x10; }
	void  push(_stack* mem)   override { _tetron::push(mem); *mem << s; }
	void  pop(_stack* mem)    override { _tetron::pop(mem);  *mem >> s; }
	void  push(_wjson& b)     override { _tetron::push(b);   b.add("s", s); }
	void  pop(_rjson& b)      override { _tetron::pop(b);    b.read("s", s); }

	operator std::wstring* () override { return &s; }
	operator _t_string* ()    override { return this; }
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct _t_multi_string : public _tetron
{
	_multi_string s;

	uchar type()               override { return 3; }
	int   get_froglif()        override { return 0x90; }
	void  push(_stack* mem)    override { _tetron::push(mem); s.push(mem); }
	void  pop(_stack* mem)     override { _tetron::pop(mem);  s.pop(mem); }
	void  push(_wjson& b)      override { _tetron::push(b);   b.add("s", s); }
	void  pop(_rjson& b)       override { _tetron::pop(b);    b.read("s", s); }

	operator _multi_string* () override { return &s; }
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct _t_function : public _tetron
{
	i64 a = 0;

	_t_function() = default;
	_t_function(i64 b) : a(b) {}

	operator i64* ()         override { return &a; }
	operator _t_function* () override { return this; }

	uchar type()             override { return 32; }
	int get_froglif()        override { return 0xA0; }
	void  push(_stack* mem)  override { _tetron::push(mem); *mem << a; }
	void  pop(_stack* mem)   override { _tetron::pop(mem);  *mem >> a; }
	void  push(_wjson& b)    override { _tetron::push(b);   b.add("a", a); }
	void  pop(_rjson& b)     override { _tetron::pop(b);    b.read("a", a); }

	void run(_tetron* tt0, _tetron* tt, u64 flags) override;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct _t_xy : public _tetron
{
	_xy a = { 0.0, 0.0 };

	operator _t_xy* ()       override { return this; }

	uchar type()             override { return 15; }
	int get_froglif()        override { return 0x30; }
	void  push(_stack* mem)  override { _tetron::push(mem); *mem << a; }
	void  pop(_stack* mem)   override { _tetron::pop(mem);  *mem >> a; }
	void  push(_wjson& b)    override { _tetron::push(b);   b.add("a", a); }
	void  pop(_rjson& b)     override { _tetron::pop(b);    b.read("a", a); }
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct _t_basic_go : public _tetron
{
	_area area; // визуальная область полная
	bool area_definite = false; // area задана

	operator _t_basic_go* () { return this; }
	void after_create_link(_link* li);          // вызывается после создания связи
	void before_delete_link(_link* li);         // вызывается перед удалением связи
	_area calc_area();                          // вычислить область
	void cha_area(_area a = {}, bool first = true); // эта область изменена внутри
	void cha_area(_trans tr); // эта область изменена внутри c известной трансформацией
	void add_area(_area a = {}, bool first = true);         // эта область добавлена
	void del_area(_area a = {}, bool first = true);         // эта область удалена
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
	_t_trans* set_t_trans(_tetron* go, u64 flags); // именяет прокладочный тетрон ??
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct _t_trans : public _t_basic_go
{
	_trans trans;

	~_t_trans() { if (run_before_del_link) del_area(); }

	uchar     type()            override { return 5; }
	int       get_froglif()     override { return 0xF0; }
	void      push(_stack* mem) override { _t_basic_go::push(mem); *mem << trans; }
	void      pop(_stack* mem)  override { _t_basic_go::pop(mem); *mem >> trans; }
	void      push(_wjson& b)   override { _t_basic_go::push(b); b.add("trans", trans); }
	void      pop(_rjson& b)    override { _t_basic_go::pop(b); trans = b.read_trans("trans"); }
	_t_trans* ttrans()          override { return this; } // ищет первый тетрон с трансофрмацией, указывающий на этот _t_go

	operator _t_trans* ()       override { return this; }

	void clear_go_rod() override; // удалить все графические родительские связи
	bool mouse_move(_trans tr, bool final) override; // перемещение мышки
	void ris(_trans tr, bool final) override; // нарисовать
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct _t_go : public _t_basic_go
{
	//	static constexpr uint c1_default = c_def; // цвет рисования по умолчанию
	static constexpr uint c2_default = 0; // цвет фона по умолчанию

	_area local_area; // область только этого объекта
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

struct _g_circle : public _t_go
{
	_g_circle() { calc_local_area(); }

	uchar type()            override { return 10; }
	int   get_froglif()     override { return 0x74; }
	void  push(_stack* mem) override { _t_go::push(mem); *mem << center << radius << width; }
	void  pop(_stack* mem)  override { _t_go::pop(mem); *mem >> center >> radius >> width; }

	operator _g_circle* ()  override { return this; }

	void push(_wjson& b)                           override;
	void pop(_rjson& b)                            override;
	void ris2(_trans tr, bool final)               override;
	void run(_tetron* tt0, _tetron* tt, u64 flags) override;
	bool test_local_area(_xy b)                    override; // лежит ли точка внутри

	void calc_local_area();

private:
	_xy center{ 0,0 };
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
		i64 f_int64;
	};

	_g_froglif() { local_area = { {0, 24}, {0, 24} }; }

	uchar type()            override { return 14; }
	int   get_froglif()     override { return 0x7F; }
	void  push(_stack* mem) override { _t_go::push(mem); mem->push_data(f, size_f); }
	void  pop(_stack* mem)  override { _t_go::pop(mem);  mem->pop_data(f, size_f); }
	void  push(_wjson& b)   override { _t_go::push(b);   b.add_mem("f", f, size_f); }
	void  pop(_rjson& b)    override { _t_go::pop(b);    b.read_mem("f", f, size_f); }

	operator i64* ()       override { return &f_int64; }

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

	void push(_wjson& b)                           override;
	void pop(_rjson& b)                            override;
	void ris2(_trans tr, bool final)               override;
	void run(_tetron* tt0, _tetron* tt, u64 flags) override;
	bool test_local_area(_xy b)                    override; // лежит ли точка внутри

	void calc_local_area();

private:
	_xy p1{ 0, 0 }, p2{ 100, 100 }; // начало и конец
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
	void  pop(_stack* mem)  override { _t_go::pop(mem);  *mem >> pic; }
	void  push(_wjson& b)   override { _t_go::push(b);   b.add("pic", pic); }
	void  pop(_rjson& b)    override { _t_go::pop(b);    b.read("pic", pic); }

	operator _g_picture* () override { return this; }
	operator _picture* ()   override { return &pic; }

	void ris2(_trans tr, bool final) override;

	void new_size(int rx3, int ry3);
	bool load_from_file(wstr fn);
	bool save_to_file(wstr fn) const;
	void set_pic(const _picture& pic2);
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct _g_text : public _t_go
{
	std::wstring s = L"";

	_g_text() { local_area = { {0, 13}, {0, 13} }; }

	uchar type()              override { return 29; }
	int   get_froglif()       override { return 0xF1; }
	bool  mouse_move2(_xy r)  override { return false; }
	void  push(_stack* mem)   override { _t_go::push(mem); *mem << s; }
	void  pop(_stack* mem)    override { _t_go::pop(mem);  *mem >> s; }
	void  push(_wjson& b)     override { _t_go::push(b);   b.add("s", s); }
	void  pop(_rjson& b)      override { _t_go::pop(b);    b.read("s", s); }

	operator std::wstring* () override { return &s; }
	operator _g_text* ()      override { return this; }

	void ris2(_trans tr, bool final) override;

	void set_text(std::wstring_view s2_);
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
	bool  mouse_down_left2(_xy r)      override { mouse_move_left2(r); return true; }
	void  mouse_up_left2(_xy r)        override {}

	void  push(_wjson& b)              override;
	void  pop(_rjson& b)               override;
	void  mouse_move_left2(_xy r)      override;
	void  ris2(_trans tr, bool final)  override;
	void  after_create_link(_link* li) override;

	void  prilip(_t_go* r); // прилипание к графическому объекту
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

extern bool      time_ris;             // отображать время рисования
extern _trans    master_trans_go;      // трансформация тяни-толкай объекта, или объекта под мышкой
extern _trans    master_trans_go_move; // трансформация n_go_move
extern _xy       par_koo1;             // .....вспомогательная переменная  !!!ИЗБАВИТЬСЯ!!!
extern _chain_go master_chain_go;      // активная цепочка графических объектов

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void del_hint();                                 // удалить подсказку
void add_hint(std::wstring_view hint, _t_go* g); // создать подсказку
void change_hint(std::wstring_view hint);        // изменить подсказку

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct _g_edit_double : public _g_rect
{
	int cursor;
	double a;

	_g_edit_double();

	operator double* ()     override { return &a; }

	uchar type()            override { return 33; }
	int   get_froglif()     override { return 0x79; }
	void  push(_stack* mem) override { _g_rect::push(mem); *mem << a; }
	void  pop(_stack* mem)  override { _g_rect::pop(mem);  *mem >> a; }
	void  push(_wjson& b)   override { _g_rect::push(b);   b.add("a", a); }
	void  pop(_rjson& b)    override { _g_rect::pop(b);    b.read("a", a); }

	void ris2(_trans tr, bool final) override;
	void key_down(ushort key)        override;
	void key_press(ushort key)       override;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct _g_edit_int : public _t_go
{
	int cursor = 0;

	_g_edit_int() { key_fokus = true; local_area = { {0, 100}, {0, 16} }; }

	uchar type()                      override { return 4; }
	int   get_froglif()               override { return 0xF9; }

	void  ris2(_trans tr, bool final) override;
	bool  mouse_wheel2(_xy r)         override;
	void  key_down(ushort key)        override;
	void  key_press(ushort key)       override;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct _g_edit_string : public _t_go
{
	int cursor = 0; // абсолютное положение курсора
	int first = 0; // номер первого символа
	int len2 = 0; // длина куска

	_g_edit_string();
	uchar type() { return 25; }
	int get_froglif() { return 0xFB; }
	void ris2(_trans tr, bool final) override;
	void key_down(ushort key);
	void key_press(ushort key);
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct _g_edit_multi_string : public _t_go
{
	_multi_string strings;
	_ixy cursor;
	i64 first = 0; // номер первого элемента в списке

	_g_edit_multi_string();

	uchar type()            override { return 30; }
	int   get_froglif()     override { return 0xFD; }
	void  push(_stack* mem) override { _t_go::push(mem); strings.push(mem); }
	void  pop(_stack* mem)  override { _t_go::pop(mem);  strings.pop(mem); }
	void  push(_wjson& b)   override { _t_go::push(b);   b.add("strings", strings); }
	void  pop(_rjson& b)    override { _t_go::pop(b);    b.read("strings", strings); }

	void ris2(_trans tr, bool final)               override;
	void key_press(ushort key)                     override;
	void key_down(ushort key)                      override;
	bool mouse_down_left2(_xy r)                   override;
	void run(_tetron* tt0, _tetron* tt, u64 flags) override;

private:
	int max_i = 0, len2 = 0; // вспомогательные
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct _g_edit64bit : public _t_go
{
	i64 a;

	_g_edit64bit();
	uchar type() { return 1; }
	int get_froglif() { return 0xFE; }
	void ris2(_trans tr, bool final) override;
	bool mouse_move2(_xy r) override;
	void mouse_finish_move();
	bool mouse_down_left2(_xy r) override;
	operator _g_edit64bit* () { return this; }
	void run(_tetron* tt0, _tetron* tt, u64 flags);

private:
	int act;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct _g_button : public _t_go
{
	i64 checked;
	std::wstring hint;
	_picture picture;

	_g_button();

	operator std::wstring* ()        override { return &hint; }
	operator _picture* ()            override { return &picture; }
	operator i64* ()                 override { return &checked; }
	operator _g_button* ()           override { return this; }

	uchar type()                     override { return 34; }
	int   get_froglif()              override { return 0xF3; }
	void  mouse_up_left2(_xy r)      override { cha_area(); }
	void  mouse_move_left2(_xy r)    override {}

	void ris2(_trans tr, bool final) override;
	bool mouse_move2(_xy r)          override;
	void mouse_finish_move()         override;
	bool mouse_down_left2(_xy r)     override;
	void push(_stack* mem)           override;
	void pop(_stack* mem)            override;
	void push(_wjson& b)             override;
	void pop(_rjson& b)              override;

	void RisIco(astr kod, const char* s);
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct _g_color_ring : public _t_go
{
	_hsva color = c_def;

	_g_color_ring() { local_area = { {0, 300}, {0, 300} }; }
	uchar type() { return 17; }
	int get_froglif() { return 0x72; }
	void ris2(_trans tr, bool final) override;
	bool mouse_down_left2(_xy r)     override { change(r, true); return true; }
	void mouse_move_left2(_xy r)     override { change(r, false); }
	void mouse_up_left2(_xy r)       override {}

private:
	static constexpr double li_r_1 = 0.35;
	static constexpr double li_r_2 = 0.55;
	static constexpr double zazor = (2 * 3.14159265358979323846) * 0.02;
	int iii = 0; // номер четверти при перетаскивании
	void change(_xy r, bool start);
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct _g_list_link : public _g_rect // графический объект - список связей
{
	int cursor = 0; // абсолютный номер элемента
	int first = 0; // номер первого элемента в списке

	uchar type() { return 28; }
	int get_froglif() { return 0xF5; }
	void ris2(_trans tr, bool final) override;
	bool mouse_down_left2(_xy r) override;
	void run(_tetron* tt0, _tetron* tt, u64 flags) override;
	operator _g_list_link* () { return this; }

private:
	_tetron* tf = nullptr; // для какого тетрона отображены флаги
	int curf = 0; // курсор у которого отображены флаги
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct _g1list : public _t_go
{
	_area oo; // область
	bool rez1; // вывод только 1 элемента

	_g1list();

	uchar type()               override { return 2; }
	int   get_froglif()        override { return 0xF7; }
	void  push(_stack* mem)    override { _t_go::push(mem); *mem << oo; }
	void  pop(_stack* mem)     override { _t_go::pop(mem);  *mem >> oo; }
	void  push(_wjson& b)      override { _t_go::push(b);   b.add("oo", oo); }
	void  pop(_rjson& b)       override { _t_go::pop(b);    oo = b.read_area2("oo"); }

	operator _g1list* ()       override { return this; }

	void ris2(_trans tr, bool final) override;
	bool mouse_wheel2(_xy r)         override;
	void mouse_finish_move()         override;
	bool mouse_move2(_xy r)          override;

private:
	void init_sel(); // задать F_sel
	void calc_local_area(); // область только этого объекта
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct _g_tetron : public _t_go
{
	std::wstring hint;
	_id star;

	_g_tetron() { local_area = { {0, 24}, {0, 24} }; }

	uchar  type()            override { return 38; }
	int    get_froglif()     override { return 0xFA; }
	void   push(_stack* mem) override { _t_go::push(mem); *mem << hint; }
	void   pop(_stack* mem)  override { _t_go::pop(mem);  *mem >> hint; }
	void   push(_wjson& b)   override { _t_go::push(b);   b.add("hint", hint); }
	void   pop(_rjson& b)    override { _t_go::pop(b);    b.read("hint", hint); }
	double final_radius()    override { return 21.0; } // минимальный полуразмер, после которого не рисуется

	operator _g_tetron* () { return this; }
	operator std::wstring* () { return &hint; }

	void ris2(_trans tr, bool final) override;
	bool mouse_move2(_xy r)          override;
	void mouse_finish_move()         override;
	bool test_local_area(_xy b)      override; // лежит ли точка внутри
	void add_unique_flags(_tetron* t, u64 flags, bool after = true) override; // создать уникальную связь

private:
	std::vector<_t_basic_go*> drawn_create_star; // стек отрисованных в момент создания звезды

	bool cmp_drawn(); // сравнение стеков
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct _molecule
{
	_id tetron; // тетрон
	_id tr_t;
};

struct _g_tetron2 : public _t_go
{
	std::wstring hint;
	std::vector<_molecule> mo;

	_g_tetron2() { local_area = { {0, 24}, {0, 24} }; }
	uchar type() { return 18; }
	int get_froglif() { return 0xFF; }
	void ris2(_trans tr, bool final) override;
	double final_radius()  override { return 21.0; } // минимальный полуразмер, после которого не рисуется структура
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template <typename _t>
_t* find1_plus_gtetron(_tetron* tet, u64 flags)
{
	for (auto i : tet->link)
	{
		_tetron* a = (*i)(tet);
		if (!i->test_flags(tet, flags)) continue;
		if (_g_tetron* g = *a)
		{
			_t* t2 = g->find1<_t>(flag_specialty);
			if (t2 != nullptr) return t2;
		}
		if (_t* x = *a) return x;
	}
	return nullptr;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct _g_link : public _t_go
{
	static constexpr double dalpha = 0.4;
	static constexpr i64 v_link = 9;

	_xy p11 = { 0, 0 };
	_xy p12 = { 0, 0 };
	_xy p21 = { 0, 0 };
	_xy p22 = { 0, 0 };
	double k = 1.0;
	int act_li = -1;

	uchar type() { return 8; }
	int get_froglif() { return 0x7A; }
	void calc_local_area();
	void ris2(_trans tr, bool final) override;
	bool test_local_area(_xy b) override; // лежит ли точка внутри
	bool mouse_move2(_xy r) override;
	void mouse_finish_move();
	bool mouse_down_left2(_xy r) override;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

