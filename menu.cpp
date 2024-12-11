#include "menu.h"
#include "handle_events.h"
#include "pulse_audio_actions.h"

GtkMenu* createMenu() {
    GtkWidget* menu_widget = gtk_menu_new();

    GtkWidget* pavucontrol_item = gtk_menu_item_new_with_label("Open pavucontrol");
    g_signal_connect(pavucontrol_item, "activate", G_CALLBACK(open_pavucontrol), NULL);

    GtkWidget* quit_item = gtk_menu_item_new_with_label("Quit");
    g_signal_connect(quit_item, "activate", G_CALLBACK(handle_quit), NULL);

    pa_print_sinks();

    gtk_menu_shell_append(GTK_MENU_SHELL(menu_widget), pavucontrol_item);
    gtk_menu_shell_append(GTK_MENU_SHELL(menu_widget), quit_item);
    gtk_widget_show_all(menu_widget);

    return GTK_MENU(menu_widget);
}

void pop_menu(XAppStatusIcon* status_icon, int x, int y, 
        guint button, guint time, int panel_position, gpointer user_data) {
    GtkMenu* menu = createMenu();

    xapp_status_icon_popup_menu(status_icon, (GtkMenu*)menu, 
            x, y, button, time, panel_position);
}

