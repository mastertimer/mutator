#include "piano.h"
#include "tetron.h"

namespace
{
	_bitmap* pic = nullptr;

	void create_blank_slate()
	{
		auto a = new _g_picture;
		a->new_size(master_bm.size);
		pic = &a->pic;
		pic->clear(0xFFFFFFFF);
		_t_trans* ttr = new _t_trans;
		auto ko = n_ko->operator _t_trans * ();
		ko->trans = _trans();
		ttr->add_flags(a, flag_sub_go + flag_part);
		n_ko->add_flags(ttr, flag_sub_go + flag_part);
	}

	void draw_notes()
	{
		pic->set_font(L"Consolas", false);
		pic->text({ 100LL, 100LL }, L"♩♪𝄞𝄢𝄢𝅗𝅥𝅘𝅥𝅱𝅘𝅥𝅯𝄞𝄢", 33, 0xFF000000);
	}
}

void start_piano()
{
	static bool started = false;
	if (started) return;
	started = true;
	create_blank_slate();
	draw_notes();
}

void stop_piano()
{

}
