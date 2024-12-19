#include "notify.h"
#include "pulse_audio_actions.h"

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
        const char* icon, int progress, int replace_id, int timeout) {
    notify_notification_update(notification, message1, message2, icon);

    if (progress != -1)
        notify_notification_set_hint_int32(notification, 
                "value", progress);

    if (replace_id != -1)
        ;

    notify_notification_show(notification, NULL);
}

void notify_send_volume() {
    string icon_name;
    pa_get_icon_name(icon_name);

    int volume;
    bool mute;
    pa_get_volume(&volume, &mute);

    char volume_str[] = "Volume: 100% [muted]";
    sprintf(volume_str, "Volume: %d%%%s", volume, mute ? " [muted]" : "");
    notify_send(volume_notification, volume_str, "", icon_name, 
            volume, volume, -1);
}

void notify_send_cycle_sink() {
    pa_cycle_sink();

    string default_sink_id;
    pa_get_default_sink_id(default_sink_id);
    pa_sink_info default_sink = pa_get_sink_info();

    notify_send(sink_notification, "New Sink:", default_sink.description, 
            "audio-card", -1, -1, -1);
}

void notify_close() {
    g_object_unref(G_OBJECT(volume_notification));
    g_object_unref(G_OBJECT(sink_notification));

    notify_uninit();
}

