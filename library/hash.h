#pragma once

#include "basic.h"

inline uint hash_func(void* const& a) { return (uint)((((u64)a) >> 4) * 27644437); }
inline uint hash_func(const u64& a) { return (uint)(a * 27644437); }

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// хэш таблица, с мгновенной очисткой, и любыми данными, но с дополнительной переменной id
t_t struct __hash_table
{
	struct _hash_info
	{
		u64 id = 0; // код занятости
		_t a = {}; // данные
	};

	struct _iterator
	{
		_hash_info* hi;
		__hash_table* ht;

		bool operator!=(const _iterator& b) { return hi != b.hi; }
		_iterator& operator++();
		_t& operator*() const { return hi->a; }
		_t* operator->() const { return &hi->a; }
		bool life() { if (hi->id == ht->id) return false; hi->id = ht->id; ht->size++; return true; } // оживление
		operator bool() { return (hi->id == ht->id); }
	};

	_hash_info* data = nullptr;
	uint capacity = 0;
	uint size = 0; // количество элементов

	~__hash_table() noexcept { delete[] data; }
	void clear() noexcept { id++; size = 0; }
	bool insert(const _t& b) noexcept; // return - элемента небыло? (!!присваивание в любом случае!!)
	bool insert(_t&& b) noexcept; // return - элемента небыло? (!!присваивание в любом случае!!)
	template <typename _b> _iterator find(const _b& x); // хэш функции должны одинаковые значания выдавать
	template <typename _b> void erase(const _b& x) { erase(find(x)); } // удалить элемент
	void erase(_t* x); // удалить элемент
	void erase(const _iterator& x); // удалить элемент

	_iterator begin() noexcept;
	_iterator end() noexcept { return { (data + capacity), this }; }

private:
	u64 id = 1; // рабочий код занятости

	void reserve() noexcept;
};

t_t void __hash_table<_t>::erase(const _iterator& x)
{
	if (x.hi->id != id) return; // элемента нет
	uint n = (uint)(x.hi - data);
	uint max_i = capacity - 1;
	for (uint j = (n + 1) & max_i; data[j].id == id; j = (j + 1) & max_i)
	{
		uint j0 = hash_func(data[j].a) & max_i;
		if ((j < j0) ^ (n >= j0) ^ (n < j)) continue;
		data[n] = std::move(data[j]);
		n = j;
	}
	data[n].id = 0;
	size--;
}

t_t void __hash_table<_t>::erase(_t* x)
{
	uint n = (uint)(((u64)x - (u64)data) / sizeof(_hash_info));
	if (n < capacity)
		erase(_iterator{ &data[n], this });
	else
		erase(find(*x));
}

t_t template <typename _b> typename __hash_table<_t>::_iterator __hash_table<_t>::find(const _b& x)
{
	if (size * 2 >= capacity) reserve(); // 3x - 1.25 итерации, 2x - 1.5 итерации, 1.5x - 2.0 итерации
	uint i;
	uint max_i = capacity - 1;
	for (i = hash_func(x) & max_i; data[i].id == id; i = (i + 1) & max_i) if (data[i].a == x) break;
	return { &data[i], this };
}

t_t void __hash_table<_t>::reserve() noexcept
{ // 3x - 1.25 итерации, 2x - 1.5 итерации, 1.5x - 2.0 итерации
	uint capacity_old = capacity;
	capacity = (capacity) ? capacity * 2 : 2;
	_hash_info* data2 = new _hash_info[capacity]();
	for (uint j = 0; j < capacity_old; j++)
	{
		if (data[j].id != id) continue;
		uint i;
		uint max_i = capacity - 1;
		for (i = hash_func(data[j].a) & max_i; data2[i].id == id; i = (i + 1) & max_i);
		data2[i] = std::move(data[j]);
	}
	delete[] data;
	data = data2;
}

t_t bool __hash_table<_t>::insert(const _t& b) noexcept
{
	_iterator n = find(b);
	n.hi->a = b;
	return n.life();
}

t_t bool __hash_table<_t>::insert(_t&& b) noexcept
{
	_iterator n = find(b);
	n.hi->a = std::move(b);
	return n.life();
}

