#include <gtk/gtk.h>
#include <xapp-status-icon.h>

#include "status_icon.h"
#include "pulse_audio.h"

int main(int argc, char** argv) {
    gtk_init(&argc, &argv);
    pa_init();

    XAppStatusIcon* status_icon = create_status_icon();

    gtk_main();

    return 0;
}
