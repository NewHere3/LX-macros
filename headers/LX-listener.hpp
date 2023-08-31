#pragma once
#include "LX-global_variables.hpp"

int open_real_device(const char* device_path);

void read_keyboard(GlobalVariables* G_var, const char* device_path);

void read_mouse(GlobalVariables* G_var, const char* device_path);