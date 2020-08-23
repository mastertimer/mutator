#pragma once

#include "t_basic_go.h"

struct _g_list_link : public _g_rect // графический объект - список связей
{
	int cursor = 0; // абсолютный номер элемента
	int first = 0; // номер первого элемента в списке

	uchar type() { return 28; }
	int get_froglif() { return 0xF5; }
	void ris2(_trans tr, bool final) override;
	bool mouse_down_left2(_xy r);
	void run(_tetron* tt0, _tetron* tt, uint64 flags);
	operator _g_list_link* () { return this; }

private:
	_tetron* tf = nullptr; // для какого тетрона отображены флаги
	int curf = 0; // курсор у которого отображены флаги
};

