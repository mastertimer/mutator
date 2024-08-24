#include "ui.h"
#include "e_terminal.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct _diary_mode : public _ui
{
private:

	virtual bool start2() override;
	void init_ui_elements();
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace
{

bool start_unit()
{
	main_modes[L"diary"] = std::make_unique<_diary_mode>();
	return true;
}
auto ignore = start_unit();

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool _diary_mode::start2()
{
	static bool first_run = true;
	if (first_run) first_run = false; else return true;

	init_ui_elements();
	return true;
}

void _diary_mode::init_ui_elements()
{
	auto term = std::make_shared<_e_terminal>(this);
	term->local_area.x = _interval(10, 480);
	term->local_area.y = _interval(10, 740);
	n_ko->add_child(term);
	n_act_key = term;
}