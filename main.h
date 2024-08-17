#pragma once

#include <map>

#include "graphics.h"

struct _mode
{
    virtual ~_mode();

    bool start();
    bool stop();
    virtual bool save() { return false; }
    virtual void key_down(u64 key) {}
    virtual void key_up(u64 key) {}
    virtual void key_press(u64 key) {}
    virtual std::wstring get_window_text() { return {}; }
    virtual _iarea draw(_isize r) { return {}; }
    virtual void resize(_isize r) {}
    virtual void mouse_button_left(bool pressed, bool dbl = false) {}
    virtual void mouse_button_right(bool pressed, bool dbl = false) {}
    virtual void mouse_button_middle(bool pressed, bool dbl = false) {}
    virtual void mouse_move() {}
    virtual void mouse_leave() {}
    virtual void mouse_wheel(short delta) {}
    virtual void timer1000() {}
    virtual void timer250() {}
    virtual void destroy() {}

    virtual _bitmap& get_bitmap() = 0;

private:
    bool started = false;

    virtual bool start2() { return false; }
    virtual bool stop2() { return false; }

};

inline std::map<std::wstring, std::unique_ptr<_mode>> main_modes;
