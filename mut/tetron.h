﻿#pragma once

#include <vector>
#include <map>

#include "mgraphics.h"
#include "mspeed.hpp"
#include "mjson.h"
#include "peach_flavor.h"

inline uint c_background = 0xFF000000;
inline uint c_maxx       = 0xFFA0FFC0;
inline uint c_max        = 0xFF40FF80;
inline uint c_def        = 0xFF208040;
inline uint c_min        = 0xFF104020;
inline uint c_minn       = 0xFF082010;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

inline _cursor g_cursor = _cursor::normal; // установленный курсор

inline uint64 id_tetron = 1; // глобальный счетчик id тетронов
inline bool run_before_del_link = true; // вызывать ли спец функции перед удалением связи
inline _area_old master_obl_izm = _tarea::empty; // область изменений
inline _bitmap master_bm; // кусочный рисунок

inline _peach top_graph; // графика над тетронами

inline _xy mouse_xy; // координаты мышки
inline _xy mouse_xy_pr; // предыдущие координаты мышки

inline uint64 inverted_flags(uint64 a) { return (a << 32) | (a >> 32); }

constexpr uint64 flag_parent       = 0x001;
constexpr uint64 flag_part         = 0x002;
constexpr uint64 flag_run          = 0x004;
constexpr uint64 flag_sub_go       = 0x010;
constexpr uint64 flag_information  = 0x020;
constexpr uint64 flag_information2 = 0x040;
constexpr uint64 flag_specialty    = 0x080;
constexpr uint64 flag_specialty2   = 0x100;

struct _link;
struct _tetron;
struct _one_tetron;
struct _t_function;
struct _t_trans;
struct _t_string;
struct _t_basic_go;
struct _t_int;
struct _t_go;
struct _g_scrollbar;
struct _g_text;
struct _g1list;
struct _g_bitmap;
struct _g_tetron;
struct _g_rect;
struct _g_picture;
struct _g_edit64bit;
struct _g_list_link;
struct _g_button;
struct _g_circle;

inline _hash_map<uint64, _tetron*> all_tetron;

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

typedef __hash_table<_pair_t<int64> > _hash_table_tetron;
typedef std::vector<_tetron*> _vector_tetron;

struct _id;

typedef std::vector<_id> _vector_id;

struct _tetron
{
	std::vector<_link*> link; // указатели на связи
	uint64 id; // уникальный идентификатор в пространстве и времени

	_tetron();
	virtual ~_tetron();
	virtual uchar type() { return 0; }
	std::string name();
	virtual int get_froglif() { return 0x20; }
	virtual void push(_stack* mem) { }
	virtual void pop(_stack* mem) { }
	virtual void push(_wjson &b) { }
	virtual void pop(_rjson& b) { }
	virtual void run(_tetron* tt0, _tetron* tt, uint64 flags); // выполнить, tt0 - вызывающий тетрон, tt - от чего имени

	uint64 get_flags(_tetron* t); // если связи нет --> 0
	bool test_flags(_tetron* t, uint64 f) { return ((get_flags(t) & f) == f); }
	void add_flags(_tetron* t, uint64 flags, bool after = true) // изменяет флаги, может создать связь
	{
		set2_flags(t, flags, [](uint64& f, uint64 f2) { f |= f2; }, after);
	}
	void del_flags(_tetron* t, uint64 flags, bool after = true) // изменяет флаги, может удалить связь
	{
		set2_flags(t, flags, [](uint64& f, uint64 f2) { f &= (~f2); }, after);
	}
	void xor_flags(_tetron* t, uint64 flags, bool after = true) // изменяет флаги, может или создать или удалить связь
	{
		set2_flags(t, flags, [](uint64& f, uint64 f2) { f ^= f2; }, after);
	}

	virtual void add_unique_flags(_tetron* t, uint64 flags, bool after = true); // создать уникальную связь
	virtual void after_create_link(_link* li) {} // выз. после создания связи
	virtual void before_delete_link(_link* li) {} // выз. перед удалением связи
	void traversal(_hash_table_tetron* ht, uint64 flags, _vector_tetron* lt = 0);

	void copy(_tetron* a); // присвоение содержимого
	_tetron* copy_plus(); // копировать с хвостами

	template <typename _t> _t* find1(uint64 flags); // найти указатель нужного типа на глубине 1, с заданными флагами  
	template <typename _t> _t* find_intermediate(_tetron* t, uint64 flags_before, uint64 flags_after); // промежуточный
	void find_all_intermediate(_tetron* t, uint64 flags_before, uint64 flags_after, _vector_id& res); // все промежуточные (у родителей тоже)

