#include "status_icon.h"
#include "handle_events.h"

XAppStatusIcon* create_status_icon() {
    XAppStatusIcon* status_icon = xapp_status_icon_new_with_name("my-app");
    setup_events(status_icon);

    return status_icon;
}

void setup_events(XAppStatusIcon* status_icon) {
    g_signal_connect(status_icon, "button-release-event", 
            G_CALLBACK(handle_button_release), NULL);
    g_signal_connect(status_icon, "scroll-event",
            G_CALLBACK(handle_scroll), NULL);
}

