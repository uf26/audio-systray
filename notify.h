#ifndef NOTIFY_H
#define NOTIFY_H

#include <libnotify/notify.h>

void notify_setup();
static void notify_send(NotifyNotification* notification,
        const char* message1, const char* message2, 
        const char* icon, int progress, int replace_id, int timeout);
void notify_send_volume();
void notify_send_cycle_sink();
void notify_close();

#endif
