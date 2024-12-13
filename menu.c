#include "menu.h"
#include "handle_events.h"
#include "pulse_audio_actions.h"

static GtkMenu* menu = NULL;

void destroy_menu() {
    if (menu == NULL)
        return;

    GtkWidget* widget = GTK_WIDGET(menu);

    GList* children = gtk_container_get_children(GTK_CONTAINER(widget));
    for (GList* iter = children; iter != NULL; iter = g_list_next(iter)) {
        GtkWidget* child = GTK_WIDGET(iter->data);
        
        pa_info_list* sink = (pa_info_list*) g_object_get_data(G_OBJECT(child), "sink");
        if (sink != NULL)
            free(sink);
    }

    gtk_widget_destroy(widget);
}

GtkMenu* create_menu() {
    GtkWidget* menu_widget = gtk_menu_new();

    pa_info_list* sinks = pa_get_sinks();
    GtkRadioMenuItem* radio_group = NULL;
    for (pa_info_list* sink = sinks; sink != NULL; sink = sink->next) {
        GtkWidget* sink_item = gtk_radio_menu_item_new_with_label_from_widget(radio_group, 
                sink->name);
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

