#pragma once
#include "LX-constants.hpp"
#include <string>
#include <X11/Xlib.h>


struct Overlay
{
    std::string filename;

    int width;
    int height;
    int depth;
    
    Display* display;
    Visual* visual;
    XImage* ximage;
    Window window;
    GC gc;
    XWindowChanges changes;

    FILE* image_log = fopen(IMAGE_LOG_PATH, "a");

    ~Overlay();

    Overlay(std::string path);

    void map(int x, int y);

    void unmap();

    void create_window();

    char* get_image();
    
    XImage* create_ximage();

};