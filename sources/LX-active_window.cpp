#include "../headers/LX-active_window.hpp"
#include "../headers/LX-global_variables.hpp"
#include "../headers/LX-constants.hpp"
#include "../headers/LX-utilities.hpp"

#include <stdio.h>
#include <X11/Xlib.h>
#include <X11/Xmu/WinUtil.h>

#include <iostream>

void get_active_window(GlobalVariables* Gvar)
{
    Window active_window;
    Display* display = XOpenDisplay(DISPLAY_NR);
    XSetErrorHandler ( catch_bad_window );
    while (1)
    {
        msleep(500);
        get_focused_window(&active_window, display);
        find_parent(&active_window, display);
        register_window_info(active_window, display, Gvar);
    }
    XSetErrorHandler( NULL );
}

int catch_bad_window(Display* d_display, XErrorEvent* d_xerrorevent)
{
    fprintf(stderr, "A bad window has been encountered!\n");
    /*
    active_window_name = "ERROR";
    active_window_application = "ERROR";
    active_window_something = "ERROR";

    if(active_window_error_count < 30)
    {
        active_window_error_count++;
        msleep(100);
        get_active_window();

    } else
    {
        fprintf(stderr, "ACTIVE WINDOW == Bad window; no more searching\n");

    }
    active_window_error_count = 0;
    pthread_cancel(pthread_self());*/
    return 0;
}

void get_focused_window(Window* window, Display* display)
{
    static int revert_to_return;
    XGetInputFocus(display, window, &revert_to_return);
    if(*window == 1)
    {
        fprintf(stderr, "get_focused_window >> *p_window == 1 \n");
        //notify("get_focused_window >> *p_window == 1");
    }
}

void find_parent(Window* window, Display* display)
{
    static Window root, parent, *children = NULL;
    static unsigned int nchildren;

    while (1)
    {
        static unsigned long is_real_window = 0;
        static Atom atom_wm_state = XInternAtom(display, "WM_STATE", False); // https://metacpan.org/pod/X11::Protocol::WM#WM_STATE
        check_window_property(*window, display, atom_wm_state, &is_real_window);

        if (is_real_window == 0)
        {
            XQueryTree(display, *window, &root, window, &children, &nchildren); // pass the old window and get it's parent in it's place
            if (children != NULL) { XFree(children); }

        } else
        {
            return;
        }
    }
}

void register_window_info(Window window, Display* display, GlobalVariables* Gvar)
{
    static bool X_nr = 0;
    static XClassHint* xclass = XAllocClassHint();
    XGetClassHint(display, window, xclass);
    Gvar->Win[X_nr].application =  xclass->res_class;
    Gvar->Win[X_nr].something = xclass->res_name;
    //XFree(xclass);  //this comment might lead to memory leak inside X server, but lets hope it doesn't

    unsigned long nitems;
    unsigned char* name;

    static Atom atom_net_wm_name = XInternAtom(display, "_NET_WM_NAME", False);
    static Atom atom_wm_name = XInternAtom(display, "WM_NAME", False);

    name = check_window_property(window, display, atom_net_wm_name, &nitems);

    if (nitems == 0)
    {
        name = check_window_property(window, display, atom_wm_name, &nitems);
    }
    
    if (nitems == 0)
    {
        fprintf(stderr, "WINDOW NAME NOT FOUND \n");
        Gvar->Win[X_nr].name = "ERROR";
    } else
    {
        //printf("WINDOW NAME: %s \n", name);
        Gvar->Win[X_nr].name = (char*) name;
        XFree(name);
        if( Gvar->Win[X_nr].something != Gvar->Win[!X_nr].something ||
            Gvar->Win[X_nr].application != Gvar->Win[!X_nr].application ||
            Gvar->Win[X_nr].name != Gvar->Win[!X_nr].name)
        {
            Gvar->active = X_nr;
            X_nr = !X_nr;
        }
    }
}

unsigned char* check_window_property(Window window, Display* display, Atom atom, unsigned long* nitems)
{
    static Atom actual_type;
    static int actual_format;
    static unsigned long bytes_after;
    static unsigned char* prop;

    if (XGetWindowProperty(display, window, atom, 0, (~0L), False, AnyPropertyType, &actual_type, &actual_format, nitems, &bytes_after, &prop))
    {
        fprintf(stderr, "XGetWindowProperty failed! \n");
    }

    return prop;
}