	        operator _tetron		* () { return this; }
	virtual operator double			* () { return nullptr; }
	virtual operator int64			* () { return nullptr; }
	virtual operator _xy			* () { return nullptr; }
	virtual operator std::wstring	* () { return nullptr; }
	virtual operator _t_string		* () { return nullptr; }
	virtual operator _multi_string	* () { return nullptr; }
	virtual operator _picture		* () { return nullptr; }
	virtual operator _t_function	* () { return nullptr; }
	virtual operator _t_go			* () { return nullptr; }
	virtual operator _t_int			* () { return nullptr; }
	virtual operator _g_scrollbar	* () { return nullptr; }
	virtual operator _g_text		* () { return nullptr; }
	virtual operator _g_bitmap		* () { return nullptr; }
	virtual operator _g1list		* () { return nullptr; }
	virtual operator _g_tetron		* () { return nullptr; }
	virtual operator _g_circle		* () { return nullptr; }
	virtual operator _g_picture		* () { return nullptr; }
	virtual operator _g_edit64bit	* () { return nullptr; }
	virtual operator _g_list_link	* () { return nullptr; }
	virtual operator _g_button		* () { return nullptr; }
	virtual operator _g_rect		* () { return nullptr; }
	virtual operator _t_trans		* () { return nullptr; }
	virtual operator _t_basic_go	* () { return nullptr; }
	virtual operator _one_tetron	* () { return nullptr; }

private:
	typedef void (*func_fl)(uint64&, uint64);
	void set2_flags(_tetron* t, uint64 flags, func_fl trans, bool after); // изменяет флаги, может или создать или удалить связь
};

struct _id
{
	uint64 id = 0;

	_id() = default;
	_id(uint64 b)   : id(b)                      {}
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

inline std::map<int, _id> master_layers; // тетроны слоёв

inline _id n_ko;				// !!корневой графический объект (трансформация)
inline _id n_s_shift;			// !!зажата клавиша Shift
inline _id n_s_alt;				// !!зажата клавиша Alt
inline _id n_s_ctrl;			// !!зажата клавиша Ctrl
inline _id n_s_left;			// зажата левая кнопка мышки
inline _id n_s_right;			// зажата правая кнопка мышки
inline _id n_s_middle;			// зажато колесо мышки
inline _id n_s_double;			// двойной щелчок мышки
inline _id n_down_left;			// нажата левая кнопка мышки
inline _id n_down_right;		// нажата правая кнопка мышки
inline _id n_down_middle;		// нажато колесо мышки
inline _id n_up_left;			// отжата левая кнопка мышки
inline _id n_up_right;			// отжата правая кнопка мышки
inline _id n_up_middle;			// отжато колесо мышки
inline _id n_move;				// перемещен курсор мышки
inline _id n_wheel;				// повернуто колесо мышки
inline _id n_hint;				// подсказка для элемента под курсором мышки
inline _id n_hex;				// АБСТРАКТНАЯ ИНФОРМАЦИЯ 16-ричная система счисления
inline _id n_go_move;			// !!цепочка!! GO, над которым перемещается мышка
inline _id n_perenos;			// режим переноса
inline _id n_move_all;			// вызывается после глобального смещения или масштабирования
inline _id n_ramk;				// рамка выделения потенциально активного
inline _id n_ramk2;				// рамка выделения активного
inline _id n_pot_act;			// потенциально активный тетрон
inline _id n_act;				// активный тетрон
inline _id n_zagolovok;			// АБСТРАКТНЫЙ ПРЕДОК  - заголовки
inline _id n_checkbox;			// АБСТРАКТНЫЙ ПРЕДОК  - переключатель
inline _id n_go_layer;			// АБСТРАКТНЫЙ ПРЕДОК  - графический слой
inline _id n_color_line;		// АБСТРАКТНЫЙ ПРЕДОК  - цвет линий и текста
inline _id n_color_bg;			// АБСТРАКТНЫЙ ПРЕДОК  - цвет фона
inline _id n_act_key;			// активный тетрон для управления клавиатурой
inline _id n_down_key;			// нажата кнопка клавиатуры
inline _id n_press_key;			// введен символ
inline _id n_timer1000;			// таймер с периодом 1000
inline _id n_tani;				// объект, который тянется, перемещается
inline _id n_fun_tani0;			// АБСТРАКТНЫЙ ПРЕДОК  функция начала тяни-толкай
inline _id n_fun_tani;			// АБСТРАКТНЫЙ ПРЕДОК  функция тяни-толкай
inline _id n_fun_tani1;			// АБСТРАКТНЫЙ ПРЕДОК  функция конца тяни-толкай
inline _id n_temp_go;			// АБСТРАКТНЫЙ ПРЕДОК  временный графический объект
inline _id n_center;			// АБСТРАКТНЫЙ ПРЕДОК  центр
inline _id n_radius;			// АБСТРАКТНЫЙ ПРЕДОК  радиус
inline _id n_width;				// АБСТРАКТНЫЙ ПРЕДОК  толщина
inline _id n_begin;				// АБСТРАКТНЫЙ ПРЕДОК  начало
inline _id n_end;				// АБСТРАКТНЫЙ ПРЕДОК  конец
inline _id n_mouse_inactive;	// АБСТРАКТНАЯ ИНФОРМАЦИЯ неактивный для перемещения мышки
inline _id n_start_mouse_move;	// АБСТРАКТНЫЙ ПРЕДОК  функция начала перемещения мышки над объектом
inline _id n_mouse_move;		// АБСТРАКТНЫЙ ПРЕДОК  функция перемещения мышки над объектом
inline _id n_finish_mouse_move;	// АБСТРАКТНЫЙ ПРЕДОК  функция конца перемещения мышки над объектом
inline _id n_fun_up_middle;		// АБСТРАКТНЫЙ ПРЕДОК  функция отжато колесо мышки

struct _pair_tetron // пара тетронов
{
	_tetron* low_tetron = nullptr; // тетрон с меньшим адресом
	_tetron* high_tetron = nullptr; // тетрон с большим адресом

