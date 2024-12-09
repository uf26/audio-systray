#include <gtk/gtk.h>
#include <xapp-status-icon.h>

#include "status_icon.h"

int main(int argc, char** argv) {
    gtk_init(&argc, &argv);

    XAppStatusIcon* status_icon = create_status_icon();

    gtk_main();

    return 0;
}
