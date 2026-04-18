#include "pulse_actions.h"
#include "pulse.h"
#include "sink_list.h"
#include "notify.h"

void pa_change_volume(int step) {
    SinkInfo* default_sink = sink_list_get_default();
    if (!default_sink || step == 0) return;

    pa_volume_t vol_step = (pa_volume_t)(abs(step) * PA_VOLUME_NORM / 100);

    pa_cvolume new_cv = default_sink->volume;

    if (step > 0)
        pa_cvolume_inc(&new_cv, vol_step);
    else
        pa_cvolume_dec(&new_cv, vol_step); 

    for (int i = 0; i < new_cv.channels; i++) {
        if (new_cv.values[i] > PA_VOLUME_NORM)
            new_cv.values[i] = PA_VOLUME_NORM;
    }

    pa_update_sink(default_sink->index, &new_cv, FALSE);
}

void pa_toggle_mute() {
    SinkInfo* default_sink = sink_list_get_default();
    if (!default_sink) return;

    pa_update_sink(default_sink->index, &default_sink->volume, !default_sink->is_muted);
}

void pa_cycle_sink(bool notify) {
    pa_set_default_sink(sink_list_get_default_next()->id->str);
    if (notify)
        notify_new_default_sink(sink_list_get_default()->name->str);
}

void pa_open_pavucontrol() {
    const gchar* path = g_getenv("PAVUCONTROL_PATH");
    const gchar* command = (path != NULL && path[0] != '\0') ? path : "pavucontrol";

    GError* error = NULL;
    if (!g_spawn_command_line_async(command, &error)) {
        g_warning("Failed to open Pavucontrol: %s", error->message);
        g_error_free(error);
    }
}
