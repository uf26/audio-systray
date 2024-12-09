#ifndef MENU_H
#define MENU_H

#include <gtk/gtk.h>
#include <xapp-status-icon.h>

GtkMenu* createMenu();
void pop_menu(XAppStatusIcon* status_icon, int x, int y, 
        guint button, guint time, int panel_position, gpointer user_data);

#endif
