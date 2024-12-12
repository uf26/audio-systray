#ifndef HANDLE_EVENTS_H
#define HANDLE_EVENTS_H

#include <gtk/gtk.h>
#include <xapp-status-icon.h>

void handle_quit(GtkMenuItem *item, gpointer user_data);
void handle_scroll(XAppStatusIcon* icon, int amount,
        XAppScrollDirection direction, guint time,
        gpointer user_data);
void handle_button_release(XAppStatusIcon* icon, int x, int y,
        guint button, guint time, int panel_position, gpointer user_data);
void handle_sink_select(GtkRadioMenuItem *item, gpointer user_data);

#endif

