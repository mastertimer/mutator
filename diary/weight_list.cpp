#include "weight_list.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

_e_weight_list::_e_weight_list(_ui* ui_) : _ui_element(ui_)
{
	local_area = { {0, 60}, {0, 100} };
}

void _e_weight_list::draw(_trans tr)
{
	_iarea oo = tr(local_area);
	ui->canvas.rectangle(oo, ui->border_color);
}

void _e_weight_list::update()
{
	cha_area();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
