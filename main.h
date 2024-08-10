#pragma once

#include <string>
#include <memory>
#include <map>

struct _mode
{
    bool start();
    bool stop();
    virtual bool save();
    virtual void key_down();
    virtual ~_mode();
private:
    bool started = false;

    virtual bool start2();
    virtual bool stop2();
};

inline std::map<std::string, std::unique_ptr<_mode>> main_modes;
