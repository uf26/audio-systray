#include "x11_keygrab.h"
#include "pulse_audio_actions.h"
#include "notify.h"

#include <gdk/gdkx.h>
#include <X11/Xlib.h>
#include <X11/XF86keysym.h>

#define NUM_KEYS_TO_GRAB 4

static const char *keysym_names[NUM_KEYS_TO_GRAB] = {
    "XF86AudioRaiseVolume",
    "XF86AudioLowerVolume",
    "XF86AudioMute",
    "XF86AudioMicMute"
};

GdkFilterReturn x11_event_filter(GdkXEvent* xevent, 
        GdkEvent* event, gpointer data) {
    XEvent *xev = (XEvent *)xevent;

    if (xev->type == KeyPress) {
        KeySym keysym = XLookupKeysym(&xev->xkey, 0);
        switch (keysym) {
            case XF86XK_AudioRaiseVolume:
                pa_change_volume(SCROLL_AMOUNT);
                notify_send_volume();
                break;

            case XF86XK_AudioLowerVolume:
                pa_change_volume(-SCROLL_AMOUNT);
                notify_send_volume();
                break;

            case XF86XK_AudioMute:
                pa_toggle_mute();
                notify_send_volume();
                break;

            default:
                break;
        }
    }

    return GDK_FILTER_CONTINUE;
}

void x11_init() {
    GdkDisplay *gdkDisplay = gdk_display_get_default();
    Display *dpy = GDK_DISPLAY_XDISPLAY(gdkDisplay);
    GdkScreen *gdkScreen = gdk_display_get_default_screen(gdkDisplay);
    GdkWindow *gdkRoot = gdk_screen_get_root_window(gdkScreen);
    Window root = GDK_WINDOW_XID(gdkRoot);

    for (int i = 0; i < NUM_KEYS_TO_GRAB; ++i) {
        KeySym keysym = XStringToKeysym(keysym_names[i]);
        if (keysym == NoSymbol) {
            g_printerr("Failed to resolve %s into a keysym\n", keysym_names[i]);
            continue;
        }

        KeyCode key_code = XKeysymToKeycode(dpy, keysym);
        if (key_code == 0) {
            g_printerr("Failed to resolve %s into a keycode\n", keysym_names[i]);
            continue;
        }


        gdk_x11_display_error_trap_push(gdkDisplay);
        XGrabKey(dpy, key_code, AnyModifier, 
                    root, True, GrabModeAsync, GrabModeAsync);

        if (gdk_x11_display_error_trap_pop(gdkDisplay))
            g_printerr("Failed to grab %s\n", keysym_names[i]);
    }
    gdk_window_add_filter(gdkRoot, x11_event_filter, NULL);
}

