#include "handle_events.h"
#include "menu.h"
#include "pulse_audio_actions.h"
#include "pulse_audio.h"

#define SCROLL_AMOUNT 5

void handle_quit(GtkMenuItem *item, gpointer user_data) {
    destroy_menu();
    gtk_main_quit();
    pa_quit();
}

void handle_scroll(XAppStatusIcon* icon, int amount,
        XAppScrollDirection direction, guint time,
        gpointer user_data) {
    pa_set_volume(-amount * SCROLL_AMOUNT);
}

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

void handle_sink_select(GtkRadioMenuItem *item, gpointer user_data) {
    if (!gtk_check_menu_item_get_active(GTK_CHECK_MENU_ITEM(item)))
        return;

    pa_info_list* sinks = (pa_info_list*) user_data;
    pa_set_default_sink(sinks);
}

