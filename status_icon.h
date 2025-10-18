#ifndef STATUS_ICON_H
#define STATUS_ICON_H

#include <gtk/gtk.h>
#include <xapp/libxapp/xapp-status-icon.h>

const char* get_icon_path();

void update_icon();
void status_icon_setup();

void setup_events();

#endif
