#pragma once

#include <map>

#include "geometry.h"

struct _mode
{
    virtual ~_mode();

    bool start();
    bool stop();
    virtual bool save();
    virtual void key_down(u64 key);
    virtual std::wstring get_window_text();
    virtual _iarea draw(_isize r);
    virtual void resize(_isize r);

private:
    bool started = false;

    virtual bool start2();
    virtual bool stop2();
};

inline std::map<std::string, std::unique_ptr<_mode>> main_modes;
