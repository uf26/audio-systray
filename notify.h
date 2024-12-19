#ifndef NOTIFY_H
#define NOTIFY_H

#include <libnotify/notify.h>
#include <stdbool.h>

void notify_setup();
static void notify_send(NotifyNotification* notification,
        const char* message1, const char* message2, 
        const char* icon, int progress, int timeout);
void notify_send_volume(int volume, bool mute);
void notify_send_cycle_sink(char* name);
void notify_close();

static void notify_get_icon(char* icon,
        int volume, bool muted);

#endif
