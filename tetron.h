#pragma once

#include <map>

#include "mgraphics.h"
#include "mjson.h"

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
struct _one_tetron;
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
	virtual operator _one_tetron   * () { return nullptr; }

private:
	typedef void (*func_fl)(u64&, u64);
	void set2_flags(_tetron* t, u64 flags, func_fl trans, bool after); // изменяет флаги, может или создать или удалить связь
};

struct _id
{
	u64 id = 0;

	_id() = default;
	_id(u64 b)   : id(b)                      {}
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
	_tetron* tetron_before = nullptr;
	_tetron* tetron_after = nullptr;
	_tetron* tetron_intermediate = nullptr; // искомый тетрон
	u64 flags_before = 0;
	u64 flags_after = 0;
	u64 number = 0; // порядковый номер

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
	operator bool() { return tetron2; }
	operator _tetron* () { return tetron2; }
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

extern __hash_table<_link> link; // ??? если сделать inline - будут ошибки при закрытии

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
	void  pop(_stack* mem)  override { _tetron::pop(mem); *mem >> a; }
	void  push(_wjson& b)   override { _tetron::push(b); b.add("a", a); }
	void  pop(_rjson& b)    override { _tetron::pop(b); b.read("a", a); }
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct _t_double : public _tetron
{
	double a = 0.0;

	operator _t_double* ()  override { return this; }

	uchar type()            override { return 7; }
	int   get_froglif()     override { return 0x50; }
	void  push(_stack* mem) override { _tetron::push(mem); *mem << a; }
	void  pop(_stack* mem)  override { _tetron::pop(mem); *mem >> a; }
	void  push(_wjson& b)   override { _tetron::push(b); b.add("a", a); }
	void  pop(_rjson& b)    override { _tetron::pop(b); b.read("a", a); }

	operator double* ()     override { return &a; }
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct _t_string : public _tetron
{
	std::wstring s;

	uchar type()              override { return 24; }
	int   get_froglif()       override { return 0x10; }
	void  push(_stack* mem)   override { _tetron::push(mem); *mem << s; }
	void  pop(_stack* mem)    override { _tetron::pop(mem); *mem >> s; }
	void  push(_wjson& b)     override { _tetron::push(b); b.add("s", s); }
	void  pop(_rjson& b)      override { _tetron::pop(b); b.read("s", s); }

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
	void  pop(_stack* mem)     override { _tetron::pop(mem); s.pop(mem); }
	void  push(_wjson& b)      override { _tetron::push(b); b.add("s", s); }
	void  pop(_rjson& b)       override { _tetron::pop(b); b.read("s", s); }

	operator _multi_string* () override { return &s; }
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct _one_tetron : public _tetron
{
	union
	{
		i64 i[16]{};
		u64 ui[16];
		double d[16];
		wchar_t s[64];
		uchar c[128];
	};

	_one_tetron() = default;
	_one_tetron(double p1) : d{ p1 } {}
	_one_tetron(double p1, double p2) : d{ p1, p2 } {}

	uchar type()             override { return 13; }
	int   get_froglif()      override { return 0xAF; }
	void  push(_stack* mem)  override { _tetron::push(mem); mem->push_data(c, sizeof(c)); }
	void  pop(_stack* mem)   override { _tetron::pop(mem); mem->pop_data(c, sizeof(c)); }

	void  push(_wjson& b)    override { _tetron::push(b); b.add_mem("a", c, 128); }
	void  pop(_rjson& b)     override { _tetron::pop(b); b.read_mem("a", c, 128); }

	operator _one_tetron* () override { return this; }
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

/*	void  pop(_rjson& b)     override
	{ 
		double d[16];
		_tetron::pop(b);
		b.read_mem("a", d, 128);
		a.x = d[0];
		a.y = d[1];
	}*/
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
