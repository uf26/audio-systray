#ifndef STATUS_ICON_H
#define STATUS_ICON_H

#include <gtk/gtk.h>
#include <xapp-status-icon.h>

XAppStatusIcon* create_status_icon();

void setup_events(XAppStatusIcon* status_icon);

#endif
