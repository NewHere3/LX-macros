#include "../headers/LX-selections.hpp"
#include <string>
#include <string.h>
#include <iostream>
#include <stdio.h> // printf
#include <X11/Xlib.h>

Block::Block(void* ptr, size_t s)
{
    size = s;
    contents = malloc(size);
    memcpy(contents, ptr, size);
    return;
}

Block::Block(Block& t)
{
    size = t.size;
    contents = malloc(size);
    memcpy(contents, t.contents, size);
    return;
}

Block::~Block()
{
    free(contents);
}

void Block::store(void* ptr, size_t s)
{
    free(contents);
    size = s;
    contents = malloc(size);
    memcpy(contents, ptr, size);
    return;
}

Selection::Selection(Display* d)
{
    display = d;
    screen = DefaultScreen(display);
    window = XCreateSimpleWindow(display, DefaultRootWindow(display), 0, 0, 1, 1, 0, 0, 0);
    //XSelectInput(display, window, 0);
}

Selection::~Selection()
{
    XDestroyWindow(display, window);
}

int Selection::get(std::string source, std::string format, Block& destination)
{
    XEvent event;
	int format_return;
	unsigned long N, size;
	//char * data;
    Atom * data;
	Atom target,
         type = XInternAtom(display, format.c_str(), True),
	     selection = XInternAtom(display, source.c_str() , 0),
		 xsel_data = XInternAtom(display, "XSEL_DATA", 0);
	XConvertSelection(display, selection, type, xsel_data, window, CurrentTime);
	XSync(display, 0);

	XNextEvent(display, &event);
	switch(event.type)
    {
		case SelectionNotify:
            //std::cout << XGetAtomName(display, event.xselection.target) << std::endl;
            if(event.xselection.selection != selection)
            {
                return 1;
                //break;
            }
            if(event.xselection.property)
            {
                XGetWindowProperty(event.xselection.display, event.xselection.requestor, event.xselection.property, 0L,(~0L), 0, AnyPropertyType, &target, &format_return, &size, &N,(unsigned char**)&data);
                if(target == type)
                {
                    destination.store(data, size+1); // XGetWindowProperty() always allocates one EXTRA byte in prop_return and sets it to zero
                    XFree(data);
                }
                else
                {
                    XDeleteProperty(event.xselection.display, event.xselection.requestor, event.xselection.property);
                    return 2;
                }
                XDeleteProperty(event.xselection.display, event.xselection.requestor, event.xselection.property);
            }
        break;
	}   
  return 0;
}

int Selection::store(std::string destination, std::string format, Block& source)
{
    static Atom targets_atom = XInternAtom(display, "TARGETS", 0);
    static Atom xa_atom = XInternAtom(display, "ATOM", 0);
    Atom type = XInternAtom(display, format.c_str(), 0);
	Atom selection = XInternAtom(display, destination.c_str(), 0);
    XEvent event;

	XSetSelectionOwner (display, selection, window, CurrentTime);
	if (XGetSelectionOwner (display, selection) != window) return 1;
    
    for (size_t i = 0; i < 6; i++)
    {
        XNextEvent (display, &event);
		XSelectionRequestEvent * xsr = &event.xselectionrequest;
		XSelectionEvent ev = {0};
		int R = 0;
		switch (event.type)
        {
			case SelectionRequest:
                if (event.xselectionrequest.selection != selection)
                {
                    break;
                }

                ev.type = SelectionNotify, ev.display = xsr->display, ev.requestor = xsr->requestor,
                ev.selection = xsr->selection, ev.time = xsr->time, ev.target = xsr->target, ev.property = xsr->property;

                if (ev.target == targets_atom)
                {
                    R = XChangeProperty (ev.display, ev.requestor, ev.property, xa_atom, 32, PropModeReplace, (unsigned char*)&type, 1);
                    //std::cout << "Property:" << XGetAtomName(display, ev.property) << "Type:" << XGetAtomName(display, xa_atom) << std::endl;

                }
                else if (ev.target == type)
                {
                    R = XChangeProperty(ev.display, ev.requestor, ev.property, type, 8, PropModeReplace, (unsigned char*)source.contents, source.size);
                }
                else
                {
                    ev.property = None;
                }

                if ((R & 2) == 0)
                {
                    XSendEvent (display, ev.requestor, 0, 0, (XEvent *)&ev);
                }
			break;
			case SelectionClear:
			    return 0;
            break;
		}
	}
    XSync(display, 1);
    return 0;
}

int Selection::info(std::string source)
{
    XEvent event;
	int format_return;
	unsigned long N, size;
    Atom * data;
	Atom target,
         type = XInternAtom(display, "TARGETS" , 0),
	     selection = XInternAtom(display, source.c_str() , 0),
		 xsel_data = XInternAtom(display, "XSEL_DATA", 0);
	XConvertSelection(display, selection, type, xsel_data, window, CurrentTime);

	XNextEvent(display, &event);
	switch(event.type)
    {
		case SelectionNotify:
            if(event.xselection.selection != selection)
            {
                std::cout << "error! selections do not mach: " << XGetAtomName(display, event.xselection.selection) << " and " << XGetAtomName(display, selection) << std::endl;
                return 1;
            }
            if(event.xselection.property)
            {
                XGetWindowProperty(event.xselection.display, event.xselection.requestor, event.xselection.property, 0L,(~0L), 0, AnyPropertyType, &target, &format_return, &size, &N,(unsigned char**)&data);
                std::cout << "available formats:" << std::endl;

                for (size_t i = 0; i < size; i++)
                {
                    std::cout << XGetAtomName(display, data[i]) << std::endl;
                }

                XFree(data);
                XDeleteProperty(event.xselection.display, event.xselection.requestor, event.xselection.property);
            }
        break;
	}
  return 0;
}