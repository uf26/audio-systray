#include "notify.h"
#include "utils.h"

#include <libnotify/notify.h>

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

void notify_sink_change(int volume_percent, gboolean is_muted) {
    const gchar* icon_name = get_volume_icon_name(volume_percent, is_muted);

    GString* volume_str = g_string_new(NULL);
    g_string_printf(volume_str, "%d%% %s", volume_percent, is_muted ? " [muted]" : "");

    notify_send("sink change", volume_str->str, "", icon_name, 
            volume_percent, 1000);

    g_string_free(volume_str, TRUE);
}

void notify_new_default_sink(const char* sink_name) {
    notify_send("new default sink", "New Sink", sink_name,
            "audio-card", -1, 2000);
}

void notify_close() {
    if (notify_is_initted())
        notify_uninit();
}
