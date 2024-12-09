#include "diary.h"
#include "basic.h"
#include <fstream>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void _time_interval::operator=(time_t time)
{
    end = begin = time;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void _diary::load_from_file(const std::filesystem::path& fn)
{
    entries.clear();
    std::ifstream file(fn);
}

bool _diary::save_to_file(const std::filesystem::path& fn)
{
    std::ofstream file(fn);
    char s[20];
    for (auto& entry : entries)
    {
        tm t3;
        localtime_s(&t3, &entry.time.end);

        strftime(s, sizeof(s), "%d.%m.%g %H.%M", &t3);
        file << s << " ";

        switch (entry.type)
        {
        case _entry_type::weight:
            file << "вес=" << double_to_string(entry.value_, 1);
            break;
        }
        file << std::endl;
    }
    return !file.fail();
}

void _diary::add_weight(double weight)
{
    _diary_entry entry;
    entry.time = time(0);
    entry.type = _entry_type::weight;
    entry.value_ = weight;
    entries.push_back(entry);
}
