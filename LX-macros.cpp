#include "headers/LX-input.hpp"
#include "headers/LX-listener.hpp"
#include "headers/LX-utilities.hpp"
#include "headers/LX-global_variables.hpp"
#include "headers/LX-overlay.hpp"
#include "headers/LX-active_window.hpp"
#include "headers/LX-run.hpp"
#include "headers/LX-messages.hpp"
#include "headers/LX-selections.hpp"

#include <thread>
#include <iostream>

Selection selection(XOpenDisplay(0));;
Overlay x("assets/n251.bmp");
Block cont;

void KEY_X_macro(input_event event, GlobalVariables* Gvar)
{
    if(event.value == 0)
    {
        selection.info("CLIPBOARD");
    }
}

void KEY_D_macro(input_event event, GlobalVariables* Gvar)
{
    // std::cout << Gvar->Win[Gvar->active].name << std::endl;
    std::string txt = "Hello world";
    Block temp((void*) txt.c_str(), txt.size()+1);
    if(event.value == 0)
    {
        selection.store("CLIPBOARD", "UTF8_STRING", cont);
    }
}

void KEY_P_macro(input_event event, GlobalVariables* Gvar)
{
    // if(event.value == 0)
    // {
    //     beep(Gvar);
    // }
}

void close_fun(GlobalVariables* Gvar)
{
    XCloseDisplay(Gvar->display);
    fclose(Gvar->root_pipe);
    fclose(Gvar->user_pipe);
}


int main()
{
    GlobalVariables Gvar;
    msleep(100);
    char keyboard_path[] = "/dev/input/by-id/usb-Logitech_USB_Receiver-event-kbd";
    char mouse_path[] = "/dev/input/by-id/usb-Logitech_USB_Receiver-if02-event-mouse";

    Gvar.macros[KEY_X] = &KEY_X_macro;
    Gvar.macros[KEY_P] = &KEY_P_macro;
    Gvar.macros[KEY_D] = &KEY_D_macro;

    std::thread t_active_window(get_active_window, &Gvar);
    std::thread t_messages(catch_message_return, &Gvar);
    std::thread t_mouse(read_mouse, &Gvar, mouse_path);
    std::thread t_keyboard(read_keyboard, &Gvar, keyboard_path);
    
    ////////////////////////////////////////
    std::string line;
    while(1)
    {
        std::getline(std::cin, line);

        if(line == "exit")
        {
            Message M("exit", LX_MESSAGE);
            M.send(Gvar.root_pipe);
            fflush(stdout);
            fflush(stderr);
            return 1;
        } else if(line == "close")
        {
            while(Gvar.unfinished_threadcount>0)
            {
                msleep(100);
            }
            Message M("close", LX_MESSAGE);
            M.send(Gvar.root_pipe);
            fprintf(stderr, "%s.\n", strerror(errno));
            fflush(stdout);
            fflush(stderr);
            close_fun(&Gvar);
            return 0;
        } else
        {
            std::cout << "command not recognised!" << std::endl;
        }
    }
    ////////////////////////////////////////////

    t_mouse.join();
    t_keyboard.join();
    t_active_window.join();
    t_messages.join();
    return 0;
}