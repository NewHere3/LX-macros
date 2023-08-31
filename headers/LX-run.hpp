#pragma once
#include "LX-global_variables.hpp"
#include "LX-constants.hpp"

#include <string>

//void run(std::string command, GlobalVariables* Gvar, bool root = 0, bool wait = 0);

void execute(std::string command, GlobalVariables* Gvar);

void run_as_root(std::string command, GlobalVariables* Gvar, bool wait_to_finish = 1);

void run(std::string command, GlobalVariables* Gvar, bool wait_to_finish = 1);

void catch_message_return(GlobalVariables* Gvar);
