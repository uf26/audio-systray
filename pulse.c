#include "pulse.h"
#include <pulse/glib-mainloop.h>
#include <pulse/pulseaudio.h>
#include <gtk/gtk.h>
#include "notify.h"
#include "sink_list.h"
#include "status_icon.h"
#include "utils.h"
#include <math.h>

#define PA_NO_NOTIFY_TIMEOUT_MS 0

static pa_glib_mainloop* mainloop = NULL;
static pa_context *context = NULL;
static pa_mainloop_api *mainloop_api = NULL;
static size_t pa_restart_attempts = 0;
static struct timespec pa_created_at = {0};

void pa_state_cb(pa_context* c, void* userdata);

gboolean pa_restart(gpointer user_data) {
    g_debug("Attempting to reconnect to PulseAudio...\n");

    clock_gettime(CLOCK_MONOTONIC, &pa_created_at);

    if (context) {
        pa_context_disconnect(context);
        pa_context_unref(context);
    }

    context = pa_context_new(mainloop_api, "audio_systray");

    sink_list_clear();
    
    pa_context_set_state_callback(context, pa_state_cb, NULL);
    
    if (pa_context_connect(context, NULL, PA_CONTEXT_NOFLAGS, NULL) < 0) {
        g_printerr("PulseAudio connection failed immediately.\n");
    }

    return G_SOURCE_REMOVE;
}

void pa_init() {
    clock_gettime(CLOCK_MONOTONIC, &pa_created_at);

    mainloop = pa_glib_mainloop_new(NULL);
    mainloop_api = pa_glib_mainloop_get_api(mainloop);
    context = pa_context_new(mainloop_api, "audio_systray");

    pa_context_set_state_callback(context, pa_state_cb, NULL);
    pa_context_connect(context, NULL, PA_CONTEXT_NOFLAGS, NULL);
}

void pa_quit() {
    pa_created_at.tv_sec = 0;

    pa_context_disconnect(context);
    pa_context_unref(context);
    pa_glib_mainloop_free(mainloop);
}

int volume_to_percent(const pa_cvolume* volume) {
    if (!volume || volume->channels == 0) return 0;
    return round((float)pa_cvolume_avg(volume) * 100 / PA_VOLUME_NORM);
}

void pa_sink_add_or_update_cb(pa_context *c, const pa_sink_info *i, int eol, void *userdata) {
    if (eol > 0 || !i) return;

    gboolean notify, update_icon;
    SinkInfo* info = sink_list_add_or_update(i->description, i->name,
            i->index, i->mute, &i->volume, &notify, &update_icon);
    if (!info)
        return;

    int volume_percent = volume_to_percent(&info->volume);
    if (update_icon)
        status_icon_update_icon(volume_percent, info->is_muted, info->name->str);
    if (notify && check_timeout(pa_created_at, PA_NO_NOTIFY_TIMEOUT_MS))
        notify_sink_change(volume_percent, info->is_muted);
}

void pa_default_sink_change_cb(pa_context *c, const pa_server_info *i, void *userdata) {
    if (!i) return;

    gboolean has_default = sink_list_has_default();
    gboolean notify = sink_list_update_default(i->default_sink_name);
    if (!notify && has_default)
        return;

    SinkInfo* default_sink = sink_list_get_default();
    if (!default_sink)
        return;

    // if (notify && check_timeout(pa_created_at, PA_NO_NOTIFY_TIMEOUT_MS))
    //     notify_new_default_sink(default_sink->name->str);

    status_icon_update_icon(volume_to_percent(&default_sink->volume), default_sink->is_muted, default_sink->name->str);
}

void pa_subscribe_cb(pa_context *c, pa_subscription_event_type_t t, 
        uint32_t idx, void *userdata) {
    int facility = t & PA_SUBSCRIPTION_EVENT_FACILITY_MASK;
    int type = t & PA_SUBSCRIPTION_EVENT_TYPE_MASK;

    if (facility == PA_SUBSCRIPTION_EVENT_SINK) {
        if (type == PA_SUBSCRIPTION_EVENT_NEW || type == PA_SUBSCRIPTION_EVENT_CHANGE) {
            pa_operation_unref(pa_context_get_sink_info_by_index(c, idx, 
                    pa_sink_add_or_update_cb, NULL));
        } else if (type == PA_SUBSCRIPTION_EVENT_REMOVE) {
            sink_list_remove_by_index(idx); 
        }
    } else if (facility == PA_SUBSCRIPTION_EVENT_SERVER) {
        if (type == PA_SUBSCRIPTION_EVENT_CHANGE) {
            pa_operation_unref(pa_context_get_server_info(c, 
                    pa_default_sink_change_cb, NULL));
        }
    }
}

void pa_subscribe_to_sink_changes() {
    pa_context_set_subscribe_callback(context, pa_subscribe_cb, NULL);
    pa_operation* pa_op = pa_context_subscribe(context, 
            PA_SUBSCRIPTION_MASK_SINK | PA_SUBSCRIPTION_MASK_SERVER, NULL, NULL);
    pa_operation_unref(pa_op);
}

void pa_state_cb(pa_context* c, void* userdata) {
    switch (pa_context_get_state(c)) {
        case PA_CONTEXT_READY:
            pa_restart_attempts = 0;
            pa_operation_unref(pa_context_get_sink_info_list(c, pa_sink_add_or_update_cb, NULL));
            pa_operation_unref(pa_context_get_server_info(c, pa_default_sink_change_cb, NULL));

            pa_subscribe_to_sink_changes();
            break;

        case PA_CONTEXT_FAILED:
        case PA_CONTEXT_TERMINATED:
            if (pa_restart_attempts++ > 10) {
                g_printerr("Failed to connect to PulseAudio after 10 attempts, quitting.\n");
                gtk_main_quit();
                return;
            }

            g_timeout_add_seconds(2, (GSourceFunc)pa_restart, NULL);
            break;

        default:;
    };
}

void pa_update_sink(uint32_t index, const pa_cvolume* volume, gboolean is_muted) {
    if (!volume || !context || pa_context_get_state(context) != PA_CONTEXT_READY) {
        g_printerr("Cannot update sink: PulseAudio context not ready.\n");
        return;
    }

    pa_operation_unref(pa_context_set_sink_mute_by_index(context, index, is_muted, NULL, NULL));
    pa_operation_unref(pa_context_set_sink_volume_by_index(context, index, volume, NULL, NULL));
}

void pa_set_default_sink(const char* id) {
    if (!id || !context || pa_context_get_state(context) != PA_CONTEXT_READY) {
        g_printerr("Cannot set default sink: invalid id or PulseAudio context not ready.\n");
        return;
    }

    pa_operation_unref(pa_context_set_default_sink(context, id, NULL, NULL));
}

