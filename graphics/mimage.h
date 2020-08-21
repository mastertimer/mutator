#pragma once

#include "mbasic.h"
#include "mgeometry.h"
#include "mgraphics.h"

// заглушка для загрузки из/в файл .bmp
// ! создаёт пиксели только при чтении, не владеет ими (не удаляет)
struct _bitmap_file
{
	_color* data = nullptr;
	vec2i   size = {0, 0};

	_bitmap_file()               = default;
	_bitmap_file(_bitmap_file&&) = default;
	_bitmap_file(_color* data, vec2i size) : data(data), size(size) {}
	_bitmap_file(_picture const& p) : data((_color*)p.data), size(p.size) {}

	_bitmap_file& operator=(_bitmap_file const&) = default;

	operator bool() const;
	operator _picture() &&; // оператор преобразования с перемещенем
};

_stack& operator<<(_stack& s, _bitmap_file const&& f);
_stack& operator>>(_stack& s, _bitmap_file& f);
