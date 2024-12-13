#include "status_icon.h"
#include "handle_events.h"

#include "pulse_audio_actions.h"

void update_icon(XAppStatusIcon* status_icon) {
    string icon_name;
    pa_get_icon_name(icon_name);
    xapp_status_icon_set_icon_name(status_icon, icon_name);
}

XAppStatusIcon* create_status_icon() {
    XAppStatusIcon* status_icon = xapp_status_icon_new_with_name("audio_systray");
    setup_events(status_icon);

    update_icon(status_icon);

    return status_icon;
}

void setup_events(XAppStatusIcon* status_icon) {
    g_signal_connect(status_icon, "button-release-event", 
            G_CALLBACK(handle_button_release), NULL);
    g_signal_connect(status_icon, "scroll-event",
            G_CALLBACK(handle_scroll), NULL);
}

