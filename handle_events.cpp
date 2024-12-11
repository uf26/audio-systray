#include "handle_events.h"
#include "menu.h"
#include "pulse_audio.h"

void handle_quit(GtkMenuItem *item, gpointer user_data) {
    gtk_main_quit();
    pa_quit();
}

void handle_scroll(XAppStatusIcon* icon, int amount,
        XAppScrollDirection direction, guint time,
        gpointer user_data) {
    g_print("Scroll event\n");
}

void handle_button_release(XAppStatusIcon* icon, int x, int y,
        guint button, guint time, int panel_position, gpointer user_data) {
    switch(button) {
        case 1:
            g_print("Left click\n");
            break;
        case 2:
            g_print("Middle click\n");
            break;
        case 3:
            pop_menu(icon, x, y, button, time, panel_position, user_data);
            break;
    }
}