t_t typename __hash_table<_t>::_iterator __hash_table<_t>::begin() noexcept
{
	for (uint i = 0; i < capacity; i++)	if (data[i].id == id) return { (data + i), this };
	return { (data + capacity), this };
}

t_t typename __hash_table<_t>::_iterator& __hash_table<_t>::_iterator::operator++()
{
	_hash_info* en = (ht->data + ht->capacity);
	for (++hi; hi != en; ++hi) if (hi->id == ht->id) break;
	return *this;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// хэш таблица парных элементов с медленной очисткой, и _t() означает отсутствие элемента, и нельзя добавлять!
template <typename _t, typename _tt>
struct _hash_map
{
	struct _hash_info
	{
		_t a = {}; // данные
		_tt b = {}; // данные
	};

	struct _iterator
	{
		_hash_info* hi;
		_hash_map* ht;

		bool operator!=(const _iterator& b) { return hi != b.hi; }
		_iterator& operator++();
		_tt& operator*() const { return hi->b; }
		_tt* operator->() const { return &hi->b; }
		void  life() { if (!hi->a) ht->size++; } // оживление !! в начале
		operator bool() { return hi->a; }
	};

	_hash_info* data = nullptr;
	uint capacity = 0;
	uint size = 0; // количество элементов

	~_hash_map() noexcept { delete[] data; }
	void insert(_t a, _tt b) noexcept; // нужно ли const& ??
	_iterator find(_t x) noexcept; // поиск места
	void erase(_t b); // удалить элемент

	_iterator begin() noexcept;
	_iterator end() noexcept { return { (data + capacity), this }; }

private:
	void reserve() noexcept;
};

template <typename _t, typename _tt>
void _hash_map<_t, _tt>::insert(_t a, _tt b) noexcept
{
	_iterator n = find(a);
	n.life();
	*n.hi = { a,b };
}

template <typename _t, typename _tt>
typename _hash_map<_t, _tt>::_iterator& _hash_map<_t, _tt>::_iterator::operator++()
{
	_hash_info* en = (ht->data + ht->capacity);
	for (++hi; hi != en; ++hi) if (hi->a) break;
	return *this;
}

template <typename _t, typename _tt>
typename _hash_map<_t, _tt>::_iterator _hash_map<_t, _tt>::find(_t x) noexcept
{
	if (size * 2 >= capacity) reserve(); // 3x - 1.25 итерации, 2x - 1.5 итерации, 1.5x - 2.0 итерации
	uint i;
	uint max_i = capacity - 1;
	for (i = hash_func(x) & max_i; data[i].a; i = (i + 1) & max_i) if (data[i].a == x) break;
	return { &data[i], this };
}

template <typename _t, typename _tt>
void _hash_map<_t, _tt>::reserve() noexcept
{ // 3x - 1.25 итерации, 2x - 1.5 итерации, 1.5x - 2.0 итерации
	uint capacity_old = capacity;
	capacity = (capacity) ? capacity * 2 : 2;
	_hash_info* data2 = new _hash_info[capacity]();
	for (uint j = 0; j < capacity_old; j++)
		if (data[j].a)
		{
			uint i;
			uint max_i = capacity - 1;
			for (i = hash_func(data[j].a) & max_i; data2[i].a; i = (i + 1) & max_i);
			data2[i] = std::move(data[j]);
		}
	delete[] data;
	data = data2;
}

template <typename _t, typename _tt>
typename _hash_map<_t, _tt>::_iterator _hash_map<_t, _tt>::begin() noexcept
{
	for (uint i = 0; i < capacity; i++)	if (data[i].a) return { (data + i), this };
	return { (data + capacity), this };
}

template <typename _t, typename _tt>
void _hash_map<_t, _tt>::erase(_t b)
{
	auto x = find(b);
	if (!x) return;
	uint n = (uint)(x.hi - data);
	uint max_i = capacity - 1;
	for (uint j = (n + 1) & max_i; data[j].a; j = (j + 1) & max_i)
	{
		uint j0 = hash_func(data[j].a) & max_i;
		if ((j < j0) ^ (n >= j0) ^ (n < j)) continue;
		data[n] = std::move(data[j]);
		n = j;
	}
	data[n] = {}; // и a и b должны обнулиться
	size--;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

