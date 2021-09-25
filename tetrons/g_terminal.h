#pragma once

#include "tetron.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct _g_terminal : public _t_go
{
	struct _command
	{
		virtual std::wstring help() = 0;
		virtual void run(_g_terminal* t, std::vector<std::wstring>& parameters) = 0;
		virtual ~_command() {}
	};

	std::map<std::wstring, std::unique_ptr<_command>> command;
	std::wstring cmd; // ��������� ������
	std::vector<std::wstring> previous_cmd; // ���� ���� ���������� ������

	i64 act_previous_cmd = 0; // �������� ���������� �������
	i64 cursor = 0; // ������� ������� � ��������� ������
	bool visible_cursor = true;
	bool insert_mode = true;
	_iarea area_cursor;
	i64 scrollbar = 0; // ������ �������� �����
	inline static int font_size = 16; // ������� 12 ��� �������������
	int font_width = 0; // ������ ��������
	inline static std::wstring prefix = L"> ";
	inline static i64 width_scrollbar = 15; // ������ ������ ���������
	inline static i64 otst_x = 3; // ������ ��� ���������
	inline static i64 otst_y = 2; // ������ ��� ���������
	_ixy selection_begin = { -1LL,0LL }; // ����� ������������ ������ � ����� �������
	_ixy selection_end = { 0LL,0LL }; // ����� ������������ ������ � ����� �������

	_g_terminal();
	uchar type() override { return 18; }
	int get_froglif() override { return 0xFC; }
	void ris2(_trans tr, bool final) override;
	void run(_tetron* tt0, _tetron* tt, u64 flags) override;
	void key_down(ushort key) override;
	void key_press(ushort key) override;
	bool mouse_wheel2(_xy r) override;
	bool mouse_down_left2(_xy r) override;
	void mouse_move_left2(_xy r) override;
	void run_cmd(); // ��������� ��������� �������
	void add_text(std::wstring_view s); // �������� �����
	void text_clear() { text.clear(); }
	void start_timer();
	void stop_timer(std::wstring_view s);

private:
	std::vector<std::wstring> text;
	std::vector<decltype(std::chrono::high_resolution_clock::now())> timer;
	i64 old_cmd_vis_len = -1; // ���������� �������� � ������
	i64 old_full_lines = 0; // ������ ���������� �����
	i64 vis_cur = false; // ������� ������ �������
	_iinterval y_slider; // ���������� ���������� y ��������
	i64 max_lines = 0; // ������� ����� ���������� �� ������
	double y0_move_slider = -1; // ��������� y - ����������� ��������
	i64 scrollbar0_move_slider = 0; // ��������� ��������� scrollbar
	i64 full_lines = 0; // ������ ���������� �����
	i64 cmd_vis_len = 0; // ���������� �������� �� x

	void set_clipboard(); // ����������� ���������� ����� � ������ ������
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
