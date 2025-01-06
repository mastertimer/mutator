#pragma once

#include "ui.h"
#include <map>
#include <mutex>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct _e_terminal : public _e_scrollable_area
{
	struct _command
	{
		std::wstring caption;
		std::function<void(_e_terminal&, const std::vector<std::wstring>&)> function;
	};

	std::map<std::wstring, _command> command;
	std::wstring cmd; // командная строка
	std::vector<std::wstring> previous_cmd; // база всех вызываемых команд

	i64 act_previous_cmd = 0; // активная предыдущая команда
	i64 cursor = 0; // позиция курсора в командной строке
	bool visible_cursor = true;
	bool insert_mode = true;
	_iarea area_cursor;
	i64 font_width = 0; // ширина символов
	std::wstring font_name = L"Cascadia Code";
	inline static std::wstring prefix = L"> ";
	inline static i64 otst_x = 3; // отступ при рисовании
	inline static i64 otst_y = 2; // отступ при рисовании
	_ixy selection_begin = { -1LL,0LL }; // номер отображаемой строки и номер символа
	_ixy selection_end = { 0LL,0LL }; // номер отображаемой строки и номер символа

	_e_terminal(_ui* ui_, int font_size_ = 14);
	void run_cmd(); // выволнить введенную команду
	void print(std::wstring_view s); // добавить текст
	void text_clear() { text.clear(); }
	void start_timer();
	void stop_timer(std::wstring_view s);
	void update() override;
	void key_down(u64 key) override;
	void key_press(u64 key) override;
	bool mouse_wheel2(_xy r, short value) override;
	bool mouse_down_left2(_xy r) override;
	void mouse_move_left2(_xy r) override;
	void calc_lines() override;

protected:
	void draw(_trans tr) override;

private:
	int font_size = 14; // минимум 12 для читабельности
	std::vector<std::wstring> text;
	std::vector<decltype(std::chrono::high_resolution_clock::now())> timer;
	double y0_move_slider = -1; // начальный y - перемещения ползунка
	i64 scrollbar0_move_slider = 0; // начальное положение scrollbar
	std::map<i64, i64> total_lines_map; // для разного количества символов в строке
	i64 cmd_vis_len = 0; // количество символов по x
	std::mutex mtx;
	bool need_to_update = false;

	void set_clipboard(); // скопировать выделенный текст в буффер обмена
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

