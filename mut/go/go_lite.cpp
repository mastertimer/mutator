#include "go_lite.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void _g_circle::push(_wjson& b)
{
	_t_go::push(b);
	b.add("center", center);
	b.add("radius", radius);
	b.add("width", width);
}

void _g_circle::pop(_rjson& b)
{
	_t_go::pop(b);
	b.read("center", center);
	b.read("radius", radius);
	b.read("width", width);
}

void _g_circle::run(_tetron* tt0, _tetron* tt, uint64 flags)
{
	_one_tetron* a = find_intermediate<_one_tetron>(n_center, flag_information, flag_parent);
	if (a) center = { a->d[0], a->d[1] };
	a = find_intermediate<_one_tetron>(n_radius, flag_information, flag_parent);
	if (a) radius = a->d[0];
	a = find_intermediate<_one_tetron>(n_width, flag_information, flag_parent);
	if (a) width = a->d[0];
	del_area();
	calc_local_area();
	area = _tarea::indefinite;
	add_area();
	_t_go::run(tt0, tt, flags);
}

void _g_circle::calc_local_area()
{
	local_area = _area_old(center.x - radius, center.x + radius, center.y - radius, center.y + radius);
}

void _g_circle::ris2(_trans tr, bool final)
{
	master_bm.fill_ring(tr(center), tr(radius), tr(width), get_c(), get_c2());
}

