#pragma once

#include "tetron.h"

namespace mutator
{
	double get_main_scale(); // главный масштаб
	void save_to_txt_file(std::filesystem::path fn);
	bool start(std::filesystem::path fn);
	void draw(_bitmap &e);
	bool need_draw();

	void mouse_button_left(bool pressed);
	void mouse_button_right(bool pressed);
	void mouse_button_middle(bool pressed);
}