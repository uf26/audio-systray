#ifndef NOTIFY_H
#define NOTIFY_H

#include <glib.h>

void notify_setup();
static void notify_send(char* type,
        const char* message1, const char* message2, 
        const char* icon, int progress, int timeout);
void notify_sink_change(int volume_percent, gboolean is_muted);
void notify_new_default_sink(const char* sink_name);

void notify_close();

#endif
