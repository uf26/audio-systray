#ifndef STATUS_ICON_H
#define STATUS_ICON_H

#include <glib.h>

void status_icon_update_icon(int volume_percent, gboolean is_muted, const char* sink_name);
void status_icon_setup();
void status_icon_cleanup();

// void setup_events();

#endif
