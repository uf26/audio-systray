#ifndef NOTIFY_H
#define NOTIFY_H

#include <libnotify/notify.h>
#include "pulse_audio_actions.h"

void notify_setup();
static void notify_send(NotifyNotification* notification,
        const char* message1, const char* message2, 
        const char* icon, int progress, int timeout);
void notify_sink_change(pa_info_list* sink);
void notify_new_default_sink(pa_info_list* sink);

void notify_close();

#endif
