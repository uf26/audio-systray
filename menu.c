#include "menu.h"
#include "handle_events.h"
#include "pulse_audio_actions.h"

static GtkMenu* menu = NULL;

void destroy_menu() {
    if (menu == NULL)
        return;

    GtkWidget* widget = GTK_WIDGET(menu);
    gtk_widget_destroy(widget);
}

void force_item_length(GtkMenuItem* item, int width) {
    GtkWidget *label = gtk_bin_get_child(GTK_BIN(item));

    char* text = (char*) gtk_label_get_text(GTK_LABEL(label));

    gtk_widget_set_tooltip_text(label, text);

    if (strlen(text) < width)
        return;

    text[width - 4] = '.';
    text[width - 3] = '.';
    text[width - 2] = '.';
    text[width - 1] = '\0';

    gtk_label_set_text(GTK_LABEL(label), text);
}

GtkMenu* create_menu() {
    GtkWidget* menu_widget = gtk_menu_new();

    pa_info_list* sinks = pa_get_sinks();
    GtkRadioMenuItem* radio_group = NULL;
    for (pa_info_list* sink = sinks; sink != NULL; sink = sink->next) {
        GtkWidget* sink_item = gtk_radio_menu_item_new_with_label_from_widget(radio_group, 
                sink->name);
        force_item_length(GTK_MENU_ITEM(sink_item), 50);

        radio_group = GTK_RADIO_MENU_ITEM(sink_item);

        if (sink->is_default)
            gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(sink_item), TRUE);

        g_object_set_data(G_OBJECT(sink_item), "sink", 
                (gpointer) sink);

        gtk_menu_shell_append(GTK_MENU_SHELL(menu_widget), sink_item);
        g_signal_connect(sink_item, "toggled", G_CALLBACK(handle_sink_select), 
                (gpointer) sink);
    }

    gtk_menu_shell_append(GTK_MENU_SHELL(menu_widget), gtk_separator_menu_item_new());

    GtkWidget* pavucontrol_item = gtk_menu_item_new_with_label("Open pavucontrol");
    g_signal_connect(pavucontrol_item, "activate", G_CALLBACK(open_pavucontrol), NULL);
    gtk_menu_shell_append(GTK_MENU_SHELL(menu_widget), pavucontrol_item);

    GtkWidget* quit_item = gtk_menu_item_new_with_label("Quit");
    g_signal_connect(quit_item, "activate", G_CALLBACK(handle_quit), NULL);
    gtk_menu_shell_append(GTK_MENU_SHELL(menu_widget), quit_item);

    gtk_widget_show_all(menu_widget);
    return GTK_MENU(menu_widget);
}

void pop_menu(XAppStatusIcon* status_icon, int x, int y, 
        guint button, guint time, int panel_position, gpointer user_data) {
    if (menu)
        destroy_menu();

    menu = create_menu();
    xapp_status_icon_popup_menu(status_icon, (GtkMenu*)menu, 
            x, y, button, time, panel_position);
}

