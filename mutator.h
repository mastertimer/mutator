#pragma once

#include "basic.h"

namespace mutator
{
	double get_main_scale();
	void save_to_txt_file(const std::filesystem::path& fn);
	bool start(const std::filesystem::path& fn);
	void resize(_isize r);
	void draw(_isize r);

	void mouse_button_left(bool pressed);
	void mouse_button_right(bool pressed);
	void mouse_button_middle(bool pressed);
}