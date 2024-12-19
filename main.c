#include <gtk/gtk.h>
#include <xapp-status-icon.h>

#include "status_icon.h"
#include "pulse_audio.h"
#include "notify.h"

int main(int argc, char** argv) {
    gtk_init(&argc, &argv);

    status_icon_setup();
    pa_init();
    notify_setup();

    gtk_main();

    return 0;
}
