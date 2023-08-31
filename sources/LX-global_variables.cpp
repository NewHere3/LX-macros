#include "../headers/LX-global_variables.hpp"
#include "../headers/LX-constants.hpp"
#include "../headers/LX-input.hpp"
#include <iostream>

GlobalVariables::GlobalVariables()
{
        std::fill(real_keyboard_state, real_keyboard_state+NUMBER_OF_KEYS,  0);
        std::fill(virtual_keyboard_state, real_keyboard_state+NUMBER_OF_KEYS,  0);
        std::fill(macros, macros+NUMBER_OF_KEYS,  pass_event_on);
        virtual_input_device = create_virtual_input_device();
        display = XOpenDisplay(DISPLAY_NR);
        active = 0;
        unfinished_threadcount = 0;
        root_pipe = fopen(ROOT_PIPE, "wb");
        std::cout << "root_pipe opened!" << std::endl;
        user_pipe = fopen(USER_PIPE, "rb");
        std::cout << "user_pipe opened!" << std::endl;
}
