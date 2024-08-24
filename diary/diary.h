#pragma once

#include <vector>
#include <filesystem>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

enum class _entry_type { weight };

struct _diary_entry
{
	time_t time;
	_entry_type type;
	double double_;
};

struct _diary
{
	void load_from_file(const std::filesystem::path& fn);
	bool save_to_file(const std::filesystem::path& fn);

	void add_weight( double weight );

private:
	std::vector<_diary_entry> entries;
};
