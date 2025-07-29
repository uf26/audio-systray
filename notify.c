#include "notify.h"

void notify_setup() {
    if (!notify_init("notify")) {
        g_error("Failed to initialize libnotify");
    }
}

void notify_send(char* type,
        const char* message1, const char* message2, 
        const char* icon, int progress, int timeout) {
    NotifyNotification* notification = notify_notification_new(message1, message2, icon);

    notify_notification_set_hint_string(notification, "x-canonical-private-synchronous", type);

    if (progress != -1)
        notify_notification_set_hint_int32(notification, 
                "value", progress);

    if (timeout != -1)
        notify_notification_set_timeout(notification, timeout);

    notify_notification_show(notification, NULL);

    g_object_unref(G_OBJECT(notification));
}

void notify_sink_change(pa_info_list* sink) {
    bool mute = sink->mute;
    int volume = pa_cvolume_to_int(&sink->volume);
    string icon_name;
    pa_get_icon_name(icon_name);

    string volume_str = "Volume: ";
    pa_volume_text(volume_str + strlen(volume_str));
    notify_send("sink change", volume_str, "", icon_name, 
            volume, 1000);
}

void notify_new_default_sink(pa_info_list* sink) {
    notify_send("new default sink", "New Sink", sink->name, 
            "audio-card", -1, 2000);
}

void notify_close() {
    notify_uninit();
}
