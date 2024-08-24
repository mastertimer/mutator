#include "ui.h"
#include "e_terminal.h"
#include "diary.h"
#include <functional>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct _diary_mode : public _ui
{
private:
	_diary diary;

	virtual bool start2() override;
	virtual void destroy() override;
	virtual bool save() override;

	void init_ui_elements();
	void add_weight(_e_terminal& trm, const std::vector<std::wstring>& parameters);
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace
{

std::wstring diary_file = L"..\\..\\diary\\diary.txt";

bool start_unit()
{
	main_modes[L"diary"] = std::make_unique<_diary_mode>();
	return true;
}
auto ignore = start_unit();

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void _diary_mode::add_weight(_e_terminal& trm, const std::vector<std::wstring>& parameters)
{
	if (parameters.size() != 1)
	{
		trm.print(L"неправильно. должен быть один параметр веса");
		return;
	}
	auto w = std::stod(parameters[0]);
	if (w > 200 || w < 50)
	{
		trm.print(L"нереальный вес");
		return;
	}
	diary.add_weight(w);
	trm.print(L"вес принят: " + double_to_wstring(w, 1));
}

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
	term->command.insert({ L"вес", { L"ввод текущего веса", std::bind(&_diary_mode::add_weight, this, std::placeholders::_1, std::placeholders::_2) } });
}

void _diary_mode::destroy()
{
	diary.save_to_file(exe_path / diary_file);
}

bool _diary_mode::save()
{
	return diary.save_to_file(exe_path / diary_file);
}
