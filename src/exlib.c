#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>
#include <stdio.h>

void fullscreen(Display* dpy, Window win) {
  Atom atoms[2] = { XInternAtom(dpy, "_NET_WM_STATE_FULLSCREEN", False), None };
  XChangeProperty(
      dpy, 
      win, 
      XInternAtom(dpy, "_NET_WM_STATE", False),
      XA_ATOM, 32, PropModeReplace, atoms, 1
  );
}

int main(int argc, char* argv[])
{
    Display* display = XOpenDisplay(NULL);

    XVisualInfo vinfo;

    XMatchVisualInfo(display, DefaultScreen(display), 32, TrueColor, &vinfo);

    XSetWindowAttributes attr;
    attr.colormap = XCreateColormap(display, DefaultRootWindow(display), vinfo.visual, AllocNone);
    attr.border_pixel = 0;
    attr.background_pixel = 0xA0000000;

    Window win = XCreateWindow(display, DefaultRootWindow(display), 0, 0, 100, 100, 0, vinfo.depth, InputOutput, vinfo.visual, CWColormap | CWBorderPixel | CWBackPixel, &attr);
    XSelectInput(display, win, StructureNotifyMask);
    GC gc = XCreateGC(display, win, 0, 0);

    Atom wm_delete_window = XInternAtom(display, "WM_DELETE_WINDOW", 0);
    XSetWMProtocols(display, win, &wm_delete_window, 1);
    fullscreen(display, win);
    XMapWindow(display, win);

    int keep_running = 1;
    XEvent event;

    while (keep_running) {
        XNextEvent(display, &event);

        switch(event.type) {
            case ClientMessage:
                if (event.xclient.message_type == XInternAtom(display, "WM_PROTOCOLS", 1) && (Atom)event.xclient.data.l[0] == XInternAtom(display, "WM_DELETE_WINDOW", 1))
                    keep_running = 0;

                break;

            default:
                break;
        }
    }

    XDestroyWindow(display, win);
    XCloseDisplay(display);
    return 0;
}