	_pair_tetron() = default;
	_pair_tetron(_tetron* t1, _tetron* t2) : low_tetron(t1), high_tetron(t2)
	{
		if (t1 > t2) { low_tetron = t2;  high_tetron = t1; }
	}
	_tetron* pairr(_tetron* t) { return (t == low_tetron) ? high_tetron : low_tetron; }
	bool operator==(const _pair_tetron& pt) { return (low_tetron == pt.low_tetron) && (high_tetron == pt.high_tetron); }
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

	uint64 get_flags(_tetron* t) { return (t == low_tetron) ? flags : inverted_flags(flags); }

	bool test_flags(_tetron* const t, uint64 flags_)
	{
		if (t != low_tetron) flags_ = inverted_flags(flags_);
		return ((flags & flags_) == flags_);
	}

	void set_flags(_tetron* t, uint64 f)
	{
		if (f == 0)	{ this->~_link(); return; }
		flags = (t == low_tetron) ? f : inverted_flags(f);
	}

	template <typename _t> _t* get(_tetron* t, uint64 f);

private:
	uint64 flags = 0; // флаги
};

struct _he_intermediate
{
	_tetron* tetron_before = nullptr;
	_tetron* tetron_after = nullptr;
	_tetron* tetron_intermediate = nullptr; // искомый тетрон
	uint64 flags_before = 0;
	uint64 flags_after = 0;
	uint64 number = 0; // порядковый номер

	_he_intermediate() = default;
	_he_intermediate(_tetron* t1, _tetron* t2, uint64 f1, uint64 f2) : tetron_before(t1), tetron_after(t2),
		flags_before(f1), flags_after(f2) {};
	_he_intermediate(_tetron* t1, _tetron* t2, _tetron* ti, uint64 f1, uint64 f2, uint64 n) : tetron_before(t1),
		tetron_after(t2), tetron_intermediate(ti), flags_before(f1), flags_after(f2), number(n) {};
	bool operator==(const _he_intermediate& b) {
		return (tetron_before == b.tetron_before) && (tetron_after == b.tetron_after) &&
			(flags_before == b.flags_before) && (flags_after == b.flags_after);
	}
};

struct _frozen_link // для цикла по слепку связей
{
	_frozen_link(_tetron* t, uint64 flags_);
	void operator++(int);
	operator bool() { return tetron2; }
	operator _tetron* () { return tetron2; }
	_tetron* operator->() { return tetron2; }

private:
	_tetron* tetron;
	_speed<_vector_id> lt;
	uint i;
	_tetron* tetron2;
	uint64 flags;
};

inline __hash_table <_he_intermediate> hash_intermediate;
inline uint64 number_intermediate = 0;

extern __hash_table<_link> link; // ??? если сделать inline - будут ошибки при закрытии

void delete_hvost(_tetron* t, bool del_t = false, bool run_func = true);
void optimize_hash_intermediate();
void add_obl_izm(_area_old a); // добавить область изменения
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
	return (uint)((((uint64)a.tetron) >> 4) * 27644437);
}

template <typename _t> _t* _link::get(_tetron* t, uint64 f)
{
	if (!test_flags(t, f)) return nullptr;
	_t* a = *pairr(t);
	return a;
}

template <typename _t> _t* _tetron::find_intermediate(_tetron* t, uint64 flags_before, uint64 flags_after)
{
	auto ii = hash_intermediate.find(_he_intermediate(this, t, flags_before, flags_after));
	if (ii)
	{ // !!!!!!!!!!!!!!!!!! вызываются функции у мЄртвых тетронов !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
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
			_tetron* a = i->pairr(this);
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
			_tetron* a = i->pairr(t);
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

template <typename _t> _t* _tetron::find1(uint64 flags)
{
	for (auto i : link)
	{
		_tetron* a = i->pairr(this);
		if (!i->test_flags(this, flags)) continue;
		if (_t* x = *a) return x;
	}
	return nullptr;
}
