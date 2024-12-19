#include "notify.h"

static NotifyNotification* volume_notification;
static NotifyNotification* sink_notification;

void notify_setup() {
    if (!notify_init("Audio Tray")) {
        g_error("Failed to initialize libnotify");
    }

    volume_notification = notify_notification_new("Volume", "", NULL);
    sink_notification = notify_notification_new("Sink", "", NULL);
}

void notify_send(NotifyNotification* notification,
        const char* message1, const char* message2, 
        const char* icon, int progress, int timeout) {
    notify_notification_update(notification, message1, message2, icon);

    if (progress != -1)
        notify_notification_set_hint_int32(notification, 
                "value", progress);

    notify_notification_show(notification, NULL);
}

void notify_send_volume(int volume, bool mute) {
    char message[] = "Volume: 100%% [muted]";
    sprintf(message, "Volume: %d%% %s", volume, mute ? "[muted]" : "");

    char icon[50];
    notify_get_icon(icon, volume, mute);

    notify_send(volume_notification,
            message, "", icon, volume, 1000);
}

void notify_send_cycle_sink(char* name) {
    notify_send(sink_notification, "New default sink:", 
            name, "audio-card", -1, 2000);
}

void notify_close() {
    g_object_unref(G_OBJECT(volume_notification));
    g_object_unref(G_OBJECT(sink_notification));

    notify_uninit();
}

void notify_get_icon(char* icon, int volume, bool muted) {
    if (muted) {
        strcpy(icon, "audio-volume-muted-symbolic");
    } else if (volume == 0) {
        strcpy(icon, "audio-volume-muted-symbolic");
    } else if (volume < 33) {
        strcpy(icon, "audio-volume-low-symbolic");
    } else if (volume < 66) {
        strcpy(icon, "audio-volume-medium-symbolic");
    } else {
        strcpy(icon, "audio-volume-high-symbolic");
    }
}

