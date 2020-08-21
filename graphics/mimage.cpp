#include "mimage.h"

#include <exception>

#pragma pack(push, 1)
struct _bitmap_file_header
{
	uint16 file_type = 0x4D42; // Указывает на то, что тип файла - bitmap
	uint32 file_size = 0;      // Размер файла (в байтах)

	uint16 _1 = 0;
	uint16 _2 = 0;

	uint32 data_offset = 0; // Начало пикселей (размер заголовка)
};
#pragma pack(pop)

#pragma pack(push, 1)
struct _bitmap_info_header
{
	uint32 size  = 0; // Размер заголовка (в байтах)
	int32  width = 0; // Ширина в пикселях
	int32 height = 0; // Высота в пикселях (+, перевёрнуто по y, -, не перевёрнуто)

	uint16 planes      = 1;  // 1
	uint16 bit_count   = 32; // Битов на пиксель
	uint32 compression = 0;  // 0
	uint32 size_image  = 0;  // 0

	int32  x_pixels_per_meter = 0;
	int32  y_pixels_per_meter = 0;
	uint32 colors_used        = 0; // Число цветов в таблице, 0 если используется возможный максимум
	uint32 colors_important   = 0; // Число отображаемых цветов, 0 если используется возможный максимум
};
#pragma pack(pop)

constexpr uint32 bitmap_data_offset = (uint32)sizeof(_bitmap_file_header) + (uint32)sizeof(_bitmap_info_header);
constexpr uint32 bitmap_info_size   = (uint32)sizeof(_bitmap_info_header);

_bitmap_file::operator bool() const { return data; }
_bitmap_file::operator _picture() &&
{
	_picture p;
	p.size.x = size.x;
	p.size.y = size.y;
	p.data   = (uint*)data;
	data     = nullptr;
	size     = {0, 0};
	return p;
}

_stack& operator<<(_stack& s, _bitmap_file const&& f)
{
	_bitmap_file_header bfh;
	bfh.data_offset = bitmap_data_offset;
	bfh.file_size   = bfh.data_offset + sizeof(_color) * f.size.x * f.size.y;
	s << bfh;

	_bitmap_info_header bih;
	bih.size   = bitmap_info_size;
	bih.width  = f.size.x;
	bih.height = -f.size.y;
	s << bih;

	s.push_data((void*)f.data, sizeof(_color) * f.size.x * f.size.y);
	return s;
}

_stack& operator>>(_stack& s, _bitmap_file& f)
{
	f.data = nullptr;
	f.size = {0, 0};

	_bitmap_file_header bfh;
	s >> bfh;

	if (bfh.file_type != 0x4D42) return s;

	_bitmap_info_header bih;
	s >> bih;

	vec2i size = {bih.width, std::abs(bih.height)};

	if (size.x < 1 || size.y == 0 || (bih.bit_count != 32 && bih.bit_count != 24) || bih.compression != 0 ||
	    bih.colors_used != 0)
		return s;

	f.size = size;

	const int32 offs = (int32)bitmap_info_size - (int32)bih.size;

	if (offs > 0) // вернуть данные
		s.revert(offs);
	else if (offs < 0) // пропустить лишнее
		s.skip(-offs);

	if (bih.bit_count == 32)
	{
		if (bih.height < 0)
		{
			f.data = new _color[(size_t)size.x * size.y];
			s.pop_data((void*)f.data, sizeof(_color) * size.x * size.y);
		}
		else // изображение перевёрнуто по y
		{
			f.data = new _color[(size_t)f.size.x * f.size.y];
			for (auto it = f.data + (size_t)size.x * ((size_t)size.y - 1); it > f.data; it -= size.x)
				s.pop_data((void*)it, sizeof(_color) * f.size.x);
		}
	}
	else if (bih.bit_count == 24)
	{
		if (bih.height < 0)
		{
			f.data     = new _color[(size_t)size.x * size.y];
			auto fdend = f.data + (size_t)size.x * size.y;
			for (auto it = f.data; it != fdend; ++it) s.pop_data((void*)it, 3);
		}
		else // изображение перевёрнуто по y
		{
			f.data = new _color[(size_t)f.size.x * f.size.y];
			for (auto it = f.data + (size_t)size.x * ((size_t)size.y - 1); it > f.data; it -= size.x)
			{
				auto fdend = it + size.x;
				for (auto it2 = it; it2 != fdend; ++it2) s.pop_data((void*)it2, 3);
			}
		}
	}

	return s;
}
