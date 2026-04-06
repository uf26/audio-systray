#include "menu.h"
#include "pulse_actions.h"
#include "sink_list.h"
#include "pulse.h"

static GtkMenu* menu = NULL;

void destroy_menu() {
    if (menu == NULL || !GTK_IS_WIDGET(menu))
        return;

    GtkWidget* widget = GTK_WIDGET(menu);
    gtk_widget_destroy(widget);
    menu = NULL;
}

void force_item_length(GtkMenuItem* item, int width) {
    GtkWidget *label = gtk_bin_get_child(GTK_BIN(item));
    const gchar* original_text = gtk_label_get_text(GTK_LABEL(label));
    
    gtk_widget_set_tooltip_text(label, original_text);

    glong char_len = g_utf8_strlen(original_text, -1);
    if (char_len < width)
        return;

    gchar *truncated = g_utf8_substring(original_text, 0, width - 3);
    gchar *new_text = g_strdup_printf("%s...", truncated);

    gtk_label_set_text(GTK_LABEL(label), new_text);

    g_free(truncated);
    g_free(new_text);
}

void handle_sink_select(GtkRadioMenuItem *item, gpointer user_data) {
    if (!gtk_check_menu_item_get_active(GTK_CHECK_MENU_ITEM(item)))
        return;

    SinkInfo* info = (SinkInfo*)user_data;
    pa_set_default_sink(info->id->str);
}

void handle_quit() {
    gtk_main_quit();
}

GtkMenu* create_menu() {
    GtkWidget* menu_widget = gtk_menu_new();

    GList* it = sink_list_get_all();
    GtkRadioMenuItem* radio_group = NULL;
    for (; it; it = it->next) {
        GtkWidget* sink_item = gtk_radio_menu_item_new_with_label_from_widget(radio_group, 
                ((SinkInfo*)it->data)->name->str);
        force_item_length(GTK_MENU_ITEM(sink_item), 50);

        radio_group = GTK_RADIO_MENU_ITEM(sink_item);

        if (sink_info_is_default((SinkInfo*)it->data))
            gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(sink_item), TRUE);

        g_object_set_data(G_OBJECT(sink_item), "sink", 
                (gpointer)it->data);

        gtk_menu_shell_append(GTK_MENU_SHELL(menu_widget), sink_item);
        g_signal_connect(sink_item, "toggled", G_CALLBACK(handle_sink_select), 
                (gpointer)it->data);
    }

    gtk_menu_shell_append(GTK_MENU_SHELL(menu_widget), gtk_separator_menu_item_new());

    GtkWidget* pavucontrol_item = gtk_menu_item_new_with_label("Open pavucontrol");
    g_signal_connect(pavucontrol_item, "activate", G_CALLBACK(pa_open_pavucontrol), NULL);
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

