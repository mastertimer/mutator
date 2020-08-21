#pragma once

#include "tetron.h"
#include "mwindow.h"

namespace mutator
{
	double get_main_scale(); // главный масштаб
	void save_to_txt_file(std::filesystem::path fn);
	bool start(std::filesystem::path fn);
	void draw(_window_event::_render e);

	void mouse_button_left(_number2 xy, bool pressed);
	void mouse_button_right(_number2 xy, bool pressed);
	void mouse_button_middle(_number2 xy, bool pressed);
}