#include "../headers/LX-overlay.hpp"
#include "../headers/LX-global_variables.hpp"
#include "../headers/LX-constants.hpp"

#include <string>
#include <X11/Xmu/WinUtil.h>


Overlay::~Overlay()
{
    XFreeGC(display, gc);
    XDestroyImage(ximage);
    XDestroyWindow(display, window);
}

Overlay::Overlay(std::string path)
{
    display = XOpenDisplay(DISPLAY_NR);
    filename = path;
    visual = DefaultVisual(display, 0);
    ximage = create_ximage();
    create_window();
    gc = XCreateGC(display, window, 0, NULL);
};

void Overlay::map(int x, int y)
{
    changes.x = x - width/2;
    changes.y = y - height/2;
    XConfigureWindow(display, window, CWX | CWY, &changes);
    XMapRaised(display, window);
    XPutImage(display, window, gc, ximage, 0, 0, 0, 0, ximage->width, ximage->height); 
    XFlush(display);
};

void Overlay::unmap()
{
    XUnmapWindow(display, window);
    XFlush(display);
};

void Overlay::create_window()
{
    XVisualInfo vinfo;
    XMatchVisualInfo(display, DefaultScreen(display), 32, TrueColor, &vinfo);
    XSetWindowAttributes atributes;
    atributes.background_pixmap = 0;
    atributes.background_pixel = 0xff0004;//0x0096ff;//0xa0a0a0a0; // red in case of an image isin't handeled properly you will see red window
    atributes.border_pixmap = 0;
    atributes.border_pixel = 0x80808080;
    // atributes.bit_gravity;
    // atributes.win_gravity;
    // atributes.backing_store;
    // atributes.backing_planes;
    // atributes.backing_pixel;
    // atributes.save_under;
    // atributes.event_mask;
    // atributes.do_not_propagate_mask;
    atributes.override_redirect = True;
    // atributes.event_mask;
    atributes.colormap = XCreateColormap(display, DefaultRootWindow(display), vinfo.visual, AllocNone);
    // atributes.cursor;

    window = XCreateWindow(display, DefaultRootWindow(display), 200, 200, ximage->width, ximage->height, 0, 32, InputOutput, vinfo.visual, CWColormap | CWBorderPixel | CWBackPixel | CWOverrideRedirect, &atributes);
    XFlush(display);
};

char* Overlay::get_image()
{
    uint32_t image_data_address;
    uint32_t pixel_count;
    uint16_t bit_depth;

    char* sprite;
    const char * path = filename.c_str();

    fprintf(image_log, "Loading bitmap file: %s\n", path);
    FILE *file;
    file = fopen(path, "rb");

    if(file) {
        if(fgetc(file) == 'B' && fgetc(file) == 'M') {
            fprintf(image_log, "\tbitmap file confirmed.\n");
            fseek(file, 8, SEEK_CUR);
            fread(&image_data_address, 4, 1, file);
            fseek(file, 4, SEEK_CUR);
            fread(&width, 4, 1, file);
            fread(&height, 4, 1, file);
            fseek(file, 2, SEEK_CUR);
            fread(&bit_depth, 2, 1, file);
            fprintf(image_log, "\tbit_depth %d \n", bit_depth);
            if(bit_depth != 32)
            {
                printf("\t(%s) bit depth expected %d is %d", path, 32, bit_depth);
                return NULL;
            }
            else
            {   // if image metadata is correct
                fprintf(image_log, "\timage data address:\t%d\n\twidth:\t\t\t%d pix\n\theight:\t\t\t%d pix\n\tbit depth:\t\t%d bpp\n", image_data_address, width, height, bit_depth);
                pixel_count = width * height;
                depth = bit_depth / 8;
                sprite = (char*) malloc(pixel_count * depth);
                if(sprite)
                {
                    fseek(file, image_data_address, SEEK_SET);
                    int pos = ftell(file);
                    fprintf(image_log, "\tpos %d \n", pos);


                    int lines_read = 0;
                    for (size_t i = height; i > 0; i--) // read pixel lines in reverse order because bmp format stores them reversed
                    {
                        lines_read += fread(sprite+width*i*depth, depth*width, 1, file);
                    }

                    fprintf(image_log, "\tpixel_count: %d    lines_read %d \n",pixel_count, lines_read);
                    if(lines_read != height)
                    {
                        fprintf(image_log, "\t(%s) Read pixel count incorrect. Is %d expected %d", path, lines_read, pixel_count);
                        return NULL;
                    }
                }
                else {
                    fprintf(image_log, "\t(%s) Failed to allocate %d pixels.\n", path, pixel_count);
                    return NULL;
                }
                fclose(file);
                return sprite;
            } // Done loading sprite

        }
        else
        {
            fprintf(image_log, "\t(%s) First two bytes of file are not \"BM\"", path);
            return NULL;
        }
    }
    else
    {
        fprintf(image_log, "\t(%s) Failed to open file", path);
        return NULL;
    }
    return NULL;
}

XImage* Overlay::create_ximage()
{
    char* sprite = get_image();
    fflush(image_log);
    if(sprite == NULL)
    {
        printf("getting the image failed! check the logs to see what went wrong");
        exit(1);
    }
    return XCreateImage(display, visual, 32, ZPixmap, 0, sprite, width, height, 32, 0);
}
