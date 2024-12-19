#include <gtk/gtk.h>
#include <xapp-status-icon.h>

#include "status_icon.h"
#include "pulse_audio.h"
#include "notify.h"
#include "x11_keygrab.h"

int main(int argc, char** argv) {
    gtk_init(&argc, &argv);
    pa_init();
    notify_setup();
    status_icon_setup();
    x11_init();

    gtk_main();

    return 0;
}
