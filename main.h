#pragma once

#include <functional>
#include <string>
#include <map>

inline std::map<std::string, std::function<bool()>> main_modes;
inline std::function<bool()> main_save;
