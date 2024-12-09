#pragma once

#include <vector>
#include <filesystem>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct _time_interval
{
	time_t end; // ключ
	time_t begin;

	void operator=(time_t time);
};

enum class _entry_type { weight };

struct _diary_entry
{
	_time_interval time;
	_entry_type type;
	double value_;
};

struct _diary
{
	void load_from_file(const std::filesystem::path& fn);
	bool save_to_file(const std::filesystem::path& fn);

	void add_weight( double weight );

private:
	std::vector<_diary_entry> entries; // отсортировано по ключу
};
