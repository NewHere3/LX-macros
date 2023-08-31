#pragma once
#include <string>
#include <X11/Xlib.h>

struct Block
{
    void* contents = NULL;
    size_t size = 0;
    Block(void* ptr = NULL, size_t s = 0);
    Block(Block& t);
    ~Block();
    void store(void* ptr, size_t s);
};

struct Selection
{
    Window window;
    Display* display;
    int screen;
    Selection(Display* d);
    ~Selection();
    int get(std::string source, std::string format, Block& destination);
    int info(std::string source);
    int store(std::string destination, std::string format, Block& source);
};