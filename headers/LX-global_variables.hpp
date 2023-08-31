#pragma once
#include "LX-constants.hpp"

#include <X11/Xlib.h>
#include <linux/uinput.h>
#include <atomic>
#include <algorithm>
#include <string>

struct Xnames
{
    std::string name;
    std::string application;
    std::string something; // not realy sure what it is, but on firefox says "Navigator"
};

struct GlobalVariables
{
    FILE* root_pipe;
    FILE* user_pipe;
    Display* display;
    
    Xnames Win[2];
    std::atomic<bool> active;

    int virtual_input_device;

    std::atomic<bool> real_keyboard_state[NUMBER_OF_KEYS];
    std::atomic<bool> virtual_keyboard_state[NUMBER_OF_KEYS];

    void (*macros[NUMBER_OF_KEYS])(input_event, GlobalVariables*); // array containing a macro function for every key (default macro is to simulate normal behavior)
    
    GlobalVariables();
    std::atomic<int> unfinished_threadcount;
};