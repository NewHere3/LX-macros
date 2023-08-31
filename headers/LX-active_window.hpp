#pragma once
#include "LX-global_variables.hpp"

void get_active_window(GlobalVariables* Gvar);

int catch_bad_window(Display* d_display, XErrorEvent* d_xerrorevent);

void get_focused_window(Window* window, Display* display);

void find_parent(Window* window, Display* display);

void register_window_info(Window window, Display* display, GlobalVariables* Gvar);

unsigned char* check_window_property(Window window, Display* display, Atom atom, unsigned long* nitems);