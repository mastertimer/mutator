#pragma once

#include "ui.h"

#include <functional>
#include <map>
#include <mutex>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct _e_terminal : public _ui_element
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
	i64 scrollbar = 0; // отступ ползунка снизу
	inline static int font_size = 14; // минимум 12 для читабельности
	int font_width = 0; // ширина символов
	inline static std::wstring prefix = L"> ";
	inline static i64 width_scrollbar = 15; // ширина полосы прокрутки
	inline static i64 otst_x = 3; // отступ при рисовании
	inline static i64 otst_y = 2; // отступ при рисовании
	_ixy selection_begin = { -1LL,0LL }; // номер отображаемой строки и номер символа
	_ixy selection_end = { 0LL,0LL }; // номер отображаемой строки и номер символа

	_e_terminal(_ui* ui_);
	void run_cmd(); // выволнить введенную команду
	void print(std::wstring_view s); // добавить текст
	void text_clear() { text.clear(); }
	void start_timer();
	void stop_timer(std::wstring_view s);
	void update() override;
	void key_down(ushort key) override;
	void key_press(ushort key) override;
	bool mouse_wheel2(_xy r, short value) override;
	bool mouse_down_left2(_xy r) override;
	void mouse_move_left2(_xy r) override;

protected:
	void ris2(_trans tr) override;

private:
	std::vector<std::wstring> text;
	std::vector<decltype(std::chrono::high_resolution_clock::now())> timer;
	i64 old_cmd_vis_len = -1; // количество символов в строке
	i64 old_full_lines = 0; // полное количество строк
	i64 vis_cur = false; // сделать курсор видимым
	_iinterval y_slider; // пиксельные координаты y ползунка
	i64 max_lines = 0; // сколько строк помещается на экране
	double y0_move_slider = -1; // начальный y - перемещения ползунка
	i64 scrollbar0_move_slider = 0; // начальное положение scrollbar
	i64 full_lines = 0; // полное количество строк
	i64 cmd_vis_len = 0; // количество символов по x
	std::mutex mtx;
	bool need_to_update = false;

	void set_clipboard(); // скопировать выделенный текст в буффер обмена
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

