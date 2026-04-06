#include "status_icon.h"
#include <gtk/gtk.h>
#include <xapp/libxapp/xapp-status-icon.h>
#include "pulse_actions.h"
#include "utils.h"
#include "menu.h"

#include <pulse/subscribe.h>

static XAppStatusIcon* status_icon;

void handle_button_release(XAppStatusIcon* icon, int x, int y,
        guint button, guint time, int panel_position, gpointer user_data) {
    switch(button) {
        case 1:
            pa_cycle_sink();
            break;
        case 2:
            pa_toggle_mute();
            break;
        case 3:
            pop_menu(icon, x, y, button, time, panel_position, user_data);
            break;
    }
}

void handle_scroll(XAppStatusIcon* icon, int amount,
        XAppScrollDirection direction, guint time,
        gpointer user_data) {
    pa_change_volume(-amount * 5);
}

void status_icon_update_icon(int volume_percent, gboolean is_muted,
        const char* sink_name) {
    xapp_status_icon_set_icon_name(status_icon, get_volume_icon_name(volume_percent, is_muted));

    sink_name = truncate_utf8_string(sink_name, 60);
    char* tooltip = g_strdup_printf("%s - %d%%%s", sink_name, volume_percent, is_muted ? " [muted]" : "");
    g_free((char*)sink_name);

    xapp_status_icon_set_tooltip_text(status_icon, tooltip);

    g_free(tooltip);
}

void status_icon_setup() {
    status_icon = xapp_status_icon_new_with_name("audio_systray");
    status_icon_update_icon(0, TRUE, "No sink");

    g_signal_connect(status_icon, "button-release-event", 
            G_CALLBACK(handle_button_release), NULL);
    g_signal_connect(status_icon, "scroll-event",
            G_CALLBACK(handle_scroll), NULL);
}

void status_icon_cleanup() {
    if (status_icon) {
        g_object_unref(status_icon);
        status_icon = NULL;
    }
}

void setup_events() {
}

