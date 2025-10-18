#include "status_icon.h"
#include "handle_events.h"

#include "pulse_audio_actions.h"
#include <pulse/subscribe.h>

XAppStatusIcon* status_icon;

string icon_path;

const char* get_icon_path() {
    return icon_path;
}

void read_icon_path() {
    char* path = getenv("ICON_PATH");
    if (path == NULL) {
        memset(icon_path, 0, MAX_STR_LEN);
        return;
    }

    strncpy(icon_path, path, MAX_STR_LEN - 2);

    icon_path[MAX_STR_LEN - 1] = '\0';
    if (icon_path[strlen(icon_path) - 1] != '/') {
        icon_path[strlen(icon_path)] = '/';
    }
}
    
void update_icon() {
    string icon_name;
    pa_get_icon_name(icon_name);
    xapp_status_icon_set_icon_name(status_icon, icon_name);

    string volume_text;
    pa_volume_text(volume_text);
    xapp_status_icon_set_tooltip_text(status_icon, volume_text);
}

void status_icon_setup() {
    status_icon = xapp_status_icon_new_with_name("audio_systray");
    read_icon_path();
    setup_events();
}

void setup_events() {
    g_signal_connect(status_icon, "button-release-event", 
            G_CALLBACK(handle_button_release), NULL);
    g_signal_connect(status_icon, "scroll-event",
            G_CALLBACK(handle_scroll), NULL);
}

