#include "piano.h"
#include "tetron.h"

void start_piano()
{
	static bool started = false;
	if (started) return;
	started = true;
	auto a = new _g_picture;
	a->new_size(master_bm.size);
	a->pic.clear(0xFFFFFFFF);
	_t_trans* ttr = new _t_trans;
	auto ko = n_ko->operator _t_trans * ();
	ko->trans = _trans();
	ttr->add_flags(a, flag_sub_go + flag_part);
	n_ko->add_flags(ttr, flag_sub_go + flag_part);
}

void stop_piano()
{

}