bool _g_circle::test_local_area(_xy b)
{
	if (!local_area.test(b)) return false;
	return ((b - center).len2() <= radius * radius);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void _g_froglif::ris2(_trans tr, bool final)
{
	_area_old a = tr(local_area);
	master_bm.froglif(a.top_left(), a.min_length(), f, r_f, get_c(), get_c2());
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void _g_line::run(_tetron* tt0, _tetron* tt, uint64 flags)
{
	_one_tetron* a = find_intermediate<_one_tetron>(n_begin, flag_information, flag_parent);
	if (a) p1 = { a->d[0], a->d[1] };
	a = find_intermediate<_one_tetron>(n_end, flag_information, flag_parent);
	if (a) p2 = { a->d[0], a->d[1] };
	a = find_intermediate<_one_tetron>(n_width, flag_information, flag_parent);
	if (a) width = a->d[0];
	del_area();
	calc_local_area();
	area = _tarea::indefinite;
	add_area();
	_t_go::run(tt0, tt, flags);
}

void _g_line::calc_local_area()
{
	local_area = (_area_old(p1) + p2).expansion(width * 0.5);
}

void _g_line::ris2(_trans tr, bool final)
{
	master_bm.lines(tr(p1), tr(p2), tr(width), get_c());
}

bool _g_line::test_local_area(_xy b)
{
	if (!local_area.test(b)) return false;
	_xy n = { p1.y - p2.y,p2.x - p1.x };
	n *= 1.0 / n.len();
	_xy p11 = p1 + n * (width * 0.75);
	_xy p12 = p1 - n * (width * 0.75);
	_xy p21 = p2 + n * (width * 0.75);
	_xy p22 = p2 - n * (width * 0.75);
	if (((test_line(p11, p12, b) + test_line(p21, p22, b) + test_line(p11, p21, b) + test_line(p12, p22, b)) & 1) == 0) return false;
	return true;
}

void _g_line::push(_wjson& b)
{
	_t_go::push(b);
	b.add("p1", p1);
	b.add("p2", p2);
	b.add("width", width);
}

void _g_line::pop(_rjson& b)
{
	_t_go::pop(b);
	b.read("p1", p1);
	b.read("p2", p2);
	b.read("width", width);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void _g_picture::ris2(_trans tr, bool final)
{
	if (pic.size.x * pic.size.y == 0)
	{
		int rr = 100;
		master_bm.line({ (int)tr.offset.x, (int)tr.offset.y }, { (int)(tr.offset.x + rr * tr.scale), (int)tr.offset.y },
			c_def);
		master_bm.line({ (int)(tr.offset.x + rr * tr.scale), (int)tr.offset.y },
			{ (int)(tr.offset.x + rr * tr.scale), (int)(tr.offset.y + rr * tr.scale) }, c_def);
		master_bm.line({ (int)(tr.offset.x + rr * tr.scale), (int)(tr.offset.y + rr * tr.scale) },
			{ (int)tr.offset.x, (int)(tr.offset.y + rr * tr.scale) }, c_def);
		master_bm.line({ (int)tr.offset.x, (int)(tr.offset.y + rr * tr.scale) }, { (int)tr.offset.x, (int)tr.offset.y },
			c_def);
		master_bm.line({ (int)tr.offset.x, (int)tr.offset.y },
			{ (int)(tr.offset.x + rr * tr.scale), (int)(tr.offset.y + rr * tr.scale) }, c_def);
		master_bm.line({ (int)(tr.offset.x + rr * tr.scale), (int)tr.offset.y },
			{ (int)tr.offset.x, (int)(tr.offset.y + rr * tr.scale) }, c_def);
		return;
	}
	_area_old oo = tr(local_area);
	master_bm.stretch_draw3(&pic, (int)oo.x.min, (int)oo.y.min, tr.scale);
}

void _g_picture::new_size(int rx3, int ry3)
{
	if (!pic.resize(rx3, ry3)) return;
	del_area();
	local_area = _area_old(0, (double)pic.size.x, 0, (double)pic.size.y);
	area.type = _tarea::indefinite;
	add_area();
}

bool _g_picture::load_from_file(_path fn)
{
/*	del_area();
	_stack fs;
	fs.load_from_file(fn);
	_bitmap_file bf;
	fs >> bf;
	if (!bf) return false;
	pic = std::move(bf);
	local_area = _area_old(0, (double)pic.size.x, 0, (double)pic.size.y);
	area.type = _tarea::indefinite;
	add_area();
	return true;*/
	return true;
}

bool _g_picture::save_to_file(_path fn) const
{
/*	_stack fs((size_t)pic.size.x * pic.size.y * sizeof(_color));
	fs << _bitmap_file(pic);
	return fs.save_to_file(fn);*/
	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void _g_rect::ris2(_trans tr, bool final)
{
	_area_old oo = tr(local_area);
	uint c2 = get_c2();
	master_bm.fill_rectangle({ {(int64)oo.x.min, (int64)oo.x.max+1}, {(int64)oo.y.min, (int64)oo.y.max+1} }, c2);
	uint c0 = get_c();
	if (((c0 >> 24) != 0x00) && (c0 != c2)) master_bm.rectangle((_area2)oo, c0);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void _g_text::set_text(std::wstring_view s2)
{
	del_area();
	s = s2;
	vec2i size = master_bm.size_text(s2, 13);
	local_area = _area_old(-1, size.x, 0, size.y);
	area.type = _tarea::indefinite;
	add_area();
}

void _g_text::ris2(_trans tr, bool final)
{
	int sf = (int)(13 * tr.scale + 0.5);
	if (sf < 5) return;
	master_bm.text((int)tr.offset.x, (int)tr.offset.y, s.c_str(), sf, get_c(), get_c2());
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void del_hint()
{
	if (!n_hint) return;
	delete_hvost(n_hint->operator _t_basic_go * ()->ttrans(), true);
}

void add_hint(std::wstring_view hint, _t_go* g)
{
	del_hint();
	if (hint.empty()) return;
	_t_trans* ko = *n_ko;
	_trans tr = master_trans_go;
	vec2i siz = master_bm.size_text(hint, 13);
	tr.offset += _xy{ -siz.x * 0.5, -15.0 } +_xy{ g->local_area.x(0.5), g->local_area.y.min } *tr.scale;
	tr.scale = 1;
	_g_text* go = new _g_text;
	go->set_c(c_maxx);
	go->set_c2(c_background);
	_t_trans* ttr = new _t_trans;
	ttr->trans = ko->trans.inverse() * tr;
	ttr->add_flags(go, flag_part + flag_sub_go);
	go->set_text(hint);
	ko->add_flags(ttr, flag_part + flag_sub_go);
	n_hint = go;
}

void change_hint(std::wstring_view hint)
{
	if (!n_hint) return;
	_g_text* go = *n_hint;
	go->set_text(hint);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void _g_test_graph::ris2(_trans tr, bool final)
{
	static bool first = true;
	if (first)
	{
		_picture te(120, 90);
		te.clear();
		for (int i = 0; i < 100; i++)
			te.line({prng.random(120), prng.random(90)}, { prng.random(120), prng.random(90) }, 0xFF2080FF);
		first = false;
		a.clear(0xFF0000FF);
		_area2i ogr({ 30, 225 }, {25, 125});
		a.set_area(ogr);
		a.stretch_draw3(&te, 0, 0, 3.3);
		a.rectangle(ogr, 0x80FF0000);
	}
	_area_old oo = tr(local_area);
	master_bm.stretch_draw(&a, (int)(oo.x.min+20), (int)(oo.y.min+20), 1);
}

_g_test_graph::_g_test_graph(): a(250, 150)
{
	local_area = _area_old(0, 300, 0, 200);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
