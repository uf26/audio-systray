#include "status_icon.h"
#include "handle_events.h"

XAppStatusIcon* status_icon;

void status_icon_setup() {
    status_icon = xapp_status_icon_new_with_name("audio_systray");
    setup_events();
}

void setup_events() {
    g_signal_connect(status_icon, "button-release-event", 
            G_CALLBACK(handle_button_release), NULL);
    g_signal_connect(status_icon, "scroll-event",
            G_CALLBACK(handle_scroll), NULL);
}

