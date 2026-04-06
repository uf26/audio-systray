#include "pulse.h"
#include "notify.h"
#include "utils.h"
#include "sink_list.h"
#include "x11_keygrab.h"
#include "status_icon.h"
#include "menu.h"
#include <gtk/gtk.h>

int main(int argc, char** argv) {
    gtk_init(&argc, &argv);
    pa_init();
    notify_setup();
    x11_init();
    status_icon_setup();

    gtk_main();

    pa_quit();
    notify_close();
    free_icon_cache();
    sink_list_clear();
    status_icon_cleanup();
    destroy_menu();
    x11_cleanup();
}
