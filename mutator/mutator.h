#pragma once

#include "basic.h"

struct _mutator
{
	static double get_main_scale();
	static bool save_to_txt_file();
	static bool start();
	static void resize(_isize r);
	static void draw(_isize r);

	static void mouse_button_left(bool pressed);
	static void mouse_button_right(bool pressed);
	static void mouse_button_middle(bool pressed);

private:
	static _tetron* create_tetron(const std::string& name);
	static bool load_from_txt_file(const std::filesystem::path& fn);
	static void init_layers();
};