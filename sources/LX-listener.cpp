#include "../headers/LX-listener.hpp"
#include "../headers/LX-global_variables.hpp"
#include "../headers/LX-input.hpp"

#include <fcntl.h>
#include <linux/uinput.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <thread>

int open_real_device(const char* device_path)
{
    int device = open(device_path, O_RDONLY); //open for reading only
    int ret = ioctl(device, EVIOCGRAB, (void*)1); // grab from x
    if (device == -1) //error if opening the file was a failure
    {
        fprintf(stderr, "Cannot open %s: %s.\n", device_path, strerror(errno));
    }
    return device;
}

void read_keyboard(GlobalVariables* Gvar, const char* device_path)
{
    int real_keyboard = open_real_device(device_path); // now real_keyboard represents the end-point file descriptor of the real keyboard

    struct input_event keyboard_event;
    ssize_t n;

    while (1)
    {
        n = read(real_keyboard, &keyboard_event, sizeof(struct input_event));

        if (keyboard_event.type == EV_KEY)
        {
            //std::cout << "Event type: " << keyboard_event.type << " Event code: " << keyboard_event.code << " Event value: " << keyboard_event.value << std::endl;
            Gvar->real_keyboard_state[keyboard_event.code] = keyboard_event.value;
            std::thread x = std::thread(*Gvar->macros[keyboard_event.code], keyboard_event, Gvar);
            x.detach();
        } 
    }
}

void read_mouse(GlobalVariables* Gvar, const char* device_path)
{
    int real_mouse = open_real_device(device_path); // now real_keyboard represents the end-point file descriptor of the real mouse

    struct input_event mouse_event;
    ssize_t n;

    while (1)
    {
        n = read(real_mouse, &mouse_event, sizeof(struct input_event));

        if (mouse_event.type == EV_REL)
        {
            //std::cout << "Event type: " << mouse_event.type << " Event code: " << mouse_event.code << " Event value: " << mouse_event.value << std::endl;
            std::thread x = std::thread(pass_event_on, mouse_event, Gvar);
            x.detach();
        } else if (mouse_event.type == EV_KEY)
        {
            Gvar->real_keyboard_state[mouse_event.code] = mouse_event.value;
            std::thread x = std::thread(*Gvar->macros[mouse_event.code], mouse_event, Gvar);
            x.detach();
        }
    }
}