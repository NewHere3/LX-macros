#pragma once
#include "LX-global_variables.hpp"

#include <linux/uinput.h>

void pass_event_on(input_event event, GlobalVariables* Gvar);

int create_virtual_input_device();

void create_device_description(int& virtual_input_device);

void enable_events(int& virtual_input_device);
