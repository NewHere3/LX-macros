#pragma once
#include "LX-run.hpp"

#include <time.h>
#include <unistd.h>
#include <string.h>

inline void msleep(int mseconds)
{
    struct timespec request = { mseconds/1000, (mseconds%1000)*1000000};
    nanosleep(&request, NULL);
}

inline void key_press(int key, GlobalVariables* Gvar)
{
    input_event event ={0};
    //Down
    event.type = 1;
    event.code = key;
    event.value = 1;
    write(Gvar->virtual_input_device, &event, sizeof(input_event));
    memset(&event, 0, sizeof(input_event));
    write(Gvar->virtual_input_device, &event, sizeof(input_event));
    //Up
    event.type = 1;
    event.code = key;
    event.value = 0;
    write(Gvar->virtual_input_device, &event, sizeof(input_event));
    memset(&event, 0, sizeof(input_event));
    write(Gvar->virtual_input_device, &event, sizeof(input_event));
}

inline void key_press(int key, int state, GlobalVariables* Gvar)
{
    input_event event ={0};
    event.type = 1;
    event.code = key;
    event.value = state;
    write(Gvar->virtual_input_device, &event, sizeof(input_event));
    memset(&event, 0, sizeof(input_event));
    write(Gvar->virtual_input_device, &event, sizeof(input_event));
    Gvar->virtual_keyboard_state[key] = state;
}

inline void get_mouse_coordinates(int& x, int& y, GlobalVariables* Gvar, Window window = RootWindowOfScreen(ScreenOfDisplay(XOpenDisplay(":0"), 0)))
{
    Window d_child, d_root;
    int d_x, d_y;
    unsigned int d_mask;
    XQueryPointer(Gvar->display, window, &d_child, &d_root, &d_x, &d_y, &x, &y, &d_mask);
}

//inline void key_hold(int key, int m_sec);

// inline void mouse_move(int x, int y);

// inline void mouse_move(int x, int y, int speed);

inline void notify(std::string text, GlobalVariables* Gvar, int timeout = 1000)
{
    //std::cout << "notify-send -t " + std::to_string(timeout) + " -h string:desktop-entry:" + "notification" + " \"" + text + "\"" << std::endl;
    run("notify-send -t " + std::to_string(timeout) + " -h string:desktop-entry:" + "LX-macros" + " \"" + text + "\"", Gvar, 0);
    return;
}

inline void beep(GlobalVariables* Gvar, int frequency = 320, float duration= 0.3, int volume = 100)
{
    run("ffplay -volume " + std::to_string(volume) + " -f lavfi -i \"sine=frequency=" + std::to_string(frequency) + ":duration=" + std::to_string(duration) + "\" -autoexit -nodisp -loglevel panic", Gvar, 0);
    return;
}