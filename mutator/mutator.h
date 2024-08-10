#pragma once

#include "basic.h"
#include "../main.h"

struct _mutator
{
	static double get_main_scale();

	static void mouse_button_left(bool pressed);
	static void mouse_button_right(bool pressed);
	static void mouse_button_middle(bool pressed);
};

struct _mutator_mode : public _mode
{
	virtual bool save() override;
	virtual void key_down(u64 key) override;
	virtual std::wstring get_window_text() override;
	virtual _iarea draw(_isize r) override;
	virtual void resize(_isize r) override;

private:
	virtual bool start2() override;
	bool load_from_txt_file(const std::filesystem::path& fn);
};