#ifndef X11_KEYGRAB_H
#define X11_KEYGRAB_H

#include <gtk/gtk.h>

static GdkFilterReturn x11_event_filter(GdkXEvent* xevent, 
                GdkEvent* event, gpointer data);
static void x11_grab_volume_keys();
void x11_init();

#endif
